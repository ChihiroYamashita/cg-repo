#include "shading.h"
#include "RayTracer.h"
#include "qmath.h"
#include "random.h"
#include <math.h>
Eigen::Vector3d computeShading( const Ray& in_Ray, const RayHit& in_RayHit, const Object& in_Object, const std::vector<AreaLight>& in_AreaLights )
{
    // if( in_Ray.depth > MAX_RAY_DEPTH ) return Eigen::Vector3d::Zero();
    // 交差点のシェーディング計算

    // 1. エリアライトに直接衝突した場合
    //Li(x,ωi)の部分の計算
    if (in_RayHit.mesh_idx < 0) // レイがエリアライトに当たった場合
    {
        if (!in_RayHit.isFront) // 裏面に当たった場合は無視
            return Eigen::Vector3d::Zero();

        // エリアライトの光の強度と色を返す
        return in_AreaLights[in_RayHit.primitive_idx].intensity * in_AreaLights[in_RayHit.primitive_idx].color;
    }

    const Eigen::Vector3d x = in_Ray.o + in_RayHit.t * in_Ray.d;
    const Eigen::Vector3d n = computeRayHitNormal( in_Object, in_RayHit );

    Eigen::Vector3d I = Eigen::Vector3d::Zero();

    //ドット演算子は、クラスや構造体の インスタンス のメンバー（変数や関数）にアクセスするために使用
    //この場合Objectのメンバー meshes にアクセス→特定のメッシュ（TriMesh）を取得→
    //→TriMesh のメンバーである material にアクセス→Material 型のメンバーである kd にアクセス
    //kd は Eigen::Vector3d 型。maxCoeff()はベクトルの最大値

    //拡散kd、鏡面ks、屈折kt
    const double kd_max = in_Object.meshes[in_RayHit.mesh_idx].material.kd.maxCoeff();
    const double ks_max = in_Object.meshes[in_RayHit.mesh_idx].material.ks.maxCoeff();
    const double kt_max = in_Object.meshes[in_RayHit.mesh_idx].material.kt.maxCoeff();

    //直接光の計算　とりあえず先に直接光を計算しよう

    if( kd_max > 0.0 )
    {
        I += computeDirectLighting( in_AreaLights, x, n, -in_Ray.d, in_RayHit, in_Object, in_Object.meshes[in_RayHit.mesh_idx].material, in_Ray.depth );
    }

    //randomMT() は、[0, 1) の範囲で一様分布する浮動小数点数を返す。
    const double r = randomMT();

    //間接光をランダムサンプリング

    if( r < kd_max )
    {
        I += in_Object.meshes[in_RayHit.mesh_idx].material.kd.cwiseProduct( computeDiffuseReflection( x, n, -in_Ray.d, in_RayHit, in_Object, in_Object.meshes[in_RayHit.mesh_idx].material, in_AreaLights, in_Ray.depth ) ) / kd_max;
    }
    else if( r < kd_max + ks_max )
    {
        I += in_Object.meshes[in_RayHit.mesh_idx].material.ks.cwiseProduct( computeReflection( x, n, -in_Ray.d, in_RayHit, in_Object, in_Object.meshes[in_RayHit.mesh_idx].material, in_AreaLights, in_Ray.depth ) ) / ks_max;
    }
    else if( r < kd_max + ks_max + kt_max )
    {
        I += in_Object.meshes[in_RayHit.mesh_idx].material.kt.cwiseProduct( computeRefraction( x, n, -in_Ray.d, in_RayHit, in_Object, in_Object.meshes[in_RayHit.mesh_idx].material, in_AreaLights, in_Ray.depth ) ) / kt_max;
    }

    return I;
}

Eigen::Vector3d computeDirectLighting( const std::vector<AreaLight>& in_AreaLights, const Eigen::Vector3d& in_x, const Eigen::Vector3d& in_n, const Eigen::Vector3d& in_w_eye, const RayHit& in_ray_hit, const Object& in_Object, const Material& in_Material, const int depth )
{
    // 初期化
    Eigen::Vector3d direct_light_contribution = Eigen::Vector3d::Zero();

    // 各エリアライトについて計算
    for( int i=0; i<in_AreaLights.size(); i++ )
    {
        const Eigen::Vector3d p_light = sampleRandomPoint( in_AreaLights[i] );
        Eigen::Vector3d w_L = p_light - in_x;
        const double dist = w_L.norm();
        w_L.normalize();

        Eigen::Vector3d n_light = in_AreaLights[i].arm_u.cross( in_AreaLights[i].arm_v );
        const double area = n_light.norm() * 4.0;
        n_light.normalize();
        const double cosT_l = n_light.dot( -w_L );
        if( cosT_l <= 0.0 ) continue;

        // shadow test
        Ray ray; ray.o = in_x; ray.d = w_L; ray.depth = depth + 1;
        ray.prev_mesh_idx = in_ray_hit.mesh_idx; ray.prev_primitive_idx = in_ray_hit.primitive_idx;
        RayHit rh;
        rayTracing( in_Object, in_AreaLights, ray, rh );
        if( rh.mesh_idx < 0 && rh.primitive_idx == i )
        {
            // diffuse
            const double cos_theta = std::max<double>( 0.0, w_L.dot( in_n ) );
            direct_light_contribution += area * in_AreaLights[i].color.cwiseProduct( in_Material.kd ) * in_AreaLights[i].intensity * cos_theta * cosT_l / ( M_PI * dist * dist );
        }
    }

    return direct_light_contribution;
}

