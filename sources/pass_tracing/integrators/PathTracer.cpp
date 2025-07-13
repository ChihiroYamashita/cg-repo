#include "PathTracer.h"
#include "Intersection.h" // computeRayHitNormal を使うため
#include "random.h"       // randomMT() を使うため
#include <algorithm>      // std::max のため

/**
 * @brief エリアライト上の中から一つの座標を選ぶ関数
 * @details この関数は、指定されたエリアライト上のランダムな点を一様分布でサンプリングします。
 * エリアライトの中心位置と2つの方向ベクトル（`arm_u`, `arm_v`）を利用して、エリア内の任意の点を計算します。
 *
 * @param[in] in_Light サンプリング対象のエリアライト
 *   - `pos`: ライトの中心位置
 *   - `arm_u`: ライトの一方向のベクトル（ライトの幅を表す）
 *   - `arm_v`: ライトのもう一方向のベクトル（ライトの高さを表す）
 *
 * @return Eigen::Vector3d ランダムにサンプリングされた点の3次元座標
 *
 * @note エリアライトの形状は矩形であると仮定しています。
 *
 * ### コード解説:
 * @code
 * // [-1, 1] の範囲で一様分布の乱数を生成します
 * const double r1 = 2.0 * randomMT() - 1.0;
 * const double r2 = 2.0 * randomMT() - 1.0;
 *
 * // エリアライトの中心位置（in_Light.pos）を基準に、2つの方向ベクトル（arm_u, arm_v）をスケールして加算
 * // - r1 * in_Light.arm_u: ライトの幅方向のランダムなオフセット
 * // - r2 * in_Light.arm_v: ライトの高さ方向のランダムなオフセット
 * return in_Light.pos + r1 * in_Light.arm_u + r2 * in_Light.arm_v;
 * @endcode
 *
 * 上記コードの手順：
 * 1. `randomMT()` を使って [0, 1] の範囲で乱数を生成。
 * 2. 範囲を [-1, 1] に変換し、それをエリアライトの方向ベクトルにスケール。
 * 3. 中心位置 `pos` に方向ベクトルのオフセットを加算してランダムな点を生成。
 *
 * @see randomMT
 * @see AreaLight
 */
Eigen::Vector3d sampleRandomPoint( const AreaLight& in_Light )
{
    const double r1 = 2.0 * randomMT() - 1.0;
    const double r2 = 2.0 * randomMT() - 1.0;
    return in_Light.pos + r1 * in_Light.arm_u + r2 * in_Light.arm_v;
}

