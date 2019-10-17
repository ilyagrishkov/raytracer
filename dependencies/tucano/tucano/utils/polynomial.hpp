/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __POLYNOMIAL__
#define __POLYNOMAIL__

#include "tucano/mesh.hpp"
#include "../constants.hpp"
#include "tucano/shapes/sphere.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{


/**
 * @brief A polynomial function of arbitrary degree, usually fit from given data
 *
 * The polynomial can also be set by hand, by setting its parameters, but usually it is defined
 * by a simple regression method to fit given data
 *
 * Note that our polynomial is defined in 3D space. First fit a plane to the points, then find
 * the polynomial curve on the plane.
 **/
class Polynomial {

private:

    /// Movement speed, arc length increment for each animation step
    float anim_speed = 0.025;

	/// Animation time
	float anim_time = 0.0;

	/// Start/stop animation
	bool animating = false;

    /// Flag to loop animation
    bool loop_animation = false;

	/// Flag to render control points
	bool draw_control_points = false;

	/// Flag to render support plane
	bool draw_plane = false;

	/// Flag to render canonical points and curve
	bool draw_canonical = false;

	/// Camera position at each Key frames
	std::vector< Eigen::Vector4f > key_positions;

	/// Points on curve for rendering purposes
	std::vector< Eigen::Vector4f > curve_points_3d;
	std::vector< Eigen::Vector4f > curve_points_2d;

	/// Curve resolution for rendering (number of points)
	unsigned int resolution = 50;

	/// Polynomial degree
	unsigned int polynomial_degree = 3;

	/// Polynomial coefficients
	Eigen::VectorXf polynomial_coefficients;

	/// Tranformation from 3D space to 2D coordinates on the plane z=0 (where we fit the polynomial)
	Eigen::Affine3f from3dTo2d = Eigen::Affine3f::Identity();

	/// Inverse Transformation, from 2D space back to original 3D space
	Eigen::Affine3f from2dTo3d = Eigen::Affine3f::Identity();

	/// Total path length
	float path_length;

	/// Input approximation plane
	Tucano::Shapes::Plane plane;
	Tucano::Shapes::Plane plane_canonical;

	/// Mesh with path's key positions and computed control points for drawing
	/// smooth curve between key positions
	Mesh curve_3d;
	Mesh curve_2d;

	/// A sphere to visually represent the path's key positions
	Shapes::Sphere sphere;

    /// Path shader, used for rendering the curve
    Shader curve_shader;

	/// To render simple lines
	Shader phong_shader;
	
public:

    /**
     * @brief Default constructor.
     */
    Polynomial (string resource_dir = TUCANO_DEFAULT_RESOURCE_DIR)
    {
		reset();

		string shader_dir = tucano_shader_dir(resource_dir);

        curve_shader.load("phongshader", shader_dir);
		curve_shader.initialize();

		phong_shader.load("phongshader", shader_dir);
		phong_shader.initialize();
    }

    /**
     * @brief Resets the path 
     */
    void reset (void)
    {
		key_positions.clear();
		curve_points_3d.clear();
		curve_points_2d.clear();
		path_length = 0.0;
    }

	/**
	 * @brief Set curve rendering resolution (number of points)
	 */
	void setCurveResolution ( unsigned int r )
	{
		resolution = r;
	}

	/**
	 * @brief Set polynomial degree
	 */
	void setPolynomialDegree ( unsigned int d )
	{
		polynomial_degree = d;
	}

	/**
	* @brief Fill the vertices array
	*/
	void fillVertexData (void)
	{
		fitToPlane();
		computePolynomialRegression(); 
		computePolynomialPoints();

		curve_3d = Mesh();
		curve_3d.loadVertices(curve_points_3d);
	
		curve_2d = Mesh();
		curve_2d.loadVertices(curve_points_2d);
	
		#ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif

	}

	/**
	* @brief Add key position
	* @param pos New key position
	*/
	void addKeyPosition (const Eigen::Vector3f pos)
	{
		Eigen::Vector4f new_pos; 
		new_pos << pos, 1.0;
		key_positions.push_back ( new_pos );

    	if (key_positions.size() > polynomial_degree)
			fillVertexData();
	}

