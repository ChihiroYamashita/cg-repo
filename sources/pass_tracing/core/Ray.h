#ifndef RAY_H
#define RAY_H
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#include <Eigen/Dense>

/**
 * @brief レイ（Ray）を表す構造体
 * @details レイは、光線追跡や可視化においてシーン内を進む光の経路を表します。この構造体は、レイの始点（origin）、方向（direction）、追跡の深さ、直前に交差したオブジェクトの情報を格納します。
 *
 * - **o**: レイの始点。通常はカメラの視点や反射点の位置。
 * - **d**: レイの方向。単位ベクトルとして正規化されていることが望ましい。
 * - **depth**: このレイが何回目の反射・屈折によるものかを示す深さカウント（再帰回数）。
 * - **prev_mesh_idx**: 直前に交差したメッシュのインデックス。交差の再利用を避けるために用いる。
 * - **prev_primitive_idx**: 直前に交差したプリミティブ（三角形）のインデックス。これも再交差を防ぐ目的で使用。
 *
 * @note `depth`、`prev_mesh_idx`、`prev_primitive_idx` は再帰的な光線追跡において重要で、自己交差を防止するために利用されます。
 *
 * @see Camera::screenView
 * @see rayTracing
 */
struct Ray
{
    Eigen::Vector3d o;
    Eigen::Vector3d d;
    int depth;
    int prev_mesh_idx;
    int prev_primitive_idx;
};
#endif // RAY_H