/**
 * @brief 直接光（エリアライトの寄与）を計算する関数
 * @details この関数は、指定されたエリアライトのリストに基づき、交差点における直接光の寄与を計算します。
 * エリアライトの影響をシーン内の各光源について計算し、陰影（影の有無）や法線方向を考慮した寄与を合計します。
 *
 * @param[in] in_AreaLights エリアライトのリスト（位置、方向、光の色、強度を含む）
 * @param[in] in_x レイとオブジェクトの交差点（3D空間の座標）
 * @param[in] in_n 交差点での法線ベクトル
 * @param[in] in_w_eye 視線方向（カメラから交差点へのベクトル）
 * @param[in] in_ray_hit 交差情報（交差距離、メッシュのインデックスなど）
 * @param[in] in_Object シーン内のオブジェクト
 * @param[in] in_Material 交差したオブジェクトのマテリアル情報
 * @param[in] depth 現在のレイの追跡深度（反射や屈折計算時に再帰的に利用）
 *
 * @return Eigen::Vector3d 交差点での直接光の寄与（RGB値）
 *
 * @details **処理の流れ**:
  * ### 処理の流れ
 *
 * #### 1. 初期化
 * 光の寄与を初期化します。
 * @code
 * Eigen::Vector3d direct_light_contribution = Eigen::Vector3d::Zero();
 * @endcode
 *
 * #### 2. エリアライトの計算ループ
 * エリアライトごとに以下の処理を繰り返します。
 * @code
 * for (int i = 0; i < in_AreaLights.size(); i++) {
 *     const Eigen::Vector3d p_light = sampleRandomPoint(in_AreaLights[i]); // ランダムな点をサンプリング
 *     Eigen::Vector3d w_L = p_light - in_x;                               // 光源方向のベクトル
 *     const double dist = w_L.norm();                                     // 光源までの距離
 *     w_L.normalize();                                                   // ベクトルを正規化
 * @endcode
 *
 *
 * #### 3. エリアライトの法線と面積の計算
 * 法線ベクトルと面積を計算し、裏面のライトを除外します。
 *
 * @image html 1.png "エリアライトの面積計算"
 * @code
 *     Eigen::Vector3d n_light = in_AreaLights[i].arm_u.cross(in_AreaLights[i].arm_v);
 *   //外積は大きさが面積、ベクトルが法線になるのがポイント！
 *     const double area = n_light.norm() * 4.0; // 面積計算
 *     n_light.normalize();
 *     const double cosT_l = n_light.dot(-w_L); // 法線と光方向の内積
 *     if (cosT_l <= 0.0) continue;            // ライトが裏面の場合はスキップ
 * @endcode
 *
 * #### 4. シャドウテスト
 * 光源に到達する経路が遮られていないかを確認します。
 * @code
 *     Ray ray;
 *     ray.o = in_x; // レイの始点
 *     ray.d = w_L; // レイの方向
 *     ray.depth = depth + 1;
 *     ray.prev_mesh_idx = in_ray_hit.mesh_idx; // 直前の交差情報を保持
 *     ray.prev_primitive_idx = in_ray_hit.primitive_idx;
 *
 *     RayHit rh;
 *     rayTracing(in_Object, in_AreaLights, ray, rh);
 * @endcode
 *
 * #### 5. 光の寄与を計算
 *
 * モンテカルロ法のコーディングの方法については、@ref additional_info_coding "モンテカルロ法のコーディングの方法" を参照してください。
 * 遮られていない場合、光源の寄与を計算して加算します。
 * レイが交差したオブジェクトのインデックスRayHit rhのmesh_idxを参照する
 * rh.mesh_idx < 0: 他のオブジェクトに遮られていない（エリアライトに直接到達した）。
 * rh.primitive_idx == i: 到達したエリアライトが現在計算中のエリアライト（インデックス i）である。
 * @image html 2.png "光源計算（導出はノート参照）"　width=100
 * @code
 *     if (rh.mesh_idx < 0 && rh.primitive_idx == i) {
 *         const double cos_theta = std::max<double>(0.0, w_L.dot(in_n)); // 法線と光方向の内積
 *         direct_light_contribution +=
 *             area * in_AreaLights[i].color.cwiseProduct(in_Material.kd) *
 *             in_AreaLights[i].intensity * cos_theta * cosT_l / (M_PI * dist * dist);
 *     }
 * }
 * @endcode
 *
 * #### 6. 結果を返却
 * 最終的な直接光の寄与を返却します。
 * direct_light_contribution は 3 次元ベクトル：
 * RGB の色成分を表現するため(各0~1で表現される)
 * @code
 * return direct_light_contribution;
 * @endcode
 *
 * @see sampleRandomPoint
 * @see rayTracing
 * @see AreaLight
 *
 *
 * @see AreaLight
 * @see Material
 * @see Object
 * @see RayHit
 * @see sampleRandomPoint
 *
 *
 */
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

