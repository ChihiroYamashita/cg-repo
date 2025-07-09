#ifndef RAYHIT_H
#define RAYHIT_H
#include <Eigen/Dense>
/**
 * @brief レイと三角形の交差点情報を保持する構造体
 * @details この構造体には、レイが交差した際の交差点の情報を保持します。交差距離、交差位置、法線、交差したオブジェクトの識別情報などを含みます。
 *
 * 復習 @ref barycentric_coordinates "バリュートリック座標" を参照してください。
 * - **t**: 交差点までの距離。交差しない場合は非常に大きな値（例: 無限大）。
 * - **alpha**: 交差点におけるバリュートリック座標の第一成分（α）。交差点が三角形のどの位置にあるかを示すために使用されます。
 * 通常、三角形の3つの頂点（v1, v2, v3）に対して、交差点 `x` は `x = alpha * v1 + beta * v2 + gamma * v3` と表現され、
 * `alpha + beta + gamma = 1` の関係があります。
 * - **beta**: バリュートリック座標 (β)。交差点におけるバリュートリック座標の第二成分（β）。`alpha` と同様に、三角形内の位置を特定するために使用されます。
 *
 ** 復習 @ref about_index "インデックス" を参照してください。
 * - **mesh_idx**: 交差したメッシュのインデックス。
 * - `< -1`: レイがどのオブジェクトとも交差していないことを示します。
 * - `-1`: レイがエリアライトと交差したことを示します。
 * - `>= 0`: レイがシーン内の特定のオブジェクトメッシュと交差したことを示し、そのメッシュの配列インデックスが格納されます。
 *
 * * 復習 @ref about_primitive "プリミティブ" を参照してください。<br>
 * - **primitive_idx**: 交差したプリミティブ（三角形またはライト）のインデックス。
* 交差したプリミティブ（個々の三角形またはエリアライト）のインデックス。
 * - `< 0`: レイがどのプリミティブとも交差していないことを示します。
 * - `mesh_idx = -1` の場合：交差したエリアライトの配列インデックスが格納されます。
 * - `mesh_idx >= 0` の場合：交差したメッシュ内の三角形の配列インデックスが格納されます。
 *
 * - **isFront**: レイが三角形の表面（正面）に当たったかどうか。
 *   - `true`: 表面に交差。
 *   - `false`: 裏面に交差。
 */
struct RayHit
{
    // ★★★ コンストラクタを追加 ★★★
    RayHit() :
        t(std::numeric_limits<double>::max()), // tを最大値で初期化
        alpha(0.0),
        beta(0.0),
        mesh_idx(-1),      // 「ヒットなし」を示す-1で初期化
        primitive_idx(-1), // 「ヒットなし」を示す-1で初期化
        isFront(true)
    {}

    double t;
    double alpha;
    double beta;
    int mesh_idx; // < -1: no intersection, -1: area light, >= 0: object_mesh
    int primitive_idx; // < 0: no intersection
    bool isFront;
};
#endif // RAYHIT_H
