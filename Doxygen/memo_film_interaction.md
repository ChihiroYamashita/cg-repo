
# 🧪 フィルムバッファとレイトレーシングの関数と相互作用

このセクションでは、`MyOpenGLWidget_camera` によるレイトレーシング描画の際に、`FilmBuffer` クラスがどのように活用されるかを関数単位で整理します。プロセス全体を視覚的に示すPlantUMLとともに、関数仕様をDoxygen記述形式で示します。

---

## 📊 全体フロー（初期化〜サンプル蓄積〜テクスチャ表示）

@startuml
actor User
participant MyOpenGLWidget_camera
participant FilmBuffer
participant OpenGL

== 初期化処理 ==
User -> MyOpenGLWidget_camera : initializeGL()
MyOpenGLWidget_camera -> FilmBuffer : init(width, height)  // バッファを初期化

== レイトレーシング処理開始 ==
User -> MyOpenGLWidget_camera : ボタンを押すことでupdateRayTracing() を呼び出し
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : for-loop 各ピクセルに対して処理

== ピクセルごとにレイを発射 ==
loop 各ピクセル
    MyOpenGLWidget_camera -> MyOpenGLWidget_camera : debug_computeNormalColor(ray) // 法線を色に変換
    MyOpenGLWidget_camera -> FilmBuffer : addSample(i, j, color) // 色をバッファに追加
end

== サンプル蓄積後 ==
MyOpenGLWidget_camera -> FilmBuffer : updateFilmBuffer() // 積算値から平均色を算出

== OpenGLへ転送 ==
MyOpenGLWidget_camera -> FilmBuffer : getFilmBufferPtr() // GPU転送用ポインタ取得
MyOpenGLWidget_camera -> OpenGL : glTexSubImage2D(..., ptr) // GPUに転送して画面描画
@enduml

---

## 🔧 関数仕様と目的（FilmBuffer 関連）

以下に、UML図で登場した関数や変数について、Doxygenスタイルの説明と `@see` リンクを加えたセクションを提示します。これにより、UML図で言及した各要素から対応する関数にジャンプ可能となる Doxygen対応コメントになります。

---

## 🧭 関数・変数概要と参照リンク

### `MyOpenGLWidget_camera::updateRayTracing()`

@brief 複数フレームに分割して全ピクセルのレイトレーシングを進める関数
@details この関数は、1フレームあたり一定数のピクセル（例：2000個）に対してレイトレーシング計算を実行し、その結果を `m_film` の蓄積バッファに加算します。各ピクセルごとにカメラからレイを発射し、交差した位置の法線情報に基づいて色を決定します。すべてのピクセルが計算されると、`m_isDirty` フラグが false となり、以降のフレームでは再計算をスキップします。

@see MyOpenGLWidget\_camera::debug\_computeNormalColor
@see FilmBuffer::addSample
@see FilmBuffer::updateFilmBuffer
@see Camera::screenView

---

### `MyOpenGLWidget_camera::debug_computeNormalColor(const Ray& ray)`

@brief オブジェクト交差点の法線ベクトルを色に変換して返す関数
@details 法線ベクトルを \[-1,1] → \[0,1] にマッピングし、RGB値として返します。デバッグ描画向け。

@see Ray
@see computeRayHitNormal

---

### `FilmBuffer::init(int width, int height)`

@brief バッファの初期化（サイズとすべての値を0にリセット）
@details 蓄積・平均・カウントの3つのバッファをリサイズしてクリアします。

@see FilmBuffer::reset
@see FilmBuffer::addSample
@see FilmBuffer::updateFilmBuffer

---

### `FilmBuffer::addSample(int i, int j, const Eigen::Vector3d& color)`

@brief ピクセルにレイトレ結果（色）を加算
@details 蓄積バッファに色を加算し、カウントをインクリメントします。

@see FilmBuffer::init
@see FilmBuffer::updateFilmBuffer

---

### `FilmBuffer::updateFilmBuffer()`

@brief 蓄積色とカウントから平均色を計算し、最終出力用バッファに格納
@details 平均を計算して `m_filmBuffer` に保存します。OpenGLへの転送準備。

@see FilmBuffer::addSample
@see FilmBuffer::getFilmBufferPtr

---

### `FilmBuffer::getFilmBufferPtr() const`

@brief OpenGL に渡すための平均色バッファの先頭ポインタを返す
@details `glTexSubImage2D()` で使用される。

@see FilmBuffer::updateFilmBuffer

---

### `Camera::screenView(int i, int j)`

@brief ピクセル位置からレイを生成するカメラ関数
@details レイの原点と方向を返し、レイトレーシングの起点となります。

@see Ray

---

### `MyOpenGLWidget_camera::m_film`

@brief レイトレーシング結果を保持するフィルムバッファ
@details `FilmBuffer` クラスのインスタンス。色加算・平均処理・描画データ保持を担当します。

@see FilmBuffer

---