	void addKeyPositions (std::vector < Eigen::Vector3f > positions )
	{
		Eigen::Vector4f new_pos; 
		for (auto pos : positions)
		{
			new_pos << pos, 1.0;	
			key_positions.push_back ( new_pos );
		}

		if (key_positions.size() > polynomial_degree)
			fillVertexData();	
	}

	/**
	* @brief Renders smooth path
	* End points for each Beziér is passed as line_strip
	* and control points as vertex attributes
	* Inside geometry shader each Beziér segment is approximate by linear segments
	* @param camera Current camera for viewing scene
	* @param light Camera representing light (position and orientation)
	*/
    void render (const Tucano::Camera& camera, const Tucano::Camera& light)
    {
		if (key_positions.size() > polynomial_degree)
		{

            // draw path
        	curve_shader.bind();
        
        	curve_shader.setUniform("viewMatrix", camera.getViewMatrix());
        	curve_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        	curve_shader.setUniform("nearPlane", camera.getNearPlane());
        	curve_shader.setUniform("farPlane", camera.getFarPlane());

        	Eigen::Vector4f color (1.0, 0.0, 0.0, 1.0);
        	curve_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity());
        	curve_shader.setUniform("in_Color", color);

			curve_3d.setAttributeLocation(curve_shader);
			curve_3d.bindBuffers();
            glDrawArrays(GL_LINE_STRIP, 0, curve_points_3d.size());
			curve_3d.unbindBuffers();

			if (draw_canonical)
			{
				curve_2d.setAttributeLocation(curve_shader);
				curve_2d.bindBuffers();
        	    glDrawArrays(GL_LINE_STRIP, 0, curve_points_2d.size());
				curve_2d.unbindBuffers();
			}

        	curve_shader.unbind();

			if (draw_control_points)
			{
				phong_shader.bind();
			
				phong_shader.setUniform("viewMatrix", camera.getViewMatrix());
				phong_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
				phong_shader.setUniform("lightViewMatrix", light.getViewMatrix());

				color << 1.0, 1.0, 0.0, 1.0;
				phong_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity());
				phong_shader.setUniform("default_color", color);
				phong_shader.setUniform("has_color", false);

				// render control points
				sphere.setColor( Eigen::Vector4f (0.48, 1.0, 0.16, 1.0) );
				for (unsigned int i = 0; i < key_positions.size(); i++)
				{
					sphere.resetModelMatrix();
					Eigen::Vector3f translation = key_positions[i].head(3);
					sphere.modelMatrix()->translate( translation );
					sphere.modelMatrix()->scale( 0.03 );
					sphere.render(camera, light);
				}

				
				// render control points in canonical plane (2D)
				if (draw_canonical)
				{
					sphere.setColor( Eigen::Vector4f (1.0, 0.16, 0.48, 1.0) );
					for (unsigned int i = 0; i < key_positions.size(); i++)
					{
						sphere.resetModelMatrix();
						Eigen::Vector3f translation = from3dTo2d * Eigen::Vector3f ( key_positions[i].head(3) );
						sphere.modelMatrix()->translate( translation );
						sphere.modelMatrix()->scale( 0.03 );
						sphere.render(camera, light);
					}
				}

			}

