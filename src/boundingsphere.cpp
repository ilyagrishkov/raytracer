#include "boundingsphere.hpp"

BoundingSphere::BoundingSphere(float r) {

		radius = r;

		Eigen::Matrix4f m;
		m << 	1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;

		modelMatrix.matrix() = m;
	}

bool BoundingSphere::intersection(Eigen::Vector3f &origin, Eigen::Vector3f &direction) {

	float ray_origin_x = origin.x();
	float ray_origin_y = origin.y();
	float ray_origin_z = origin.z();

	float ray_direction_x = direction.x();
	float ray_direction_y = direction.y();
	float ray_direction_z = direction.z();

	float sphere_center_x = modelMatrix(0, 3);
	float sphere_center_y = modelMatrix(1, 3);
	float sphere_center_z = modelMatrix(2, 3);

	float a = 1;
	float b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
	float c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius * radius);

	float discriminant = b * b - 4 * a * c;

	if(discriminant > 0){
		return true;
	}

	return false;
}

