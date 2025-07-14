//
//  TriMesh.h
//  gl3d_obj_mesh
//
//  Created by Yonghao Yue on 2019/11/06.
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

#ifndef TriMesh_h
#define TriMesh_h

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#include <unistd.h>
#else
#include <GL/freeglut.h>
#endif

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#include <Eigen/Core>
#include <vector>
/**
 * @brief オブジェクトの材質を表す構造体
 * @details この構造体は、3Dオブジェクトの材質（マテリアル）情報を格納します。
 * 材質はオブジェクトの表面の光反射特性や屈折特性、テクスチャ情報などを定義します。
 *
 * 

 *
 *
 *
 * 
 * ### メンバー変数:
 * - **texture**:
 *   - オブジェクト表面に適用されるテクスチャ（画像）データ。
 *   - `GLuint` 型でテクスチャIDを管理。
 *   - OpenGLでロードされたテクスチャを識別するために使用されます。
 *
 * - **kd** (拡散反射係数):
 *   - 材質の拡散反射成分を表すベクトル。
 *   - 例: マットな材質では光が表面全体に均等に散乱する。
 *   - 3次元ベクトル (R, G, B) で色を定義。
 *
 * - **ks** (鏡面反射係数):
 *   - 材質の鏡面反射成分を表すベクトル。
 *   - 光源からの光が特定の方向に強く反射する性質を制御。
 *   - 3次元ベクトル (R, G, B) で色を定義。
 *
 * - **kt** (透過率):
 *   - 材質の透過特性を表すベクトル。
 *   - 透明なオブジェクト（例: ガラスや水）の光透過性を示す。
 *   - 3次元ベクトル (R, G, B) で色を定義。
 *
 * - **eta** (屈折率):
 *   - 材質の屈折率を表すスカラ値。
 *   - 光が異なる媒質に入る際の屈折の度合いを示す。
 *   - 例: 空気の屈折率は約1.0、ガラスは約1.5。
 *
 * ### 主な用途:
 * この構造体は、光の反射や屈折をシミュレーションするためにレンダリング処理で使用されます。
 * シェーダーや光線追跡アルゴリズム（レイトレーシング）において、各ピクセルの色を計算する際の基礎情報を提供します。
 *
 * ### 使用例:
 * 材質を設定してオブジェクトに適用:
 * ```cpp
 * Material material;
 * material.texture = loadTexture("example.png");
 * material.kd = Eigen::Vector3d(0.8, 0.8, 0.8); // 灰色
 * material.ks = Eigen::Vector3d(1.0, 1.0, 1.0); // 鏡面反射（白）
 * material.kt = Eigen::Vector3d(0.0, 0.0, 0.0); // 透過なし
 * material.eta = 1.5; // ガラスの屈折率
 * ```
 */

struct Material
{
  GLuint texture;
  Eigen::Vector3d kd;
  Eigen::Vector3d ks;
  Eigen::Vector3d kt;
  double eta;
  // ▼▼▼ 以下を追加 ▼▼▼
  bool isToon;      // トゥーンシェーディングが有効か    // リットカラー (Lit Color)
  Eigen::Vector3d C_lit;
  Eigen::Vector3d C_shadow; // シャドウカラー (Shadow Color)
  double tau;       // 明暗を分ける閾値 (τ)



  //▲▲▲▲▲▲▲▲
  Material()
      : kd(0.0,0.0,0.0),ks(0.0,0.0,0.0),kt(0.0,0.0,0.0),eta(1.0),texture(0),isToon(false),
      C_lit(1.0,1.0,1.0),C_shadow(0.2,0.2,0.2),tau(0.5)
  {}



};

/**
 * @brief 三角形メッシュを表す構造体
 * @see RayHit
 *
 * @details 3Dオブジェクトを構成する基本的な要素として、三角形メッシュの情報を格納します。
 * 各メンバー変数の詳細を以下に示します：
 *

 * ### メンバー変数
 * - **vertices**:
 *   - メッシュの頂点座標リスト。
 *   - 各頂点は3次元ベクトルで表されます。
 * - **vertex_normals**:
 *   - 頂点の法線ベクトルリスト。
 *   - シェーディング計算に使用されます。
 * - **tex_coords**:
 *   - テクスチャ座標リスト。
 *   - 各頂点に対応するテクスチャマッピングのUV座標。
 * - **triangles**:
 *   - 三角形を構成する**頂点インデックス**のリスト。
 *   - 各三角形は3つの頂点インデックスで定義されます。
 * - **vertex_colors**:
 *   - 頂点カラーリスト（オプション）。
 *   - 頂点ごとのカラー情報。
 * - **material**:
 *   - 材質情報を格納する構造体。
 *   - 拡散反射係数 (`kd`)、鏡面反射係数 (`ks`)、屈折係数 (`kt`)、屈折率 (`eta`)、およびテクスチャ。
 *
 * ### 補足
 * - `std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> vertices`
 *   の記述の詳細については、@ref additional_inf_1 "レイと三角形の交差計算の詳細" を参照してください。
 * - `頂点インデックス`
 *   の概念の詳細については、@ref additional_inf_2 "レイと三角形の交差計算の詳細" を参照してください。
 * 
 */