/**
 * @brief レイのシェーディング（光の寄与）を計算する関数
 * @details レイがシーン内のオブジェクトやエリアライトと交差した場合に、その交差点での光の寄与（直接光、反射、屈折、拡散反射）を計算します。
 * 交差したオブジェクトのマテリアル特性に基づき、確率的に光の反射や屈折を考慮します。
 *
 * @param[in] in_Ray 入射レイ（始点 `o` と方向ベクトル `d`、レイの深度を含む）
 * @param[in] in_RayHit レイとオブジェクト/エリアライトの交差情報（交差距離 `t`、UV座標 `alpha, beta`、表面フラグ `isFront`）
 * @param[in] in_Object シーン内のオブジェクト（複数のメッシュとマテリアルを含む）
 * @param[in] in_AreaLights シーン内のエリアライト（位置、光の強度、光の色、形状を含む）
 *
 * @return Eigen::Vector3d 光の寄与ベクトル 交差点での最終的な光の強度（RGBの色ベクトル）
 *
 * @see computeDirectLighting
 * @see computeDiffuseReflection
 * @see computeReflection
 * @see computeRefraction
 * @see RayHit
 * @see Material
 *
 * @startuml
 * main -> computeShading : シェーディング計算開始
 * computeShading -> computeRayHitNormal : 法線ベクトルを計算
 * computeShading -> computeDirectLighting : 直接光の計算（kd_max > 0）
 * computeDirectLighting ->computeShading: RGB値の3次元ベクトルdirect_light_contribution を返却
 * computeShading -> computeDiffuseReflection : 拡散反射を計算（確率 kd_max に基づく）
 * computeShading -> computeReflection : 鏡面反射を計算（確率 ks_max に基づく）
 * computeShading -> computeRefraction : 屈折を計算（確率 kt_max に基づく）
 * computeShading -> computeShading : 光の寄与を合算
 * computeShading -> main : シェーディング結果を返却
 * @enduml
 *
 * @image html computeShading1.png "シェーディング計算フロー概要"
 *
 * @details **処理の概要**:
 * - エリアライトとの交差があれば、その光の強度と色を直接返却します。
 * - オブジェクトとの交差があれば、次の手順でシェーディングを計算します：
 *   1. **直接光**: エリアライトからの直接光の寄与を計算。
 *   2. **拡散反射**: オブジェクトの拡散反射を計算（確率的に `kd_max` に基づく）。
 *   3. **鏡面反射**: レイの反射光を計算（確率的に `ks_max` に基づく）。
 *   4. **屈折**: レイの屈折光を計算（確率的に `kt_max` に基づく）。
 * - マテリアルの確率的サンプリングを用いて、各光の寄与を統合します。
 *
 *  * ### 処理の流れ
 *
 * #### 1. エリアライトに直接衝突した場合
 * レイがエリアライトに衝突した場合、その光の強度と色を計算します。
 * @code
 * if (in_RayHit.mesh_idx < 0) // レイがエリアライトに当たった場合
 * {
 *     if (!in_RayHit.isFront) // 裏面に当たった場合は無視
 *         return Eigen::Vector3d::Zero();
 *
 *     // エリアライトの光の強度と色を返す
 *     return in_AreaLights[in_RayHit.primitive_idx].intensity * in_AreaLights[in_RayHit.primitive_idx].color;
 * }
 * @endcode
 *
 * #### 2. 交差点と法線の計算
 * 交差点とその法線を計算します。
 * @code
 * const Eigen::Vector3d x = in_Ray.o + in_RayHit.t * in_Ray.d; // 交差点の座標
 * const Eigen::Vector3d n = computeRayHitNormal(in_Object, in_RayHit); // 交差点での法線ベクトル
 * Eigen::Vector3d I = Eigen::Vector3d::Zero(); // 初期化
 * @endcode
 *
 * #### 3. マテリアル特性の取得
 * 拡散、鏡面反射、屈折の最大寄与率を取得します。
 * @code
 * const double kd_max = in_Object.meshes[in_RayHit.mesh_idx].material.kd.maxCoeff();
 * const double ks_max = in_Object.meshes[in_RayHit.mesh_idx].material.ks.maxCoeff();
 * const double kt_max = in_Object.meshes[in_RayHit.mesh_idx].material.kt.maxCoeff();
 * @endcode
 *
 * #### 4. 直接光の計算
 * 拡散反射の最大寄与率 `kd_max` が正の場合、直接光を計算します。
 * @code
 * if (kd_max > 0.0)
 * {
 *     I += computeDirectLighting(
 *         in_AreaLights, x, n, -in_Ray.d, in_RayHit,
 *         in_Object, in_Object.meshes[in_RayHit.mesh_idx].material, in_Ray.depth
 *     );
 * }
 * @endcode
 *
 * #### 5. 間接光のランダムサンプリング
 * ランダム値に基づいて間接光の寄与を計算します。
 * @code
 * const double r = randomMT(); // ランダム値 [0, 1) を生成
 *
 * if (r < kd_max)
 * {
 *     // 拡散反射の寄与を計算
 *     I += in_Object.meshes[in_RayHit.mesh_idx].material.kd.cwiseProduct(
 *         computeDiffuseReflection(
 *             x, n, -in_Ray.d, in_RayHit, in_Object,
 *             in_Object.meshes[in_RayHit.mesh_idx].material, in_AreaLights, in_Ray.depth
 *         )
 *     ) / kd_max;
 * }
 * else if (r < kd_max + ks_max)
 * {
 *     // 鏡面反射の寄与を計算
 *     I += in_Object.meshes[in_RayHit.mesh_idx].material.ks.cwiseProduct(
 *         computeReflection(
 *             x, n, -in_Ray.d, in_RayHit, in_Object,
 *             in_Object.meshes[in_RayHit.mesh_idx].material, in_AreaLights, in_Ray.depth
 *         )
 *     ) / ks_max;
 * }
 * else if (r < kd_max + ks_max + kt_max)
 * {
 *     // 屈折の寄与を計算
 *     I += in_Object.meshes[in_RayHit.mesh_idx].material.kt.cwiseProduct(
 *         computeRefraction(
 *             x, n, -in_Ray.d, in_RayHit, in_Object,
 *             in_Object.meshes[in_RayHit.mesh_idx].material, in_AreaLights, in_Ray.depth
 *         )
 *     ) / kt_max;
 * }
 * @endcode
 *
 * #### 6. 結果を返却
 * 最終的な光の寄与ベクトルを返却します。
 * @code
 * return I;
 * @endcode
 *
 * ### コード解説:
 * @code
 * const double kd_max = in_Object.meshes[in_RayHit.mesh_idx].material.kd.maxCoeff();
 * @endcode
 *
 * ドット演算子は、クラスや構造体の インスタンス のメンバー（変数や関数）にアクセスするために使用
 * この場合Objectのメンバー meshes にアクセス→特定のメッシュ（TriMesh）を取得→
 * →TriMesh のメンバーである material にアクセス→Material 型のメンバーである kd にアクセス
 * kd は Eigen::Vector3d 型。maxCoeff()はeigenベクトルで最大値を返す。
 *
 * @note 光の寄与はマテリアルの反射率や屈折率を用いて重み付けされ、確率的にサンプリングされます。
 */
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

    // 2. 交差点と法線を計算
    const Eigen::Vector3d x = in_Ray.o + in_RayHit.t * in_Ray.d;
    const Eigen::Vector3d n = computeRayHitNormal( in_Object, in_RayHit );

    Eigen::Vector3d I = Eigen::Vector3d::Zero();

    //デバッグ
    /*
    // 3. ★直接光のみを計算して返す★
    // (間接光、反射、屈折のロジックはまだ移植しない)
    return computeDirectLighting( in_AreaLights, x, n, -in_Ray.d, in_RayHit, in_Object, in_Object.meshes[in_RayHit.mesh_idx].material, in_Ray.depth );
*/


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

