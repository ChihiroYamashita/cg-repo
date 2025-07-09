#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <math.h>
#include<camera.h>
#include <qopengl.h>
#include <Eigen/Dense>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>

#include "Camera.h"
#include "Ray.h"
#include "RayHit.h"
#include "TriMesh.h"
#include "Light.h"

#include "GLPreview.h"
//#include "random.h"

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#include <Eigen/Dense>



void rayTracing( const Object& in_Object, const std::vector<AreaLight>& in_AreaLights, const Ray& in_Ray, RayHit& io_Hit );
void rayTriangleIntersect( const TriMesh& in_Mesh, const int in_Triangle_idx, const Ray& in_Ray, RayHit& out_Result );
Eigen::Vector3d computeRayHitNormal( const Object& in_Object, const RayHit& in_Hit );

#endif // DRAWFILM_H
