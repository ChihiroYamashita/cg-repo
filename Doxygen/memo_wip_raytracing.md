# レイトレーシング移植仕様メモ

このドキュメントは、MyOpenGLWidget_cameraベースで段階的にレイトレーシング機能を移植していく際の設計方針と、クラス・関数・ファイル構成を整理したものです。

---


---

## ✅ 現在の移植状況（関数・構造体）

### 🎨 表示・描画関連

* ✅ `drawFilm()` によるフィルム表示機能 → 完了
* ✅ `paintGL()` → Qt側の描画呼び出しを確認・対応済み
* ✅ `updateRayTracing()` → 全ピクセル一括更新による可視化テスト完了

### 📡 レイ生成・交差関連

* ✅ `screenView()`（Cameraクラス）→ ピクセル座標からレイ生成済み
* ✅ `rayTracing()` → 最小限版を移植済み（光源処理は除外）
* ✅ `meshTriangleIntersect()` → 移植済み
* ✅ `intersectTriangle()` → 移植済み

### 🧭 法線・色処理

* ✅ `debug_computeNormalColor()`（法線を色で可視化） → 実装済み
* ✅ `computeRayHitNormal()`（交差点での法線補間） → 実装済み

---

### 📦 使用構造体・クラス

* ✅ `Ray` → レイの定義構造体（原点、方向、交差履歴含む）
* ✅ `RayHit` → 交差情報（メッシュ番号・三角形番号・距離など）
* ✅ `Camera` → レイ生成処理用クラス（視点・画角など保持）
* ✅ `TriMesh` → メッシュ構造体（頂点・法線・三角形など）
* ✅ `MyOpenGLWidget_camera` → レイ生成・描画処理を統括するクラス

---




---
## 全体フロー





@startuml
actor User

participant MyOpenGLWidget_camera
participant Camera
participant Ray
participant Intersection
participant TriMesh
participant Geometry as "computeRayHitNormal"
participant FilmBuffer

== ユーザが描画をトリガー ==
User -> MyOpenGLWidget_camera : paintGL()
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : updateRayTracing()

== レイの生成 ==
loop 各ピクセルごとに
    MyOpenGLWidget_camera -> Camera : screenView(i, j)
    Camera --> MyOpenGLWidget_camera : Ray

== レイとシーンの交差判定 ==
    MyOpenGLWidget_camera -> Intersection : rayTracing(g_Obj, {}, ray, ray_hit)

    loop 各メッシュ
        Intersection -> TriMesh : meshTriangleIntersect()
        loop 各三角形
            TriMesh -> Intersection : intersectTriangle()
            alt ヒットしたら
                Intersection --> TriMesh : RayHit (交差情報)
            end
        end
    end
    Intersection --> MyOpenGLWidget_camera : 最短距離の RayHit

== 法線計算と色変換 ==
    MyOpenGLWidget_camera -> Geometry : computeRayHitNormal(g_Obj, ray_hit)
    Geometry --> MyOpenGLWidget_camera : 法線ベクトル

    MyOpenGLWidget_camera -> FilmBuffer : 法線 → 色変換 → m_filmBuffer に格納
end

== OpenGLで描画 ==
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : update() → paintGL()
@enduml

---
## 🔧 関数仕様と目的

### `MyOpenGLWidget_camera::updateRayTracing()`

@brief 全ピクセルに対してレイを発射し、法線色をフィルムバッファに記録  
@see @ref debug_computeNormalColor

```cpp
Eigen::Vector3d color = debug_computeNormalColor(ray);
```

---

### `MyOpenGLWidget_camera::debug_computeNormalColor(const Ray& ray)`

@brief オブジェクトとの交差点の法線を色として返す

```cpp
return Eigen::Vector3d(normal.x() * 0.5 + 0.5, 
                       normal.y() * 0.5 + 0.5, 
                       normal.z() * 0.5 + 0.5);
```

---

## 🎯 UML: paintGLまわりの初期化と描画の流れ

@details  
MyOpenGLWidget_cameraでは、initializeGL(), paintGL(), resizeGL() のみがOpenGL描画に関与しています。

詳しくは [描画関数 paintGL() の詳細](@ref MyOpenGLWidget_camera::paintGL()) をご覧ください。

```text
          QOpenGLWidgetによる初期化 & 描画
┌─────────────────────────────────────────┐
│                                        │
│           1回だけ呼ばれる関数         │
│   ┌──────────────┐                       │
│   │ initializeGL()│← OpenGLの初期設定    │
│   └──────────────┘                       │
│                                        │
│         毎フレーム or 明示的再描画で呼ばれる関数 │
│   ┌──────────────┐                       │
│   │ resizeGL()    │← リサイズ時に呼ばれる │
│   └──────────────┘                       │
│   ┌──────────────┐                       │
│   │ paintGL()     │← 描画処理の本体        │
│   └──────────────┘                       │
└─────────────────────────────────────────┘
```

---

## 🌐 PlantUML: 初期化・描画シーケンス

@startuml
actor User

User -> MyOpenGLWidget_camera : 初期化 (initializeGL)
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : initializeFilmTexture() を呼び出し、filmバッファ作成 (※デバッグ用)

User -> MyOpenGLWidget_camera : ウィンドウリサイズ (resizeGL)
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : width, height 更新
MyOpenGLWidget_camera -> Camera : setAspectRatio()
MyOpenGLWidget_camera -> OpenGL : 古いテクスチャ削除
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : initializeFilmTexture() 再実行
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : updateProjectionMatrix()

User -> MyOpenGLWidget_camera : paintGL() を呼び出し
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : projection_and_modelview(g_Camera2)
MyOpenGLWidget_camera -> drawFilm : カメラ情報とfilmテクスチャを渡す
drawFilm -> Camera : getEyePoint(), getX/Y/ZVector(), getFocalLength() を呼び出し
drawFilm -> OpenGL : テクスチャをバインドし、矩形に貼り付けて描画
@enduml

---

## 📁 推奨ファイル構成

```
sources/
│
├── camera/
│   └── Camera.cpp / Camera.h
│
├── geometry/
│   ├── TriMesh.cpp / TriMesh.h
│   └── Object.cpp (必要に応じて)
│
├── pass_tracing/
│   ├── core/
│   │   └── Ray.h, RayHit.h
│   ├── film/
│   │   └── drawfilm.cpp / film.cpp
│   ├── integrators/
│   │   └── Shading.cpp
│   ├── intersectors/
│   │   └── Intersection.cpp
│   ├── samplers/
│   │   └── Sampler.cpp, randomMT.cpp
│
├── scene/
│   ├── myopenglwidget.cpp
│   └── myopenglwidget_camera.cpp
│
└── main.cpp
```

---

## 📝 次のステップ

- [ ] `computeRayHitNormal()` の正確な法線補間を実装
- [ ] `AreaLight` の交差を復元（必要に応じて）
- [ ] `computeShading()` の実装と `integrators/` への移動
- [ ] `updateRayTracing()` を分割描画に改良（タイマで少しずつ）