/**
 * @brief 拡散反射の寄与を計算する関数
 * @details 入射点での拡散反射光の寄与をランダムサンプリングに基づいて計算します。
 * 拡散反射は、表面法線に対するランダムな方向に基づいてサンプリングされます。
 *
 * @param[in] in_x 交差点の位置（3D座標）
 * @param[in] in_n 交差点での法線ベクトル
 * @param[in] in_w_eye 視線方向ベクトル（カメラから交差点への方向）
 * @param[in] in_ray_hit 交差情報（交差距離、メッシュ、プリミティブインデックスなど）
 * @param[in] in_Object シーン内のオブジェクト（メッシュ、マテリアル情報を含む）
 * @param[in] in_Material 交差点のマテリアル情報
 * @param[in] in_AreaLights シーン内のエリアライトリスト
 * @param[in] depth 現在のレイの追跡深度
 *
 * @return Eigen::Vector3d 拡散反射光の寄与（RGB値）
 *
 * ### 処理の流れ
 *
 * #### 1. 接線ベクトルの計算
 * 法線ベクトルに直交する基底ベクトル（接線ベクトル）を計算します。
 * @code
 * Eigen::Vector3d bn = in_Object.meshes[in_ray_hit.mesh_idx].vertices[in_Object.meshes[in_ray_hit.mesh_idx].triangles[in_ray_hit.primitive_idx].x()]
 *                     - in_Object.meshes[in_ray_hit.mesh_idx].vertices[in_Object.meshes[in_ray_hit.mesh_idx].triangles[in_ray_hit.primitive_idx].z()];
 * bn.normalize();
 * const Eigen::Vector3d cn = bn.cross(in_n); // 接線ベクトルを計算
 * @endcode
 *
 * #### 2. ランダム方向のサンプリング
 * ランダムな方向を生成し、基底ベクトルに基づいてローカル座標系での拡散方向を計算します。
 * 詳しくはノート参照
 * @code
 * const double theta = acos(sqrt(randomMT())); // ランダムな角度 θ
 * const double phi = randomMT() * 2.0 * M_PI; // ランダムな角度 φ
 * const double _dx = sin(theta) * cos(phi);
 * const double _dy = cos(theta);
 * const double _dz = sin(theta) * sin(phi);
 * Eigen::Vector3d w_L = _dx * bn + _dy * in_n + _dz * cn; // ワールド座標系での方向ベクトル
 * w_L.normalize();
 * @endcode
 *
 * #### 3. 新しいレイの生成と追跡
 * サンプリングされた方向に基づいて新しいレイを生成し、交差判定を行います。
 * @code
 * Ray ray;
 * ray.o = in_x;            // レイの始点
 * ray.d = w_L;             // レイの方向
 * ray.depth = depth + 1;   // 深度をインクリメント
 * ray.prev_mesh_idx = in_ray_hit.mesh_idx; // 現在の交差情報を保持
 * ray.prev_primitive_idx = in_ray_hit.primitive_idx;
 *
 * RayHit new_ray_hit;
 * rayTracing(in_Object, in_AreaLights, ray, new_ray_hit); // 新しいレイの追跡
 * @endcode
 *
 * #### 4. 二重計算の回避
 * #####二重計算の発生
* - 間接光のレイ追跡（rayTracing）中に、レイが直接エリアライトに衝突する場合があります。
* - このエリアライトへの直接衝突は、すでに computeDirectLighting で計算されています。
* - そのため、エリアライトへの直接的な寄与を再度計算すると、光の寄与が重複して加算される（二重計算）問題が発生します。
 * レイがエリアライトに直接衝突した場合、二重計算を避けるためにゼロを返します。
 * @code
 * if (new_ray_hit.mesh_idx >= 0 && new_ray_hit.primitive_idx >= 0)
 * {
 *     return computeShading(ray, new_ray_hit, in_Object, in_AreaLights); // シェーディングの計算
 * }
 * @endcode
 *
 * #### 5. 結果を返却
 * レイが交差しない場合はゼロを返します。
 * @code
 * return Eigen::Vector3d::Zero();
 * @endcode
 *
 * @see computeShading
 * @see rayTracing
 * @see randomMT
 */
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

