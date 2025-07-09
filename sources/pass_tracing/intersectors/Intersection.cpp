#include "Intersection.h"
#include <QDebug>
#include <math.h>
const double __FAR__ = 1.0e33;
/**
* @brief レイとシーン内のオブジェクトやライトとの最短交差点を計算する関数
* @details 与えられたレイ (in_Ray) がシーン内のオブジェクト（in_Object）およびエリアライト（in_AreaLights）と交差する場合、最も近い交差点を見つけ、その情報を出力パラメータ (io_Hit) に格納する処理を行います。
*
*
* @see RayHit temp_hit.t: レイの始点から交差点までの距離、temp_hit.alpha, temp_hit.beta:交差点のUV座標、 temp_hit.isFront: レイが三角形の正面に当たったか（true）または背面に当たったか
* @see rayTriangleIntersect
* @see rayAreaLightIntersect
* @param[in] in_Object シーン内のオブジェクト（複数のメッシュを含む）
* @param[in] in_AreaLights シーン内のエリアライト
* @param[in] in_Ray レイ（始点と方向を持つ）
* @param[out] io_Hit 交差点の情報（交差距離、三角形インデックス、交差位置、法線などを含む）
*
* @startuml
* main -> rayTracing : レイと交差計算開始
* rayTracing -> rayTriangleIntersect : メッシュの三角形と交差計算
* rayTriangleIntersect -> rayTracing : 交差情報を取得（temp_hitに保存）
* rayTracing -> rayAreaLightIntersect : エリアライトとの交差計算
* rayAreaLightIntersect -> rayTracing : 交差情報を取得（temp_hitに保存）
* rayTracing -> rayTracing : 最短交差点情報を更新 \n交差距離t_min、インデックスなど
* rayTracing -> io_Hit : 最短交差点情報を保存
* @enduml
*
*  @image html rayTracing1.png "処理フローの概要図"
* @image html rayTracing2.png "処理フローの概要図-続き"
*
*オブジェクトは、通常、多数の三角形で構成される複雑な形状であり、それぞれの三角形について交差計算を行う必要があるためループが必要です。
*エリアライトは、単一の幾何学的形状として扱われ、すべてのライトと交差計算を行うだけで十分であり、三角形ごとのループは不要です。


*
* 最短交差点の情報を`io_Hit`に保存し、交差がない場合は初期値のままになります。
*/
void rayTracing( const Object& in_Object, const std::vector<AreaLight>& in_AreaLights, const Ray& in_Ray, RayHit& io_Hit )
{
    // ★★★ 修正：io_Hitを最初にリセットする ★★★
    // これで、この関数内でヒットが見つからなければ、
    // io_Hitは確実に「ヒットなし」の状態になる。
    io_Hit = RayHit();

    //初期化
    double t_min = __FAR__;
    double alpha_I = 0.0, beta_I = 0.0;
    int mesh_idx = -99; int primitive_idx = -1;
    bool isFront = true;

    //1.メッシュとの交差計算
    //オブジェクト内の各メッシュと、そのメッシュ内の各三角形に対して交差計算を行う
    for( int m=0; m<in_Object.meshes.size(); m++ )//オブジェクトの数だけループ
    {
        for( int k=0; k<in_Object.meshes[m].triangles.size(); k++ )//オブジェクト内の三角形の数だけループ
        {
            if( m == in_Ray.prev_mesh_idx && k == in_Ray.prev_primitive_idx ) continue;

            RayHit temp_hit;//一つの三角形との交差結果を一時的に保持するための変数
            //rayTriangleIntersect を使用して、三角形との交差計算を実行
            //レイ（in_Ray）と指定された三角形（in_Object.meshes[m].triangles[k]）との交差を計算
            //temp_hitに結果を格納
            rayTriangleIntersect( in_Object.meshes[m], k, in_Ray, temp_hit );

            //現在の交差点が最短かどうかを確認
            //t_minは非常に大きな値で初期化してある
            //temp_hit.t(レイの始点から交差点までの距離)がt_minより小さければ以下を更新
            //temp_hit.t(レイの始点から交差点までfの距離)がt_minより小さければ以下を更新
            if( temp_hit.t < t_min )
            {
                t_min = temp_hit.t;
                alpha_I = temp_hit.alpha;
                beta_I = temp_hit.beta;
                mesh_idx = m;
                primitive_idx = k;
                isFront = temp_hit.isFront;
            }
        }
    }

    /*デバッグ用一旦削除
    //２.エリアライトとの交差計算
    for( int l=0; l<in_AreaLights.size(); l++ )
    {
        if( -1 == in_Ray.prev_mesh_idx && l == in_Ray.prev_primitive_idx ) continue;

        RayHit temp_hit;
        rayAreaLightIntersect( in_AreaLights, l, in_Ray, temp_hit );
        if( temp_hit.t < t_min )
        {
            t_min = temp_hit.t;
            alpha_I = temp_hit.alpha;
            beta_I = temp_hit.beta;
            mesh_idx = -1;
            primitive_idx = l;
            isFront = temp_hit.isFront;
        }
    }*/

    io_Hit.t = t_min;
    io_Hit.alpha = alpha_I;
    io_Hit.beta = beta_I;
    io_Hit.mesh_idx = mesh_idx;
    io_Hit.primitive_idx = primitive_idx;
    io_Hit.isFront = isFront;
}

