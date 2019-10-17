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

#ifndef __MATH__
#define __MATH__

#include <Eigen/Dense>

namespace Tucano
{

namespace Math
{


/**
 * @brief Computes the intersection point between a ray and a sphere
 *
 * @param ray_direction Ray direction vector normalized
 * @param ray_origin Origin of the ray
 * @param sphere_center Center of the sphere in same space as ray origin
 * @param sphere_radius Radius of the sphere
 * @param interection Intersection point if any
 * @return True if found an intersection, false otherwise
 */
inline bool raySphereIntersection (const Eigen::Vector3f& ray_direction, const Eigen::Vector3f& ray_origin, const Eigen::Vector3f& sphere_center, float sphere_radius, Eigen::Vector3f& intersection)
{

    Eigen::Vector3f dir = ray_origin - sphere_center;
    float b = ray_direction.dot(dir);
    float c = dir.dot(dir) - sphere_radius*sphere_radius;
    float disc = b*b - c;

    intersection = Eigen::Vector3f::Zero();
    if (disc < 0.0)
        return false;

    float e = sqrt(disc);
    float t = min(-b-e, -b+e);

    // if by chance tmin < 0, we can still test for tmax
    if (t <= 0.0)
        t = max(-b-e, -b+e);

    if (t > 0.0)
    {
        intersection = ray_origin + ray_direction*t;
        return true;
    }

    return false;
}

/**
 * @brief Computes the intersection between a ray and a plane
 *
 * @param ray_direction Ray direction vector normalized
 * @param ray_origin Origin of the ray
 * @param plane_point A point on the planed
 * @param plane_normal The normalized normal vector of the plane
 * @param interection Intersection point if any
 * @return True if found an intersection, false otherwise

 */
inline bool rayPlaneIntersection (const Eigen::Vector3f& ray_direction, const Eigen::Vector3f& ray_origin, const Eigen::Vector3f& plane_point, const Eigen::Vector3f& plane_normal, Eigen::Vector3f& intersection)
{
    float t = (plane_point - ray_origin).dot(plane_normal) / ray_direction.dot(plane_normal);

    if (t == 0.0)
        return false;

    intersection = ray_origin + ray_direction * t;
    return true;
}

/**
 * @brief Computes the intersection between a ray and a ring in 3D space
 *
 * The ring is defined by the space between two concentric circles (with different radii) on a plane
 * The ray intersects the plane with distance d from the center of the cicle if inner_radius >= d >= outer_radius
 *
 * @param ray_direction Ray direction vector normalized
 * @param ray_origin Origin of the ray
 * @param plane_point A point on the planed
 * @param plane_normal The normalized normal vector of the plane
 * @param inner_radius Radius of smaller circle
 * @param outer_radius Radius of larger circle
 * @param intersection Intersection point between ray and ring
 * @return True if ray intersects ring, false otherwise
 */
inline bool rayRingIntersection (const Eigen::Vector3f& ray_direction, const Eigen::Vector3f& ray_origin, const Eigen::Vector3f& plane_point, const Eigen::Vector3f& plane_normal, float inner_radius, float outer_radius, Eigen::Vector3f& intersection)
{
    Eigen::Vector3f p;
    if (Tucano::Math::rayPlaneIntersection(ray_direction, ray_origin, plane_point, plane_normal, p))
    {
        float dist = (p - plane_point).norm();
        if (dist >= inner_radius && dist <= outer_radius)
        {
            intersection = p;
            return true;
        }
    }
    return false;
}



/**
 * @brief Computes the ray direction given a pixel position and camera matrices
 *
 * @param pixel Screen coordinates in pixels
 * @param viewport Size of viewport
 * @param projection_matrix Camera's projection matrix
 * @param view_matrix Camera's view matrix
 * @return 3D ray direction
 */
inline Eigen::Vector3f rayDirection(const Eigen::Vector2f& pixel, const Eigen::Vector2i& viewport_size, const Eigen::Matrix4f& projection_matrix, const Eigen::Affine3f& view_matrix)
{
     // coordinates in range [-1, 1]    
     // z = near plane in NDC, but it really doesn't matter since we are interested only in the direction, and not a specific point 
    Eigen::Vector4f screen_pos = Eigen::Vector4f(2.0* pixel[0] / (float)viewport_size[0] - 1.0, 1.0 - 2.0*pixel[1] / (float)viewport_size[1], -1.0, 1.0);

    // ray in camera space (where camera is at origin)
    Eigen::Vector4f eye_ray = projection_matrix.inverse() * screen_pos;
    // set w =0.0 since we want the vector, so we dont worry about translation
    eye_ray[3] = 0.0;

    // go to world space
    eye_ray = view_matrix.inverse() * eye_ray;


    Eigen::Vector3f ray = (eye_ray.head(3)).normalized();



    return ray;
}



}
}
#endif