/**
 * @brief 鏡面反射（反射光）の寄与を計算する関数
 * @details 入射点での鏡面反射光の寄与を計算します。
 * 鏡面反射では、反射ベクトルを計算し、その方向に向かうレイを追跡することで寄与を得ます。
 *
 * @param[in] in_x 交差点の位置（3D座標）
 * @param[in] in_n 交差点での法線ベクトル
 * @param[in] in_w_eye 視線方向ベクトル（カメラから交差点への方向）
 * @param[in] in_ray_hit 交差情報（交差距離、メッシュ、プリミティブインデックスなど）
 * @param[in] in_Object シーン内のオブジェクト（メッシュ、マテリアル情報を含む）
 * @param[in] in_Material 交差点のマテリアル情報
 * @param[in] in_AreaLights シーン内のエリアライトリスト
 * @param[in] depth 現在のレイの追跡深度
 *
 * @return Eigen::Vector3d 鏡面反射光の寄与（RGB値）
 *
 * ### 処理の流れ
 *
 * #### 1. 反射ベクトルの計算
 * 入射方向と法線ベクトルから反射ベクトルを計算します。
 * @code
 * const double e_dot_n = in_w_eye.dot(in_n); // 入射方向と法線ベクトルの内積
 * Eigen::Vector3d w_L = 2.0 * in_n * e_dot_n - in_w_eye; // 反射ベクトル
 * w_L.normalize(); // ベクトルを正規化
 * @endcode
 *
 * 数式で表すと次のようになります：
 * \f[
 * \mathbf{w}_L = 2 (\mathbf{n} \cdot \mathbf{w}_\text{eye}) \mathbf{n} - \mathbf{w}_\text{eye}
 * \f]
 * - \f${n}\f$: 法線ベクトル
 * - \f${w}_\text{eye}\f$: 入射方向（視線方向）
 * - \f${w}_L\f$: 反射ベクトル
 *
 * #### 2. 新しいレイの生成と追跡
 * 計算した反射ベクトルを用いて新しいレイを生成し、そのレイを追跡します。
 * @code
 * Ray ray;
 * ray.o = in_x;               // レイの始点
 * ray.d = w_L;                // レイの方向
 * ray.depth = depth + 1;      // 深度をインクリメント
 * ray.prev_mesh_idx = in_ray_hit.mesh_idx; // 現在の交差情報を保持
 * ray.prev_primitive_idx = in_ray_hit.primitive_idx;
 *
 * RayHit new_ray_hit;
 * rayTracing(in_Object, in_AreaLights, ray, new_ray_hit); // 新しいレイの追跡
 * @endcode
 *
 * #### 3. 新しい交差点での光の寄与を計算
 * 新しいレイがオブジェクトに衝突した場合、その交差点のシェーディングを計算します。
 * @code
 * if (new_ray_hit.primitive_idx >= 0)
 * {
 *     return computeShading(ray, new_ray_hit, in_Object, in_AreaLights);
 * }
 * @endcode
 * - `primitive_idx >= 0` は、レイがシーン内のオブジェクトに衝突したことを示します。
 * - 交差点の光の寄与を `computeShading` で計算します。
 *
 * #### 4. 結果を返却
 * レイがどのオブジェクトにも衝突しなかった場合、ゼロを返します。
 * @code
 * return Eigen::Vector3d::Zero();
 * @endcode
 *
 * @see computeShading
 * @see rayTracing
 */

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