struct TriMesh
{
  std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> vertices;
  std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> vertex_normals;
  std::vector<Eigen::Vector2d, Eigen::aligned_allocator<Eigen::Vector2d>> tex_coords;
  std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i>> triangles;
  
  std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> vertex_colors;
  
  Material material;
};

/**
 * @brief シーン内のオブジェクトを表す構造体
 * @details この構造体は、複数の三角形メッシュ (`TriMesh`) を持つ3Dオブジェクトを表します。
 * シーン内の物体（例: 箱、球、人型モデルなど）を1つのまとまりとして扱うために使用されます。
 *
 *  *  * @startuml
 * class Object {
 *   - List<TriMesh> meshes
 * }
 *
 * class TriMesh {
 *   - List<Vector3d> vertices
 *   - List<Vector3d> vertex_normals
 *   - List<Vector2d> tex_coords
 *   - List<int> triangles
 *   - Material material
 * }
 *
 * class Material {
 *   - GLuint texture
 *   - Vector3d kd
 *   - Vector3d ks
 *   - Vector3d kt
 *   - double eta
 * }
 *
 * Object --> "1..* すべて参照" TriMesh
 * TriMesh --> "1　マテリアル参照" Material
 * @enduml
 * 
 * ### メンバー変数:
 * - **meshes**:
 *   - このオブジェクトを構成する三角形メッシュ (`TriMesh`) のリスト。
 *   - 各メッシュ (`TriMesh`) は以下の情報を持ちます：
 *     - 頂点座標 (`vertices`)
 *       - メッシュを構成する3D空間内の座標リスト。
 *     - 頂点法線 (`vertex_normals`)
 *       - 各頂点の表面の方向を表すベクトル。
 *     - テクスチャ座標 (`tex_coords`)
 *       - テクスチャマッピングのためのUV座標。
 *     - 三角形インデックス (`triangles`)
 *       - メッシュを構成する三角形の頂点インデックス。
 *     - 頂点カラー (`vertex_colors`)
 *       - 頂点ごとの色情報（オプション）。
 *     - 材質情報 (`material`)
 *       - 光の反射特性やテクスチャ情報を格納する構造体。
 *
 * ### 主な関連関数:
 * - **`loadObj`**:
 *   - 外部の3Dモデルファイル（例: `.obj` ファイル）を読み込み、`Object` を初期化します。
 * - **`resizeObj`**:
 *   - オブジェクトの全体サイズを指定した範囲にスケーリングします。
 * - **`extendMesh`**:
 *   - 別のメッシュを現在のメッシュに統合し、ジオメトリを拡張します。
 *
 * ### 使用例:
 * 1. `loadObj` を用いて `.obj` ファイルからオブジェクトをロード:
 *    ```cpp
 *    Object obj;
 *    loadObj("example.obj", obj);
 *    ```
 * 2. `resizeObj` でスケールを調整:
 *    ```cpp
 *    Eigen::Vector3d min_bounds(-1.0, -1.0, -1.0);
 *    Eigen::Vector3d max_bounds(1.0, 1.0, 1.0);
 *    resizeObj(obj, min_bounds, max_bounds);
 *    ```
 */
struct Object
{
  std::vector<TriMesh> meshes;
};

void resetMesh( TriMesh& io_Mesh );
void printMesh( const TriMesh& in_Mesh );
void extendMesh( TriMesh& io_BaseMesh, const TriMesh& in_SecondMesh );
GLuint prepareTextureFromJpegFile( const char* in_FileName );

void resizeObj( Object& io_object, const Eigen::Vector3d& in_min, const Eigen::Vector3d& in_max );

bool loadObj( const std::string& in_filename, Object& out_object );

void applyMaterialColorToVertices(Object& io_Object);

#endif /* TriMesh_h */