/**
 * @brief レイが指定された三角形と交差するかどうかを判定する。交差している場合、その交差点の情報（交差距離、位置、法線など）を出力変数 out_Result に格納する。
 * @details 指定された三角形（`in_Triangle_idx`）とレイ（`in_Ray`）の交差を計算し、交差点情報を`out_Result`に保存します。交差しない場合、`out_Result.t`は初期値のままになります。
 *
 * @param[in] in_Mesh メッシュデータ（三角形の頂点とインデックスを含む）
 * @param[in] in_Triangle_idx 交差計算を行う三角形のインデックス
 * @param[in] in_Ray レイの情報（始点`o`と方向`d`）
 * @param[out] out_Result 交差点の情報（交差距離`t`、UV座標`alpha`と`beta`、表面交差フラグ`isFront`）
 *
 * @see RayHit
 *


 * @image html rayTriangleIntersect.png "処理フローの概要図"
 *
 * @details **処理の概要**:
 * - メッシュから三角形の頂点（`v1`, `v2`, `v3`）を取得。
 * - 三角形の法線を計算し、レイが表面に当たるか（`isFront`）を判定。
 * - レイと三角形の平面との交差点を計算（`t`）。
 * - 交差点が三角形内にある場合、バリュートリック座標（`alpha`, `beta`）を用いて交差位置を確認。
 * - 結果を`out_Result`に保存。
 */
