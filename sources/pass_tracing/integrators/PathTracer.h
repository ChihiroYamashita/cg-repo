#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "TriMesh.h" // Object, AreaLight, Ray, RayHit の定義が含まれるヘッダをインクルード
#include "Light.h"
#include "Ray.h"
#include "RayHit.h"

// main.cppから移植する関数の宣言
Eigen::Vector3d sampleRandomPoint(const AreaLight& in_Light);

Eigen::Vector3d computeDirectLighting(const std::vector<AreaLight>& in_AreaLights,
                                      const Eigen::Vector3d& in_x,
                                      const Eigen::Vector3d& in_n,
                                      const Eigen::Vector3d& in_w_eye,
                                      const RayHit& in_ray_hit,
                                      const Object& in_Object,
                                      const Material& in_Material,
                                      int depth);

// 最終的に完全な computeShading になるが、今は直接光のみのシンプルなバージョン
Eigen::Vector3d computeShading(const Ray& in_Ray,
                               const RayHit& in_RayHit,
                               const Object& in_Object,
                               const std::vector<AreaLight>& in_AreaLights);

Eigen::Vector3d computeDiffuseReflection( const Eigen::Vector3d& in_x,
                                         const Eigen::Vector3d& in_n,
                                         const Eigen::Vector3d& in_w_eye,
                                         const RayHit& in_ray_hit,
                                         const Object& in_Object,
                                         const Material& in_Material,
                                         const std::vector<AreaLight>& in_AreaLights, const int depth );

Eigen::Vector3d computeReflection( const Eigen::Vector3d& in_x,
                                  const Eigen::Vector3d& in_n,
                                  const Eigen::Vector3d& in_w_eye,
                                  const RayHit& in_ray_hit,
                                  const Object& in_Object,
                                  const Material& in_Material,
                                  const std::vector<AreaLight>& in_AreaLights,
                                  const int depth );

Eigen::Vector3d computeRefraction( const Eigen::Vector3d& in_x,
                                  const Eigen::Vector3d& in_n,
                                  const Eigen::Vector3d& in_w_eye,
                                  const RayHit& in_ray_hit,
                                  const Object& in_Object,
                                  const Material& in_Material,
                                  const std::vector<AreaLight>& in_AreaLights,
                                  const int depth );
#endif // PATHTRACER_H
