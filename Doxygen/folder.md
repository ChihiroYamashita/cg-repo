レイトレーシング機能を含む既存のコード（`main.cpp`, `Camera.cpp`, `TriMesh.cpp`, `GLPreview.cpp` など）をQtプロジェクトに段階的に移植する際、**目的別に役割を分けてフォルダ化・整理**することで、将来的な保守性と可読性が大きく向上します。

---

## ✅ 推奨フォルダ構成と役割

```
sources/
├── core/                  # 基本データ構造や汎用ユーティリティ
│   ├── ray.h/.cpp         # Ray, RayHit, 構造体定義
│   ├── material.h         # Material定義
│   └── utility.h/.cpp     # randomMT など
│
├── geometry/              # モデル・形状関連
│   ├── trimesh.h/.cpp     # TriMesh, extendMesh, loadObj など
│   ├── object.h/.cpp      # Object（複数Meshを含む）構造体と描画
│   └── light.h/.cpp       # AreaLight などの定義と描画
│
├── camera/                # カメラ周り
│   ├── camera.h/.cpp      # カメラの設定や screenView など
│   └── projection.h/.cpp  # OpenGL投影処理 (projection_and_modelview)
│
├── film/                  # 出力バッファ管理（レンダリング結果の保持）
│   ├── film_buffer.h/.cpp # g_FilmBuffer, init/reset/updateFilmなど
│   └── drawfilm.h/.cpp    # drawFilm
│
├── rendering/             # レンダリング全般（RT＋OpenGL両対応）
│   ├── gl_draw.cpp        # OpenGLベースの描画（drawObjectなど）
│   ├── raytracing.cpp     # rayTracing, rayTriangleIntersect など
│   └── shading.cpp        # computeShading, sampleRandomPointなど
│
└── main/                  # アプリケーションのエントリーポイント
    └── main.cpp
```

---

## ✅ 各ファイルへの分類例

| 関数/構造体                             | 移動先ファイル                    | 説明                |
| ---------------------------------- | -------------------------- | ----------------- |
| `Camera::screenView`               | `camera/camera.cpp`        | ピクセル座標→レイの生成      |
| `projection_and_modelview`         | `camera/projection.cpp`    | OpenGLの視野投影設定     |
| `TriMesh`, `loadObj`, `extendMesh` | `geometry/trimesh.cpp`     | OBJ読み込み、メッシュ管理    |
| `Object`, `drawObject`             | `geometry/object.cpp`      | 複数メッシュまとめたオブジェクト  |
| `AreaLight`, `drawLights`          | `geometry/light.cpp`       | エリアライト定義と描画       |
| `drawFilm`, `GLPreview.cpp`        | `film/drawfilm.cpp`        | 描画出力              |
| `g_FilmBuffer`, `initFilm` 等       | `film/film_buffer.cpp`     | バッファ初期化と管理        |
| `rayTriangleIntersect` 等           | `rendering/raytracing.cpp` | ジオメトリとのレイ交差       |
| `computeShading`, `samplePoint`    | `rendering/shading.cpp`    | 光の計算・間接照明など       |
| `main.cpp`                         | `main/main.cpp`            | GLUTによるループ・イベント処理 |

---

## 📌 注意点とヒント

* **描画処理（OpenGL）とレイトレーシング処理（CPU/GPU）を明確に分ける**ことで、将来のGPU化やGUIへの統合が楽になります。
* `core/` フォルダには「どこでも使われるデータ構造（`Ray`や乱数生成など）」を集めると管理しやすいです。
* `camera/` フォルダは、Qtでの `QCamera` クラスとは独立した独自カメラ管理クラスとして維持できます。

---

## ✅ .proファイルへの記述例

```pro
INCLUDEPATH += $$PWD/sources \
               $$PWD/sources/core \
               $$PWD/sources/geometry \
               $$PWD/sources/camera \
               $$PWD/sources/film \
               $$PWD/sources/rendering

SOURCES += \
    sources/main/main.cpp \
    sources/core/utility.cpp \
    sources/camera/camera.cpp \
    sources/camera/projection.cpp \
    sources/geometry/trimesh.cpp \
    sources/geometry/object.cpp \
    sources/geometry/light.cpp \
    sources/film/film_buffer.cpp \
    sources/film/drawfilm.cpp \
    sources/rendering/gl_draw.cpp \
    sources/rendering/raytracing.cpp \
    sources/rendering/shading.cpp
```

---

## ✅ 今後の発展を見据えて

| 発展項目      | ヒント                                                   |
| --------- | ----------------------------------------------------- |
| Qt GUIと連携 | `film_buffer.cpp` のテクスチャ出力を `QOpenGLWidget` にバインド可能   |
| マルチスレッド化  | `rayTracing()` を1ピクセルずつ処理しているので、スレッドで分割しやすい           |
| GPUオフロード  | `rayTracing()` や `shading` を将来的にCUDA/Metalなどへ移行しやすい構成 |

---

ご希望であれば、この構成に基づいてテンプレートとなる `.h/.cpp` 雛形をいくつか提示できますので、お気軽にどうぞ。