void rayTriangleIntersect( const TriMesh& in_Mesh, const int in_Triangle_idx, const Ray& in_Ray, RayHit& out_Result )
{

    //交差がない場合の初期値として距離を非常に大きな値（無限大に近い値）を設定
    out_Result.t = __FAR__;

    //メッシュ内の三角形のインデックス（in_Triangle_idx）を基に、三角形の頂点座標 v1, v2, v3 を取得
    const Eigen::Vector3d v1 = in_Mesh.vertices[in_Mesh.triangles[in_Triangle_idx](0)];
    const Eigen::Vector3d v2 = in_Mesh.vertices[in_Mesh.triangles[in_Triangle_idx](1)];
    const Eigen::Vector3d v3 = in_Mesh.vertices[in_Mesh.triangles[in_Triangle_idx](2)];

    //外積から頂点の法線ベクトルを生成
    Eigen::Vector3d triangle_normal = ( v1 - v3 ).cross( v2 - v3 );
    //単位ベクトル
    triangle_normal.normalize();

    bool isFront = true;
    //
    //レイの方向ベクトル（in_Ray.d）と三角形の法線ベクトルの内積を計算。
    const double denominator = triangle_normal.dot( in_Ray.d );

    //正の値: レイが三角形の裏面からあたってる。
    if( denominator >= 0.0 )
        isFront = false;

    const double t = triangle_normal.dot( v3 - in_Ray.o ) / denominator;
    //正の値: レイが三角形の表面からあたってる。
    if( t <= 0.0 )
        return;

    //交差点の位置を計算
    const Eigen::Vector3d x = in_Ray.o + t * in_Ray.d;


    /*--------------------バリュートリック座標（UV座標）を計算---------------------*/
    Eigen::Matrix<double, 3, 2> A;//計算用行列を作成

    A.col(0) = v1 - v3;  // 辺1（v1 -> v3）
    A.col(1) = v2 - v3;  // 辺2（v2 -> v3）


    Eigen::Matrix2d ATA = A.transpose() * A;
    const Eigen::Vector2d b = A.transpose() * (x - v3);

    const Eigen::Vector2d alpha_beta = ATA.inverse() * b;


    /*-------------------------------------------------------------------*/


    if( alpha_beta.x() < 0.0 || 1.0 < alpha_beta.x() || alpha_beta.y() < 0.0 || 1.0 < alpha_beta.y() || 1.0 - alpha_beta.x() - alpha_beta.y() < 0.0 || 1.0 < 1.0 - alpha_beta.x() - alpha_beta.y() ) return;

    out_Result.t = t;
    out_Result.alpha = alpha_beta.x();
    out_Result.beta = alpha_beta.y();
    out_Result.isFront = isFront;
}
/**
 * @brief レイと交差した点における補間された法線ベクトルを計算する関数
 * @details この関数は、指定された交差情報（RayHit）に基づいて、交差した三角形の各頂点の法線をバリュートリック座標で補間し、
 * 法線ベクトルを得ます。補間後、単位ベクトルに正規化し、裏面からの交差の場合は反転して返します。

 * 復習 @ref barycentric_coordinates "バリュートリック座標" を参照してください。
 *
 * @param[in] in_Object レイが交差したオブジェクト情報。メッシュと頂点法線情報を含む。
 * @param[in] in_Hit 交差点の情報（交差位置のバリュートリック座標 alpha, beta、三角形インデックス、交差面の向きなど）
 *
 * @return Eigen::Vector3d 補間された交差点での法線ベクトル（単位ベクトル）
 *
 * ### 処理の流れ
 *
 * #### 1. 対象三角形の頂点インデックスを取得
 * @code
 * const int v1_idx = in_Object.meshes ;
 * const int v2_idx = in_Object.meshes ;
 * const int v3_idx = in_Object.meshes ;
 * @endcode
 *
 * #### 2. 各頂点の法線ベクトルを取得
 * @code
 * const Eigen::Vector3d n1 = in_Object.meshes[in_Hit.mesh_idx].vertex_normals[v1_idx];
 * const Eigen::Vector3d n2 = in_Object.meshes[in_Hit.mesh_idx].vertex_normals[v2_idx];
 * const Eigen::Vector3d n3 = in_Object.meshes[in_Hit.mesh_idx].vertex_normals[v3_idx];
 * @endcode
 *
 * #### 3. バリュートリック補間を用いて交差点での法線を計算
 * \f[
 * n = \alpha n_1 + \beta n_2 + \gamma n_3 \quad (\gamma = 1 - \alpha - \beta)
 * \f]
 * @code
 * const double gamma = 1.0 - in_Hit.alpha - in_Hit.beta;
 * Eigen::Vector3d n = in_Hit.alpha * n1 + in_Hit.beta * n2 + gamma * n3;
 * n.normalize();
 * @endcode
 *
 * #### 4. 裏面からの交差であれば法線を反転
 * @code
 * if (!in_Hit.isFront) n = -n;
 * @endcode
 *
 * #### 5. 結果を返却
 * @code
 * return n;
 * @endcode
 *
 * @see RayHit
 * @see rayTracing
 * @see computeShading
 */
Eigen::Vector3d computeRayHitNormal( const Object& in_Object, const RayHit& in_Hit )
{
    const int v1_idx = in_Object.meshes[in_Hit.mesh_idx].triangles[in_Hit.primitive_idx](0);
    const int v2_idx = in_Object.meshes[in_Hit.mesh_idx].triangles[in_Hit.primitive_idx](1);
    const int v3_idx = in_Object.meshes[in_Hit.mesh_idx].triangles[in_Hit.primitive_idx](2);

    const Eigen::Vector3d n1 = in_Object.meshes[in_Hit.mesh_idx].vertex_normals[v1_idx];
    const Eigen::Vector3d n2 = in_Object.meshes[in_Hit.mesh_idx].vertex_normals[v2_idx];
    const Eigen::Vector3d n3 = in_Object.meshes[in_Hit.mesh_idx].vertex_normals[v3_idx];

    const double gamma = 1.0 - in_Hit.alpha - in_Hit.beta;
    Eigen::Vector3d n = in_Hit.alpha * n1 + in_Hit.beta * n2 + gamma * n3;
    n.normalize();

    if( !in_Hit.isFront ) n = -n;

    return n;
}
