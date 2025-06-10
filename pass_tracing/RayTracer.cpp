#include "RayTracer.h"
#include "ray.h"
const double __FAR__ = 1.0e33;

//rayTracing()（レイとシーン全体の交差）
//rayTriangleIntersect()（レイと三角形の交差）
//rayAreaLightIntersect()（レイとエリアライトの交差）
//レイと何かの交差を調べる」という意味で機能的に近いものを統合してまとめた

void rayTracing( const Object& in_Object, const std::vector<AreaLight>& in_AreaLights, const Ray& in_Ray, RayHit& io_Hit )
{
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
    }

    io_Hit.t = t_min;
    io_Hit.alpha = alpha_I;
    io_Hit.beta = beta_I;
    io_Hit.mesh_idx = mesh_idx;
    io_Hit.primitive_idx = primitive_idx;
    io_Hit.isFront = isFront;
}

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

void rayAreaLightIntersect( const std::vector<AreaLight>& in_AreaLights, const int in_Light_idx, const Ray& in_Ray, RayHit& out_Result )
{


    // 初期化: tを非常に大きな値に設定し、無効な交差とする
    out_Result.t = __FAR__;

    // 1. エリアライトの中心座標とエッジベクトルを取得
    const Eigen::Vector3d pos = in_AreaLights[in_Light_idx].pos;       // ライトの中心座標
    const Eigen::Vector3d arm_u = in_AreaLights[in_Light_idx].arm_u;   // ライトの横方向のエッジベクトル
    const Eigen::Vector3d arm_v = in_AreaLights[in_Light_idx].arm_v;   // ライトの縦方向のエッジベクトル

    // 2. ライトの法線ベクトルを計算（エッジベクトルの外積）
    Eigen::Vector3d light_normal = arm_u.cross(arm_v);
    light_normal.normalize(); // 法線を正規化して単位ベクトルにする

    bool isFront = true;

    // 3. レイとライトの法線ベクトルの内積を計算し、レイの進行方向を確認
    const double denominator = light_normal.dot(in_Ray.d);
    if (denominator >= 0.0)
        isFront = false; // レイがライトの裏面に当たっている場合、isFrontをfalseに設定

    const double t = light_normal.dot( pos - in_Ray.o ) / denominator;

    if( t <= 0.0 )
        return;

    // 5. 交差点の位置 x を計算
    const Eigen::Vector3d x = in_Ray.o + t * in_Ray.d; // 交差点 = レイの始点 + t * レイの方向ベクトル

    // 6. 交差点のUV座標を計算（エリアライトの局所座標系に変換）
    const double u = (x - pos).dot(arm_u) / arm_u.squaredNorm(); // 横方向の座標
    const double v = (x - pos).dot(arm_v) / arm_v.squaredNorm(); // 縦方向の座標


    if( u < -1.0 || 1.0 < u || v < -1.0 || 1.0 < v ) return;

    out_Result.t = t;
    out_Result.alpha = u;
    out_Result.beta = v;
    out_Result.isFront = isFront;
}
