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

MyOpenGLWidget_cameraでは、initializeGL(),paintGL,resizeGL()でのみ呼ばれる。

@startuml
actor User

User -> MyOpenGLWidget_camera : 初期化 (initializeGL)
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : initializeFilmTexture() を呼び出し、filmバッファ作成(注意;initializeFilmTexture()はデバッグ用)

User -> MyOpenGLWidget_camera : ウィンドウリサイズ (resizeGL)
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : width, height 更新
MyOpenGLWidget_camera -> Camera : setAspectRatio()
MyOpenGLWidget_camera -> OpenGL : 古いテクスチャ削除
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : initializeFilmTexture() 再実行(注意;initializeFilmTexture()はデバッグ用)
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : updateProjectionMatrix()

User -> MyOpenGLWidget_camera : paintGL() を呼び出し
MyOpenGLWidget_camera -> MyOpenGLWidget_camera : projection_and_modelview(g_Camera2)
MyOpenGLWidget_camera -> drawFilm : カメラ情報とfilmテクスチャを渡す
drawFilm -> Camera : getEyePoint(), getX/Y/ZVector(), getFocalLength() など呼び出し
drawFilm -> OpenGL : テクスチャをバインドし、矩形に貼り付けて描画
@enduml

