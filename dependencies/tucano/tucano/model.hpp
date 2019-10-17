#ifndef __MODEL__
#define __MODEL__

#include <vector>
#include <fstream>
#include <Eigen/Dense>

namespace Tucano
{

/**
 * @brief The Model class is a holder for any kind of model, such as meshes, point clouds, surfaces ... that should inherit the model class.
 *
 * It basically contains only information about its spatial location and orientation, that is, a model matrix.
 **/
class Model {

protected:

	/// Shape matrix holds information about intrinsic scaling of other affine transformation of the object
	Eigen::Affine3f shape_matrix = Eigen::Affine3f::Identity();

    /// Model matrix, holds information about the models location and orientation.
    Eigen::Affine3f model_matrix = Eigen::Affine3f::Identity();

    /// Center of the mesh object.
    Eigen::Vector3f objectCenter = Eigen::Vector3f::Zero();

    /// Object's centroid (different from center of bounding box)
    Eigen::Vector3f centroid = Eigen::Vector3f::Zero();

    /// Radius of the mesh bounding sphere.
    float radius = 1.0;

    /// The normalization scale factor, scales the model matrix to fit the model inside a unit cube.
    float normalization_scale = 1.0;

    /// Default color
    Eigen::Vector4f default_color = Eigen::Vector4f (0.7, 0.7, 0.7, 1.0);

public:

    /**
     * Default Constructor.
     */
    Model(void)
    {
        resetModelMatrix();
    }

    /**
     * @brief Returns the default color of the model
     * @return RGBA default color
     */
    Eigen::Vector4f getColor (void)
    {
        return default_color;
    }

    /**
     * @brief Sets the default color of the model
     * @param color Given new default color
     */
    void setColor (const Eigen::Vector4f & color)
    {
        default_color = color;
    }


    /**
     * @brief Returns the center of the axis-aligned bounding box.
     * @return The center of the axis-aligned bounding box.
     */
    virtual Eigen::Vector3f getObjectCenter (void) const
    {
        return objectCenter;
    }

    /**
     * @brief Returns the centroid of the model.
     * @return The centroid of the mesh object. given by the mean position of all vertices.
     */
    virtual Eigen::Vector3f getCentroid (void) const
    {
        return centroid;
    }

    /**
     * Returns the radius of the bounding sphere.
     * The bounding sphere is computed as the distance from the farthest point to the object's centroid.
     * @return Radius of the bounding sphere.
     */
    virtual float getBoundingSphereRadius ( void ) const
    {
        return radius;
    }

    /**
     * @brief Returns the combined model and shape matrix.
     * @return Combine model and shape matrix as an Affine 3f matrix.
     */
    virtual Eigen::Affine3f getShapeModelMatrix ( void ) const
    {
        return model_matrix * shape_matrix;
    }


    /**
     * @brief Returns the shape matrix.
     * @return Shape matrix as an Affine 3f matrix.
     */
    virtual Eigen::Affine3f getShapeMatrix ( void ) const
    {
        return shape_matrix;
    }


    /**
     * @brief Returns the model matrix.
     * @return Model matrix as an Affine 3f matrix.
     */
    virtual Eigen::Affine3f getModelMatrix (void) const
    {
        return model_matrix;
    }


    /**
     * @brief Returns a pointer to the shape matrix.
     * @return Pointer to the shape matrix as an Affine 3f matrix.
     */
    Eigen::Affine3f* shapeMatrix (void)
    {
        return &shape_matrix;
    }

    /**
     * @brief Returns a pointer to the model matrix.
     * @return Pointer to the mdel matrix as an Affine 3f matrix.
     */
    Eigen::Affine3f* modelMatrix (void)
    {
        return &model_matrix;
    }

	/**
	* @brief Sets the model matrix
	* @param m Given new model matrix
	*/
	virtual void setModelMatrix (const Eigen::Affine3f &m)
	{
		model_matrix = m;
	}

    /**
     * @brief Returns the scale factor for fitting the model inside a unit cube.
     * @return Scale factor.
     */
    float getNormalizationScale (void) const
    {
        return normalization_scale;
    }

    /**
     * @brief Normalize model matrix to center and scale model.
     * The model matrix will include a translation to place model's centroid
     * at the origin, and scale the model to fit inside a unit sphere.
     */
    void normalizeModelMatrix (void)
    {
        shape_matrix.scale(normalization_scale);
        shape_matrix.translate(-centroid);
    }

    /**
    * @brief Desnormalize model matrix
    */
    void desnormalizeModelMatrix (void)
    {
        model_matrix.translate(centroid);
        model_matrix.scale(1.0/normalization_scale);
    }

    /**
     * @brief Resets the model matrix.
     */
    virtual void resetModelMatrix (void)
    {
        model_matrix = Eigen::Affine3f::Identity();
    }

    /**
     * @brief Resets the shape matrix.
     */
    virtual void resetShapeMatrix (void)
    {
        shape_matrix = Eigen::Affine3f::Identity();
    }


};

}
#endif
