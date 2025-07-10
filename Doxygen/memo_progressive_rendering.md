# 🌟 プログレッシブ・レンダリング仕様メモ

## 📌 概要

本プロジェクトでは、レイトレーシングによる可視化に対して「**プログレッシブ・レンダリング**」を導入しました。
これは、各ピクセルに対して複数回のサンプルを蓄積することにより、時間と共に画質を向上させる手法です。

---

## ✅ 現在の対応内容

- ✅ `FilmBuffer::addSample()` によるピクセルごとの色蓄積処理の導入
- ✅ `FilmBuffer::updateFilmBuffer()` による平均色の更新処理の導入
- ✅ `std::vector` による蓄積バッファ（`m_accumulationBuffer`）とカウントバッファ（`m_countBuffer`）の導入
- ✅ `getFilmBufferPtr()` を用いた `OpenGL` へのバッファ転送対応

---

## 📂 関連ファイル

- `film_buffer.h / .cpp`: プログレッシブ蓄積処理の本体クラス
- `myopenglwidget_camera.cpp`: `updateRayTracing()` によって `addSample()` を呼び出し、`updateFilmBuffer()` で色を平均化して描画

---

## 🧠 動作の流れ

```plantuml
@startuml
actor User
User -> MyOpenGLWidget_camera : paintGL()
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : updateRayTracing()

loop 各ピクセル
    MyOpenGLWidget_camera -> Camera : screenView(i, j) -> Ray
    MyOpenGLWidget_camera -> rayTracing : 交差判定
    MyOpenGLWidget_camera -> computeRayHitNormal : 法線取得
    MyOpenGLWidget_camera -> debug_computeNormalColor : 色取得
    MyOpenGLWidget_camera -> FilmBuffer : addSample(i, j, color)
end

MyOpenGLWidget_camera -> FilmBuffer : updateFilmBuffer()
MyOpenGLWidget_camera -> OpenGL : glTexSubImage2D() による描画更新
@enduml
```

---

## 🧱 クラス構成（簡易）

```plantuml
@startuml
class FilmBuffer {
    - int m_width, m_height
    - vector<float> m_filmBuffer
    - vector<float> m_accumulationBuffer
    - vector<int>   m_countBuffer

    + init(width, height)
    + reset()
    + addSample(i, j, color)
    + updateFilmBuffer()
    + getFilmBufferPtr()
}
@enduml
```

---

## 🔄 更新すべき関数（ステップ案）

| ステップ | 関数名                    | 内容                                               |
|--------|-------------------------|--------------------------------------------------|
| 1      | `addSample()`           | ピクセルごとに色を加算（合計）                    |
| 2      | `updateFilmBuffer()`    | サンプル数で割って平均色に変換                     |
| 3      | `getFilmBufferPtr()`    | OpenGL描画用にバッファを取得                        |
| 4      | `reset()`               | 描画開始時にバッファ初期化（任意）                 |

---

## 🔍 備考

- 各ピクセルのサンプル数が異なるため、`countBuffer` により平均計算を適切に行う。
- `float` による色バッファの扱いにより、任意精度のサンプリングが可能。
- サンプル数を変数で制御すれば、将来的に「適応的サンプリング」にも応用可能。