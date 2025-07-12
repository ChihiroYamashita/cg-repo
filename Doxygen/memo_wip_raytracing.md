# レイトレーシング移植仕様メモ

このドキュメントは、MyOpenGLWidget_cameraベースで段階的にレイトレーシング機能を移植していく際の設計方針と、クラス・関数・ファイル構成を整理したものです。

---


---

はい、可能です。以下のようにすべての関数に Doxygen の `@ref` リンクを埋め込んだ形式に書き換えました：

---

## ✅ 現在の移植状況（関数・構造体）

### 🎨 表示・描画関連

* ✅ @ref MyOpenGLWidget\_camera::drawFilm() "drawFilm()" によるフィルム表示機能 → 完了
* ✅ @ref MyOpenGLWidget\_camera::paintGL() "paintGL()" → Qt側の描画呼び出しを確認・対応済み
* ✅ @ref MyOpenGLWidget\_camera::updateRayTracing() "updateRayTracing()" → 全ピクセル一括更新による可視化テスト完了

### 📡 レイ生成・交差関連

* ✅ @ref Camera::screenView() "screenView()"（Cameraクラス）→ ピクセル座標からレイ生成済み
* ✅ @ref rayTracing() "rayTracing()" → 最小限版を移植済み（光源処理は除外）
* ✅ @ref meshTriangleIntersect() "meshTriangleIntersect()" → 移植済み
* ✅ @ref intersectTriangle() "intersectTriangle()" → 移植済み

### 🧭 法線・色処理

* ✅ @ref MyOpenGLWidget\_camera::debug\_computeNormalColor() "debug\_computeNormalColor()"（法線を色で可視化） → 実装済み
* ✅ @ref computeRayHitNormal() "computeRayHitNormal()"（交差点での法線補間） → 実装済み

### 🧮 フィルムバッファ関連（FilmBuffer）

* ✅ @ref FilmBuffer::init() "init()" → バッファ（積算・カウント・平均）をサイズに応じて初期化
* ✅ @ref FilmBuffer::addSample() "addSample()" → 指定ピクセルに色を加算し、カウントをインクリメント
* ✅ @ref FilmBuffer::updateFilmBuffer() "updateFilmBuffer()" → 蓄積値から平均色を求めて最終出力用バッファに格納
* ✅ @ref FilmBuffer::getFilmBufferPtr() "getFilmBufferPtr()" → 平均化バッファ先頭ポインタをOpenGL転送用に取得

---

### 📦 使用構造体・クラス

* ✅ @ref Ray "Ray" → レイの定義構造体（原点、方向、交差履歴含む）
* ✅ @ref RayHit "RayHit" → 交差情報（メッシュ番号・三角形番号・距離など）
* ✅ @ref Camera "Camera" → レイ生成処理用クラス（視点・画角など保持）
* ✅ @ref TriMesh "TriMesh" → メッシュ構造体（頂点・法線・三角形など）
* ✅ @ref MyOpenGLWidget\_camera "MyOpenGLWidget\_camera" → レイ生成・描画処理を統括するクラス
* ✅ @ref FilmBuffer "FilmBuffer" → 色の蓄積・平均処理を行う描画バッファ管理クラス



---




---
## 全体フロー





@startuml
actor User
participant MyOpenGLWidget_camera
participant Camera
participant Intersection
participant TriMesh
participant Geometry as "computeRayHitNormal"
participant FilmBuffer
participant OpenGL

== 初期化処理 ==
User -> MyOpenGLWidget_camera : initializeGL()
MyOpenGLWidget_camera -> FilmBuffer : init(width, height) // フィルムバッファを初期化

== レイトレーシング処理開始 ==
User -> MyOpenGLWidget_camera : updateRayTracing()
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : for文で次のピクセル(i, j)へ

loop updateRayTracing() 各ピクセル (1フレームあたり数千個)
    MyOpenGLWidget_camera -> Camera : screenView(i, j) // カメラからレイ生成
    Camera --> MyOpenGLWidget_camera : Ray

    MyOpenGLWidget_camera -> Intersection : rayTracing(g_Obj, {}, ray, ray_hit) // レイとシーン交差判定

    loop 各メッシュ・各三角形
    MyOpenGLWidget_camera -> Intersection : rayTriangleIntersect(mesh[m], tri[k], ray, temp_hit)
    alt ヒットかつ最短距離なら
        Intersection -> MyOpenGLWidget_camera : RayHit を更新
    end
end
    Intersection --> MyOpenGLWidget_camera : 最短距離のRayHit

    MyOpenGLWidget_camera -> Geometry : computeRayHitNormal(g_Obj, ray_hit) // 補間法線を取得
    Geometry --> MyOpenGLWidget_camera : 法線ベクトル

    MyOpenGLWidget_camera -> MyOpenGLWidget_camera : debug_computeNormalColor(ray) // 法線を色に変換
    MyOpenGLWidget_camera -> FilmBuffer : addSample(i, j, color) // 色をフィルムに加算
end


== 全ピクセル終了時 ==
MyOpenGLWidget_camera -> FilmBuffer : updateFilmBuffer() // 平均色を計算し表示用バッファを更新

== GPU転送・描画 ==
MyOpenGLWidget_camera -> FilmBuffer : getFilmBufferPtr() // 転送ポインタ取得
MyOpenGLWidget_camera -> OpenGL : glTexSubImage2D(..., ptr) // OpenGLに描画
@enduml

---

@startuml

actor User
participant "MyOpenGLWidget_camera" as Widget
participant "QTimer" as Timer
participant "Camera" as Cam
participant "Intersection" as Isect
participant "FilmBuffer" as Film
participant "OpenGL" as GL


