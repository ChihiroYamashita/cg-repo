#ifndef RAYTRACER_H
#define RAYTRACER_H

#pragma once
#include "TriMesh.h"
#include "Light.h"
#include "Ray.h"
#include "RayHit.h"

void rayTracing(const Object& in_Object,
                const std::vector<AreaLight>& in_AreaLights,
                const Ray& in_Ray,
                RayHit& io_Hit);

void rayTriangleIntersect(const TriMesh& in_Mesh, int in_Triangle_idx, const Ray& in_Ray, RayHit& out_Result);

void rayAreaLightIntersect( const std::vector<AreaLight>& in_AreaLights, const int in_Light_idx, const Ray& in_Ray, RayHit& out_Result );


#endif // RAYTRACER_H