			if (draw_plane)
			{
				plane.render(camera, light);
				if (draw_canonical)
				{
					plane_canonical.render(camera, light);
				}
			}

		}

	
		#ifdef TUCANODEBUG
        Misc::errorCheckFunc(__FILE__, __LINE__);
		#endif
    }

	/**
	* @brief Toggle animation
	*/
	void toggleAnimation (void)
	{
		animating = !animating;
	}

    /**
     * @brief Set loop animation flag
     * @param f True for loop animation, false otherwise
     */
    void setLoopAnimation (bool f)
    {
        loop_animation = f;
    }

	/**
	* @brief Toggle render control points
	*/
	void toggleDrawControlPoints (void)
	{
		draw_control_points = !draw_control_points;
	}

	/**
	* @brief Toggle render support planes
	*/
	void toggleDrawSupportPlane (void)
	{
		draw_plane = !draw_plane;
	}

	/**
	* @brief Toggle render points and plane in canonical system
	*/
	void toggleDrawCanonicalCurve (void)
	{
		draw_canonical = !draw_canonical;
	}


	/**
	* @brief Returns if Animation is running or not
	* @return True if animation is running, false otherwise
	*/
	bool isAnimating (void)
	{
		return animating;
	}

	/**
	* @brief Move animation forward one step
	*/
	void stepForward ( void )
	{
    	anim_time += anim_speed;
	    // restart if necessary
		if (anim_time >= path_length)
        {
            if (loop_animation)
    		    anim_time = anim_time - path_length;
            else
            {
                anim_time -= anim_speed;
                animating = false;
            }
        }
	}

	/**
	* @brief Move animation backward one step
	*/
	void stepBackward ( void )
	{
		anim_time -= anim_speed;
		// restart if necessary
		if (anim_time < 0.0)
        {
			anim_time = anim_time + path_length;
        }
	}
	/**
	* @brief Start animation following camera path
	*/
	void startAnimation ( void )
	{
		animating = true;
	}	

	/**
	* @brief Stop animation
	*/
	void stopAnimation ( void )
	{
		animating = false;
	}

	/**
	* @brief Resets animation to first key position
	*/
	void resetAnimation ( void )
	{
		anim_time = 0.0;
	}

	/**
	* @brief Return current animation time
	* @return anim time	
	*/
	float animTime ( void )
	{
		return anim_time;
	}

	/**
	* @brief Return animation speed where 1 covers the whole curve in one step
	* @return Animation speed
	*/
	float animSpeed ( void )
	{
		return anim_speed;
	}
	
	/**
	* @brief Sets the animation speed
	* @param as Given animation speed
	*/
	void setAnimSpeed (float as)
	{
		anim_speed = as;
	}

	/**
	 * @brief Fit 3D points to plane and compute transformation matrix to take plane to XY plane (z=0)
	 */
	void fitToPlane ( void )
	{
		if (key_positions.empty())
			return;

		// compute center of mass
		Eigen::Vector3f centroid = Eigen::Vector3f::Zero();
		for (auto pt : key_positions)
		{
			centroid += pt.head(3);
		}
		centroid /= key_positions.size();
	
		Eigen::MatrixXf Y (3, key_positions.size());

		// fill scatter matrix
		for (unsigned int i = 0; i < key_positions.size(); ++i)
		{
			Y.col(i) = key_positions[i].head(3) - centroid;	
		}

		Eigen::Matrix3f YYt = Y * Y.transpose();

	    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigensolver(YYt);
	    Eigen::Vector3f normal = eigensolver.eigenvectors().col(0);
	    Eigen::Vector3f principal = eigensolver.eigenvectors().col(2);
	    normal.normalize();
		principal.normalize();

	    // compute rotation using normal, so it aligns with unit z
		Eigen::Quaternionf rotZ = Eigen::Quaternionf::FromTwoVectors ( normal, Eigen::Vector3f::UnitZ() ); 

		// align largest eigenvector with unit x
		Eigen::Quaternionf rotX = Eigen::Quaternionf::FromTwoVectors ( rotZ * principal, Eigen::Vector3f::UnitX() );

		// the above operation could probably be achieved just using the eigenvectors matrix as a change of basis matrix, should check for the sake of math (and fun)

		from3dTo2d = Eigen::Affine3f::Identity();
		from3dTo2d.rotate (rotX);
		from3dTo2d.rotate (rotZ);
	    from3dTo2d.translate ( -centroid );

		from2dTo3d = from3dTo2d.inverse();

	    plane.resetModelMatrix();
	    plane.modelMatrix()->translate ( centroid );
		plane.modelMatrix()->rotate (rotZ.inverse());
		plane.modelMatrix()->rotate (rotX.inverse());

		plane_canonical.resetModelMatrix();
	}

	/**
	* @brief Compute the n-th degree polynomial using a Least Square Method
	*
	* See A Sampler of Useful Computational Tools for Applied Geometry, Computer Graphics and Image Processing
	*/
	void computePolynomialRegression ( void )
	{
	
		// prepare the matrices for the linear system
		Eigen::MatrixXf A = Eigen::MatrixXf::Zero(polynomial_degree+1, polynomial_degree+1);
		Eigen::VectorXf b = Eigen::VectorXf::Zero(polynomial_degree+1);

		Eigen::Vector3f plane_point;
		for (unsigned int k = 0; k < key_positions.size(); ++k)
		{
			// project points to plane z = 0
			plane_point = from3dTo2d * Eigen::Vector3f(key_positions[k].head(3));

			for (unsigned int j = 0; j <= polynomial_degree; j++) // row
			{
				for (unsigned int i = 0; i <= polynomial_degree; i++) // column
				{
					A(j,i) += pow ( plane_point[0], (polynomial_degree-j)+(polynomial_degree-i) );
				}
				b[j] += pow ( plane_point[0], polynomial_degree-j ) * plane_point[1];
			}
		}

		polynomial_coefficients.resize (polynomial_degree+1);
		polynomial_coefficients = (A.transpose() * A).inverse() * A.transpose() * b;

	}

	Eigen::Vector3f evaluatePolynomial ( float x )
	{
		float y = 0.0;
		for (unsigned int i = 0; i <= polynomial_degree; ++i)
		{
			// coefficients are stored in inverse order
			y += polynomial_coefficients[polynomial_degree - i] * pow ( x, i );
		}
		Eigen::Vector3f pt (x, y, 0.0);
		return pt;
	}

	Eigen::Vector3f evaluatePolynomial ( const Eigen::Vector3f pt )
	{
		
		Eigen::Vector3f pt2d = from3dTo2d * pt;
		pt2d = evaluatePolynomial ( pt2d[0] );
		return from2dTo3d * pt2d;
	}

	/**
	* @brief Compute the n-th degree polynomial using a Least Square Method
	*
	* See A Sampler of Useful Computational Tools for Applied Geometry, Computer Graphics and Image Processing
	*/
	void computePolynomialPoints ( void )
	{
		Eigen::Vector3f point2d;
		Eigen::Vector4f point3d;
		Eigen::Vector3f first = from3dTo2d * Eigen::Vector3f( key_positions[0].head(3) );
		Eigen::Vector3f last = from3dTo2d * Eigen::Vector3f( key_positions.back().head(3) );
		float step = (last[0] - first[0])/(float)resolution;

		curve_points_3d.clear();
		curve_points_2d.clear();
		for (unsigned int i = 0; i < resolution; ++i)
		{
			float x = first[0] + step*i;
			point2d = evaluatePolynomial ( x );
			point3d << point2d, 1.0;
			curve_points_2d.push_back ( point3d );
			point3d << from2dTo3d * point2d, 1.0;
			curve_points_3d.push_back ( point3d );
		}

	}

	/*
    void writeToFile (const string filename)
    {
        ofstream file;
        file.open (filename.c_str());
        
        file << key_positions.size() << endl;
        for (unsigned int i = 0; i < key_positions.size(); ++i)
        {
            file << key_positions[i].transpose() << endl; 
            file << key_quaternions[i].w() << " " << key_quaternions[i].vec().transpose() << endl;
            file << key_directions[i].w() << " " << key_directions[i].vec().transpose() << endl;
            file << key_intervals[i] << endl;

        }
        file.close();
    }

    void loadFromFile (const string filename)
    {
        ifstream file (filename.c_str());

        if (file.is_open())
        {        
            string line;
            float elem;
            int num;

            Eigen::Quaternion<float> q;
            Eigen::Vector4f v;

            getline(file, line);
            istringstream iss(line);
            iss >> num;

            for (int i = 0; i < num; ++i)
            {
                // read key position
                getline(file, line);
                iss.clear();
                iss.str(line);
                iss >> v[0] >> v[1] >> v[2] >> v[3];
                key_positions.push_back(v);

                // read key quaternion
                getline(file, line);
                iss.clear();
                iss.str(line);
                iss >> q.w() >> q.vec()[0] >> q.vec()[1] >> q.vec()[2];
                key_quaternions.push_back(q);

                // read key direction
                getline(file, line);
                iss.clear();
                iss.str(line);
                iss >> q.w() >> q.vec()[0] >> q.vec()[1] >> q.vec()[2];
                key_directions.push_back(q);

                // read key interval
                getline(file, line);
                iss.clear();
                iss.str(line);
                iss >> elem;
                key_intervals.push_back(elem);
            }
        }
        if (key_positions.size() > 1)
        {
            fillVertexData();
        }
    }
	*/

};

}
#endif
