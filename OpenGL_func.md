# OpenGLの愉快な関数たち

## はじめに
このページでは、ややこしいOpenGLの関数などをまとめます


- OpenGL関数

## OpenGL関数
### glMatrixMode
### **`glMatrixMode` 関数の概要**

`glMatrixMode` は、OpenGL の固定機能パイプライン（古い OpenGL）で使用される関数で、**現在操作する行列の種類を設定する**ための関数です。この設定により、行列操作（例えば、平行移動や回転、スケーリングなど）が適用される対象が切り替わります。



### **関数の定義**

```c
void glMatrixMode(GLenum mode);
```

#### **引数**
- `mode`: 操作対象の行列を指定します。
  - **`GL_MODELVIEW`**: モデルビュー行列。モデリング変換や視野変換（カメラの視点設定）に使用。
  - **`GL_PROJECTION`**: 投影行列。透視投影や平行投影の設定に使用。
  - **`GL_TEXTURE`**: テクスチャ行列。テクスチャの変換（スケール、回転、平行移動）に使用。

---

### **行列の種類と用途**

1. **`GL_MODELVIEW`（モデルビュー行列）**
   - モデリング変換（オブジェクトの平行移動、回転、スケーリング）と視野変換（カメラの視点設定）を行う行列。
   - 一般的な3Dシーンでは、この行列が頻繁に操作されます。

2. **`GL_PROJECTION`（投影行列）**
   - 投影変換（3D空間を2Dスクリーンに投影する変換）を行う行列。
   - 透視投影（`gluPerspective`など）や平行投影（`glOrtho`など）に使用されます。

3. **`GL_TEXTURE`（テクスチャ行列）**
   - テクスチャ座標の変換を行う行列。
   - テクスチャを拡大・縮小したり、回転させたりする際に使用されます。

---

### **使用例**

#### **モデルビュー行列を設定する例**
```cpp
glMatrixMode(GL_MODELVIEW);  // モデルビュー行列を選択
glLoadIdentity();            // モデルビュー行列を単位行列にリセット
glTranslatef(0.0f, 0.0f, -5.0f); // カメラを5単位だけ後ろに移動
```

#### **投影行列を設定する例**
```cpp
glMatrixMode(GL_PROJECTION); // 投影行列を選択
glLoadIdentity();            // 投影行列を単位行列にリセット
gluPerspective(45.0, 1.33, 0.1, 100.0); // 透視投影を設定
```

#### **テクスチャ行列を操作する例**
```cpp
glMatrixMode(GL_TEXTURE);    // テクスチャ行列を選択
glLoadIdentity();            // テクスチャ行列を単位行列にリセット
glScalef(2.0f, 2.0f, 1.0f);  // テクスチャ座標を2倍にスケーリング
```

---

### **`glMatrixMode` の流れと操作対象**
`glMatrixMode` を使うと、その後に続く行列操作が指定した行列（`GL_MODELVIEW`, `GL_PROJECTION`, `GL_TEXTURE`）に適用されます。

#### **具体例**
```cpp
// 投影行列を設定
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0, 1.33, 0.1, 100.0);

// モデルビュー行列を設定
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
gluLookAt(0.0, 0.0, 5.0,   // カメラ位置
          0.0, 0.0, 0.0,   // 注視点
          0.0, 1.0, 0.0);  // 上方向ベクトル
```

- **`GL_PROJECTION`** の操作:
  - 透視投影の設定（`gluPerspective`）が適用される。
- **`GL_MODELVIEW`** の操作:
  - カメラ位置と視野方向（`gluLookAt`）の設定が適用される。

---

### **注意点**

1. **モダンOpenGLでは使用されない**
   - 固定機能パイプラインを使用している場合にのみ有効です。
   - モダンOpenGL（シェーダーベース）では、行列操作は手動で計算し、シェーダーに渡します。

2. **順序に注意**
   - OpenGLの行列操作は順序が重要です。投影行列とモデルビュー行列を適切に切り替える必要があります。

---

### gluLookAt
視線ベクトル、注視点、上方向ベクトルを使ってカメラ行列を設定し、モデルビュー行列に適用します。


### glEnable(GL_DEPTH_TEST);

- 深度テスト（Depth Test）を有効化します。
- 役割: オブジェクトの奥行き情報を使って、手前にあるオブジェクトで後ろのオブジェクトを隠すように描画します。
---
### glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

- アルファブレンディング（透明度の計算）の設定を行います。
- 役割: オブジェクトの透明度（アルファ値）に基づいて、前景と背景をブレンドして描画します。
---
### glEnable(GL_BLEND);
- ブレンディングを有効化します。

### glEnable(GL_POINT_SMOOTH);, glEnable(GL_LINE_SMOOTH);, glEnable(GL_POLYGON_SMOOTH);

- 点、線、多角形のスムージング（アンチエイリアス）を有効化します。
- 役割: 描画結果を滑らかにするための設定。
### glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); など

- スムージングの品質を設定します。
- GL_NICEST: 可能な限り高品質でスムージングを行う。