/**
 * @brief 屈折光の寄与を計算する関数
 * @details 入射点での屈折光の寄与を計算します。
 * 屈折光は、スネルの法則に基づき、異なる屈折率を持つ媒質間での光の屈折方向を計算します。
 * また、全反射が発生する場合には反射光の寄与を計算します。
 *
 * @param[in] in_x 交差点の位置（3D座標）
 * @param[in] in_n 交差点での法線ベクトル
 * @param[in] in_w_eye 視線方向ベクトル（カメラから交差点への方向）
 * @param[in] in_ray_hit 交差情報（交差距離、メッシュ、プリミティブインデックスなど）
 * @param[in] in_Object シーン内のオブジェクト（メッシュ、マテリアル情報を含む）
 * @param[in] in_Material 交差点のマテリアル情報
 * @param[in] in_AreaLights シーン内のエリアライトリスト
 * @param[in] depth 現在のレイの追跡深度
 *
 * @return Eigen::Vector3d 屈折光の寄与（RGB値）
 *
 * ### 処理の流れ
 *
 * #### 1. 入射方向と法線の内積を計算
 * 入射方向ベクトルと法線ベクトルの内積を計算します。
 * @code
 * const double e_dot_n = in_w_eye.dot(in_n);
 * @endcode
 * - **`e_dot_n`**:
 *   - 入射方向の法線方向へのスカラー成分。
 *
 * #### 2. 屈折率を設定
 * 表面の内側から外側へ出る場合と、外側から内側へ入る場合で屈折率を切り替えます。
 * @code
 * const double eta = in_ray_hit.isFront ? in_Material.eta : 1.0 / in_Material.eta;
 * @endcode
 * - **`eta`**:
 *   - 屈折率の比を表します。
 *   - 内側から外側の場合：`1.0 / eta`。
 *   - 外側から内側の場合：`eta`。
 *
 * #### 3. 判定：全反射が発生するか
 * スネルの法則に基づいて、全反射が発生するかどうかを判定します。
 * @code
 * const double inside_sqrt = 1.0 - eta*eta * (1.0 - e_dot_n * e_dot_n);
 * if (inside_sqrt < 0.0)
 * {
 *     return computeReflection(in_x, in_n, in_w_eye, in_ray_hit, in_Object, in_Material, in_AreaLights, depth);
 * }
 * @endcode
 * - **`inside_sqrt`**:
 *   - スネルの法則に基づく計算結果。
 *   - \( \text{inside\_sqrt} < 0 \) の場合、全反射が発生。
 * - **全反射の場合**:
 *   - 屈折ベクトルを計算せず、反射ベクトルを計算してその寄与を返します。
 *
 * #### 4. 屈折ベクトルの計算
 * 全反射が発生しない場合、スネルの法則を使って屈折ベクトルを計算します。
 * @code
 * Eigen::Vector3d w_t = -in_n * sqrt(inside_sqrt) - eta * (in_w_eye - e_dot_n * in_n);
 * w_t.normalize();
 * @endcode
 * - 数式：
 *   \f[
 *   \mathbf{w}_t = -\sqrt{1 - \eta^2 (1 - (\mathbf{w}_\text{eye} \cdot \mathbf{n})^2)} \mathbf{n} - \eta (\mathbf{w}_\text{eye} - (\mathbf{w}_\text{eye} \cdot \mathbf{n}) \mathbf{n})
 *   \f]
 * - **第一項**:
 *   - 法線方向の成分（曲がり具合を決定）。
 * - **第二項**:
 *   - 入射方向の成分を屈折率で調整。
 *
 * #### 5. 新しいレイを生成し追跡
 * 計算した屈折ベクトルを使って新しいレイを生成し、交差点を追跡します。
 * @code
 * Ray ray;
 * ray.o = in_x;
 * ray.d = w_t;
 * ray.depth = depth + 1;
 * ray.prev_mesh_idx = in_ray_hit.mesh_idx;
 * ray.prev_primitive_idx = in_ray_hit.primitive_idx;
 *
 * RayHit new_ray_hit;
 * rayTracing(in_Object, in_AreaLights, ray, new_ray_hit);
 * @endcode
 *
 * #### 6. 新しい交差点での寄与を計算
 * 新しいレイがオブジェクトに衝突した場合、その交差点での寄与を再帰的に計算します。
 * @code
 * if (new_ray_hit.primitive_idx >= 0)
 * {
 *     return computeShading(ray, new_ray_hit, in_Object, in_AreaLights);
 * }
 * @endcode
 *
 * #### 7. 結果を返却
 * レイがどのオブジェクトにも衝突しなかった場合、ゼロを返します。
 * @code
 * return Eigen::Vector3d::Zero();
 * @endcode
 *
 * @see computeReflection
 * @see computeShading
 * @see rayTracing
 */
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