Eigen::Vector3d computeDiffuseReflection( const Eigen::Vector3d& in_x, const Eigen::Vector3d& in_n, const Eigen::Vector3d& in_w_eye, const RayHit& in_ray_hit, const Object& in_Object, const Material& in_Material, const std::vector<AreaLight>& in_AreaLights, const int depth )
{
    Eigen::Vector3d bn = in_Object.meshes[in_ray_hit.mesh_idx].vertices[ in_Object.meshes[in_ray_hit.mesh_idx].triangles[in_ray_hit.primitive_idx].x() ] - in_Object.meshes[in_ray_hit.mesh_idx].vertices[ in_Object.meshes[in_ray_hit.mesh_idx].triangles[in_ray_hit.primitive_idx].z() ];

    bn.normalize();

    const Eigen::Vector3d cn = bn.cross( in_n );

    const double theta = acos( sqrt( randomMT() ) );
    const double phi = randomMT() * 2.0 * M_PI;

    const double _dx = sin(theta) * cos(phi);
    const double _dy = cos(theta);
    const double _dz = sin(theta) * sin(phi);

    Eigen::Vector3d w_L = _dx * bn + _dy * in_n + _dz * cn;
    w_L.normalize();

    Ray ray;
    ray.o = in_x; ray.d = w_L; ray.depth = depth + 1;
    ray.prev_mesh_idx = in_ray_hit.mesh_idx; ray.prev_primitive_idx = in_ray_hit.primitive_idx;

    RayHit new_ray_hit;
    rayTracing( in_Object, in_AreaLights, ray, new_ray_hit );

    // exclude the case when the ray directly hits a light source, so as not to
    // double count the direct light contribution (we are already accounting for the
    // direct light contribution in computeDirectLighting() ).
    if( new_ray_hit.mesh_idx >= 0 && new_ray_hit.primitive_idx >= 0 )
    {
        return computeShading( ray, new_ray_hit, in_Object, in_AreaLights );
    }

    return Eigen::Vector3d::Zero();
}

Eigen::Vector3d computeReflection( const Eigen::Vector3d& in_x, const Eigen::Vector3d& in_n, const Eigen::Vector3d& in_w_eye, const RayHit& in_ray_hit, const Object& in_Object, const Material& in_Material, const std::vector<AreaLight>& in_AreaLights, const int depth )
{
    const double e_dot_n = in_w_eye.dot( in_n );
    Eigen::Vector3d w_L = 2.0 * in_n * e_dot_n - in_w_eye;
    w_L.normalize();

    Ray ray;
    ray.o = in_x; ray.d = w_L; ray.depth = depth + 1;
    ray.prev_mesh_idx = in_ray_hit.mesh_idx; ray.prev_primitive_idx = in_ray_hit.primitive_idx;

    RayHit new_ray_hit;
    rayTracing( in_Object, in_AreaLights, ray, new_ray_hit );

    if( new_ray_hit.primitive_idx >= 0 )
    {
        return computeShading( ray, new_ray_hit, in_Object, in_AreaLights );
    }

    return Eigen::Vector3d::Zero();
}

Eigen::Vector3d computeRefraction( const Eigen::Vector3d& in_x, const Eigen::Vector3d& in_n, const Eigen::Vector3d& in_w_eye, const RayHit& in_ray_hit, const Object& in_Object, const Material& in_Material, const std::vector<AreaLight>& in_AreaLights, const int depth )
{
    const double e_dot_n = in_w_eye.dot( in_n );
    const double eta = in_ray_hit.isFront ? in_Material.eta : 1.0 / in_Material.eta;

    const double inside_sqrt = 1.0 - eta*eta * ( 1.0 - e_dot_n * e_dot_n );
    if( inside_sqrt < 0.0 )
    {
        return computeReflection( in_x, in_n, in_w_eye, in_ray_hit, in_Object, in_Material, in_AreaLights, depth );
    }
    else
    {
        Eigen::Vector3d w_t = - in_n * sqrt( inside_sqrt ) - eta * ( in_w_eye - e_dot_n * in_n );
        w_t.normalize();

        Ray ray;
        ray.o = in_x; ray.d = w_t; ray.depth = depth + 1;
        ray.prev_mesh_idx = in_ray_hit.mesh_idx; ray.prev_primitive_idx = in_ray_hit.primitive_idx;

        RayHit new_ray_hit;
        rayTracing( in_Object, in_AreaLights, ray, new_ray_hit );

        if( new_ray_hit.primitive_idx >= 0 )
        {
            return computeShading( ray, new_ray_hit, in_Object, in_AreaLights );
        }

        return Eigen::Vector3d::Zero();
    }
}

Eigen::Vector3d sampleRandomPoint( const AreaLight& in_Light )
{
    const double r1 = 2.0 * randomMT() - 1.0;
    const double r2 = 2.0 * randomMT() - 1.0;
    return in_Light.pos + r1 * in_Light.arm_u + r2 * in_Light.arm_v;
}