activate Widget
== 初期化とタイマー開始 ==
note right of Widget: MyOpenGLWidget_cameraコンストラクタ
Widget -> Timer : connect(timeout, updateRayTracing)
Widget -> Timer : start(1) //タイマーをセット
Widget -> Widget : m_isDirty = false // 再レンダリングフラグ=false
deactivate Widget

== レンダリングループ (タイマー駆動) ==
User -> Widget : フレーム登録する
activate Widget
note right of Widget: フレーム登録されるとsetCamerakeyframe→resetRendering() により m_isDirtyフラグ変更
Widget -> Widget : m_isDirty = true // 再レンダリングフラグ=true
Widget -> Timer : trueになったことによりupdateRayTracing()の処理開始



Timer -> Widget : updateRayTracing()
activate Widget #peachpuff
note right of Widget: レイトレーシング開始

loop updateRayTracing 

Widget -> Cam : screenView(p_x, p_y, ray)
activate Cam
Cam --> Widget : out_Ray生成
deactivate Cam

Widget -> Widget : debug_computeNormalColor(ray)
activate Widget #LightBlue
note right of Widget: debug_computeNormalColor(ray)

== レイとオブジェクト・ライトの交差計算 ==
Widget -> Isect : rayTracing(g_Obj, g_AreaLights, ray, ray_hit)
activate Isect #Coral
note left of Isect #lightyellow: Intersection.cpp内rayTracingの処理

loop 各メッシュの各三角形に対して交差計算
    Isect -> Isect : rayTriangleIntersect(mesh, k, in_Ray, temp_hit)
    activate Isect #DarkSalmon
    Isect --> Isect : temp_hit
    deactivate Isect #DarkSalmon
end

loop 各エリアライトに対して交差計算
    Isect -> Isect : rayAreaLightIntersect(g_AreaLights, l, in_Ray, temp_hit)
    activate Isect #DarkSalmon
    Isect --> Isect : temp_hit
    deactivate Isect #DarkSalmon
end
Isect --> Widget : ray_hit (最短交差点)レイがシーン内のどの三角形（またはエリアライト）と最初に交差するかを返す
deactivate Isect #Coral


alt ヒットした場合 (ray_hit.mesh_idx >= 0)
    == 法線計算と色決定 ==
    Widget -> Isect : computeRayHitNormal(g_Obj, ray_hit)
    activate Isect #LightGreen
    Isect --> Widget : normalを返す
    deactivate Isect #LightGreen

    Widget --> Widget : color (法線から計算された出力色（RGB）を返す)
else ヒットしない場合
    Widget --> Widget : color (背景色)
end
deactivate Widget #LightBlue


== 計算結果をフィルムに蓄積 ==
Widget -> Film : addSample(i, j, color)//色をバッファに追加
activate Film
deactivate Film
end

== フィルムバッファ更新と描画 ==
Widget -> Film : updateFilmBuffer()// 平均色を計算し表示用バッファを更新
activate Film
Film --> Widget :
deactivate Film

Widget -> Film : getFilmBufferPtr()//GPU用転送ポインタ
activate Film
Film --> Widget : buffer_ptr
deactivate Film

Widget -> GL : glBindTexture(m_filmTexture)//GPUに転送して画面描画
Widget -> GL : glTexSubImage2D(..., buffer_ptr)
Widget -> GL : glBindTexture(0)

Widget -> Widget : update() // 再描画を要求 (paintGLが呼ばれる)
deactivate Widget #peachpuff
deactivate Widget
@enduml
---


---
## 🔧 初期化処理
`MyOpenGLWidget_camera` クラスの**コンストラクタ**では、主に以下の初期化処理が行われています。

---


```cpp
MyOpenGLWidget_camera::MyOpenGLWidget_camera(QWidget* parent)
    : MyOpenGLWidget(parent) {
```
以下は、**Markdown（md）ファイル用の書式**で、コンストラクタ内の処理を「説明 ⇄ 対応するコード」を交互に配置したスタイルです。GitHubなどのMarkdownビューアでも見やすく表示されます。

---

## 🔧 コンストラクタの処理内容（やっていること）

### ① モデル（OBJファイル）を読み込む

```cpp
if (loadObj(m_objFileName, g_Obj)) {
    qDebug() << "sphere.obj loaded successfully.";
    ...
} else {
    qWarning() << "Failed to load box.obj.";
}
```

> `m_objFileName` で指定された3Dモデルを読み込み、グローバル変数 `g_Obj` に格納します。

---

### ② レンダリング不要フラグを設定

```cpp
m_isDirty = false; // ★ 最初は計算しない
```

> 最初は描画処理をスキップするため、`m_isDirty` を `false` にします。

---

### ③ レイトレ用ピクセル位置の初期化

```cpp
m_progress_i = 0;
m_progress_j = 0;
```

> 次にレンダリングするピクセル座標を `(0, 0)` に初期化します。

---

### ④ タイマーを使って非同期レンダリング処理を登録

```cpp
m_timer = new QTimer(this);
connect(m_timer, &QTimer::timeout, this, &MyOpenGLWidget_camera::updateRayTracing);
m_timer->start(1); // ほぼ最速で実行
```
###  connect関数；`QTimer` が一定間隔で時間切れ（timeout）になるたびに

###  `MyOpenGLWidget_camera::updateRayTracing()` 関数を自動的に呼び出す

> `QTimer` によって 1ミリ秒ごとに `updateRayTracing()` が呼ばれ、滑らかに描画が進みます。

---

### ⑤ エリアライトの初期化

```cpp
initAreaLights(g_AreaLights);
```

> グローバル変数 `g_AreaLights` にライトを追加し、ライティング処理を有効にします。

---

必要であれば、Doxygen用のスタイルや、表形式に戻したバージョンも用意できます！



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
