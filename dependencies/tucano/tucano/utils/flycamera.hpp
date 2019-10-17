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

#ifndef __FLYCAMERA__
#define __FLYCAMERA__


#include "tucano/camera.hpp"
#include "tucano/shapes/coordinateaxes.hpp"
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{


/**
 * @brief Flythrough camera class for manipulating a camera.
 *
 * This class simulates a flythrough camera with 6 DOF.
 **/
class Flycamera : public Tucano::Camera {


protected:

    /// Global movement speed
    float speed;

	/// Current mouse position
	Eigen::Vector2f start_mouse_pos;
	
	/// Camera rotation (view direction)
	Eigen::Matrix3f rotation_matrix;

    /// Default start rotation matrix
    Eigen::Matrix3f default_rotation;

	/// Camera position
	Eigen::Vector3f translation_vector;

	// Default start translation vector
	Eigen::Vector3f default_translation;

	/// Rotation angles
	float rotation_Y_axis;
	float rotation_X_axis;

	// Coordiante axes for rendering flycamera orientation
	Tucano::Shapes::CoordinateAxes axes;

public:

    /**
     * @brief Resets camera to initial position and orientation
     */
    virtual void reset (void)
    {
		start_mouse_pos = Eigen::Vector2f::Zero();
		translation_vector = Eigen::Vector3f::Zero();
		rotation_matrix = Eigen::Matrix3f::Identity();
		default_translation = Eigen::Vector3f (0.0, 0.0, -2.0);
        default_rotation = Eigen::Matrix3f::Identity();
        rotation_X_axis = 0.0;
        rotation_Y_axis = 0.0;
		updateViewMatrix();
    }

    ///Default destructor.
    ~Flycamera() 
	{}

    /**
     * @brief Initializes the view and projection matrices.
     * They are all initialized as Identity matrices, but view is then translated by the defaultTranslation vector.
     */
    void initOpenGLMatrices (void)
    {
        // reset all matrices
        reset();
    }

    /**
     * @brief Default constructor.
     */
    Flycamera ()
    {
        speed = 0.05;
		reset();
        initOpenGLMatrices();

    }


    /**
    * @brief Set default rotation and translation from and Affine3f matrix
    * @param view Affine view matrix to set as default start rotation and translation
    */ 
    void setDefaultView (Eigen::Affine3f view)
    {
        default_rotation = view.rotation();
        default_translation = view.translation();

        cout << "R = " << endl << default_rotation << endl << endl;
        cout << "T = " << default_translation.transpose() << endl << endl;
    }

    /**
     * @brief Returns the default translation for placing the camera outside the trackball sphere.
     * @return The default translation vector for the view matrix
     */
    Eigen::Vector3f getDefaultTranslation (void)
    {
        return default_translation;
    }


	/**
	* @brief Renders the camera's coordinate axis at the lower right corner of the screen.
	*/
	void renderAtCorner (void)
	{
	    float ratio = (viewport[2] - viewport[0]) / (viewport[3] - viewport[1]);
    
        Eigen::Matrix4f rep_projection_matrix = createOrthographicMatrix(-ratio, ratio, -1.0, 1.0, 0.1, 100.0);

        Eigen::Affine3f rep_view_matrix = Eigen::Affine3f::Identity();
        rep_view_matrix.translate( Eigen::Vector3f(1.0, -0.75, -5.0));
        
		Camera lightcam;

		Camera cam;
		cam.setViewMatrix(rep_view_matrix);
		cam.setProjectionMatrix(rep_projection_matrix);
		cam.setViewport(viewport);

		axes.resetModelMatrix();
		axes.modelMatrix()->rotate(rotation_matrix.inverse());
		axes.modelMatrix()->scale(0.3);
		axes.render(cam, lightcam);
	}


	/**
	 * @brief Compose rotation and translation
	 */
	virtual void updateViewMatrix()
	{
		resetViewMatrix();

		// compute X axis restricted to a rotation around Y axis
        Eigen::Vector3f rotX = Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY()) * Eigen::Vector3f::UnitX();
        rotX.normalize();

        // rotate Z axis around Y axis, then rotate new Z axis around X new axis
        Eigen::Vector3f rotZ = Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY()) * Eigen::Vector3f::UnitZ();
        rotZ = Eigen::AngleAxisf(rotation_X_axis, rotX) * rotZ;
        rotZ.normalize();

        // rotate Y axis around X new axis
		Eigen::Vector3f rotY = Eigen::AngleAxisf(rotation_X_axis, rotX) * Eigen::Vector3f::UnitY();
		rotY.normalize();

        rotation_matrix.row(0) = rotX;
        rotation_matrix.row(1) = rotY;
        rotation_matrix.row(2) = rotZ;

        view_matrix.rotate (default_rotation);
		view_matrix.rotate (rotation_matrix);
		view_matrix.translate (default_translation);
		view_matrix.translate (translation_vector);
	}	

  /**
   * @brief Translate the camera respecting only the yaw, not the pitch. A free camera would also consider pitch.
   */
  virtual void translate(float dx, float dy, float dz) {
    Eigen::AngleAxisf yaw = Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY());
    // NOTE(mickvangelderen): The translation vector doesn't do what I would
    // expect, this is a quick-fix. To properly fix it we should re-do the math
    // throughout this class but there's no point. Just negate x and y.
    translation_vector += yaw * Eigen::Vector3f(-dx, -dy, dz) * speed;
  }

    /**
     * @brief Translates the view matrix to the left.
     */
    virtual void strideLeft ( void )
    {
		Eigen::Vector3f dir = (Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY())) * Eigen::Vector3f(1.0, 0.0, 0.0);
		translation_vector += dir * speed;
    }

    /**
     * @brief Translates the view matrix to the right.
     */
    virtual void strideRight ( void )
    {
		Eigen::Vector3f dir = (Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY())) * Eigen::Vector3f(-1.0, 0.0, 0.0);
		translation_vector += dir * speed;
    }

    /**
     * @brief Translates the view matrix back.
     */
    virtual void moveBack ( void )
    {
		Eigen::Vector3f dir = (Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY())) * Eigen::Vector3f(0.0, 0.0, -1.0);
		translation_vector += dir * speed;
    }

    /**
     * @brief Translates the view matrix forward.
     */
    virtual void moveForward ( void )
    {
		Eigen::Vector3f dir = (Eigen::AngleAxisf(rotation_Y_axis, Eigen::Vector3f::UnitY())) * Eigen::Vector3f(0.0, 0.0, 1.0);
		translation_vector += dir * speed;
    }

    /**
     * @brief Translates the view matrix down.
     */
    virtual void moveDown ( void )
    {
		translation_vector += Eigen::Vector3f::UnitY() * speed;
    }

    /**
     * @brief Translates the view matrix up.
     */
    virtual void moveUp ( void )
    {
    	translation_vector -= Eigen::Vector3f::UnitY() * speed;
	}

    /**
    * @brief Nomalizes a screen position to range [-1,1].
    * @param pos Screen position
    * @return Returns position in normalized coordinates.
    */
    Eigen::Vector2f normalizePosition (const Eigen::Vector2f& pos)
    {
        return Eigen::Vector2f ((pos[0]/((viewport[2]-viewport[0])/2.0)) - 1.0,
                                1.0 - (pos[1]/((viewport[3] - viewport[1])/2.0)));
    }

	/**
	 * @brief Begin view direction rotation
	 * @param pos Mouse coordinates
	 **/
	void startRotation ( Eigen::Vector2f pos )
	{
		start_mouse_pos = normalizePosition ( pos );
	}	

	/**
	 * @brief Rotates the camera view direction
	 * @param new_mouse_pos New mouse position
	 */
	virtual void rotate ( Eigen::Vector2f new_mouse_pos )
	{
		Eigen::Vector2f new_position = normalizePosition(new_mouse_pos);
		Eigen::Vector2f dir2d = new_position - start_mouse_pos;
		
		start_mouse_pos = new_position;

		float anglex = dir2d[1]*M_PI;
		float angley = -dir2d[0]*M_PI;
	
		rotation_X_axis += anglex;
		rotation_Y_axis += angley;

        if (rotation_X_axis > 2*M_PI)
            rotation_X_axis -= 2*M_PI;
        if (rotation_X_axis < 0)
            rotation_X_axis += 2*M_PI;
        if (rotation_Y_axis > 2*M_PI)
            rotation_Y_axis -= 2*M_PI;
        if (rotation_Y_axis < 0)
            rotation_Y_axis += 2*M_PI;
    
	}
	
	/**
	 * @brief Rotates the camera view direction around Z axis. Default implementation does nothing.
	 * @param new_mouse_pos New mouse position
	 */
	virtual void rotateZ ( Eigen::Vector2f new_mouse_pos )
	{}

	/**
	 * @brief Changes the camera speed.
	 * @param speed New camera speed.
	 */
	void setSpeed( const float& speed )
	{ this->speed = speed; }
};

}
#endif
