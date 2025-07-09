
# シーケンス図一覧
@brief プロジェクト内のシーケンス図をまとめたグループ
@{

### リンク
openGLのmyopenglwiget.cppの座標変換に関しては [こちら](coordinate_transformation1.md) を参照してください。<br>
openGLの基本については[こちら](openGL_basis.md)を参照してください。<br>
レイトレのディレクトリの基本構成は[こちら](folder.md)を参照してください。<br>
filmのレイトレース結果等の出力画像を画面に表示する機構の整理は[こちら](memo_film.md)<br>

レイトレ簡易実装のせいりは[こちら](memo_wip_raytracing.md)<br>



キーフレームカメラの処理については @ref MainWindow::on_keyframeCameraButton_clicked を参照。

### 全体ざっくり
@brief こんな感じのソフト


@startuml
actor User

User -> MainWindow : スライダー操作などのユーザー入力
MainWindow -> MyOpenGLWidget : カメラパラメータを更新（FOV、位置など）
MyOpenGLWidget -> Camera : モデリング変換を適用（平行移動(setEyePoint)、回転、スケーリング）
Camera -> MyOpenGLWidget : ModelView行列を更新

MyOpenGLWidget -> Camera : 視野変換を適用（gluLookAt）
MyOpenGLWidget -> MyOpenGLWidget : 投影変換を適用（gluPerspective / glOrtho）
MyOpenGLWidget -> OpenGL : ビューポート変換を適用（glViewport）
MyOpenGLWidget -> OpenGL : シーンをレンダリング
@enduml

### カメラ位置変更後の情報取得フロー
@brief 挙動～
@details QOpenGLWidgetなどのクラスでupdate()を呼び出すと、内部的にOpenGLの再描画メソッド（通常paintGL()）が実行されるため、このような記述になっている。
ここまででカメラの位置情報をどうやって習得するかを記述した。次のセクションでこれらの情報を下にワールド座標系→ビューポート表示までの挙動を記述する。
@startuml
actor User

User -> MainWindow : スライダー操作/マウス操作\n（拡縮、回転、平行移動）
MainWindow -> MyOpenGLWidget : setCameraEyePoint(QVector eyePoint)\n目の位置を更新
MyOpenGLWidget -> Camera : setEyePoint( QVector3D in_eyePoint )
Camera --> MyOpenGLWidget : QVector3D in_eyePoint\n目の位置を返す

MainWindow -> MyOpenGLWidget : setlookAtPoint(QVector3D lookatPoint)\n注視点の位置を更新
MyOpenGLWidget -> Camera : getEyePoint( QVector3D in_eyePoint )
Camera --> MyOpenGLWidget : QVector3D in_eyePoint\n目の位置を返す
MyOpenGLWidget -> Camera :lookAt( QVector3D in_LookAt, QVector3D in_Up )\n注視点を設定
Camera -> Camera  : lookAt( QVector3D in_LookAt, QVector3D in_Up )\nカメラの座標軸（m_xVector, m_yVector, m_zVector）を再設定


MyOpenGLWidget -> OpenGL : update()\nすべての処理が終わったら画面を再描画（正確な記述は次項）


@enduml
### 視野変換→ビューポート変換までのフロー
@brief 
@details update()関数によってpaintGL()が自動で呼び出されたあとのpaintGL()の挙動\n
[基礎情報ページ](about_convertions.md)に変換周りの挙動を記述。
MyOpenGLWidget::projection_and_modelview
MyOpenGLWidget::updateProjectionMatrix()　を参照。
@startuml
actor User



MyOpenGLWidget -> MyOpenGLWidget : updateProjectionMatrix();\n投影変換を適用（gluPerspective / glOrtho）
MyOpenGLWidget -> OpenGL : glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)\nカラーバッファ（画面上のピクセルデータ）と深度バッファ（奥行き情報）をクリア\nglViewport(0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y);\n画面上で描画が行われる領域を設定

MyOpenGLWidget -> MyOpenGLWidget : projection_and_modelview(g_Camera);\nカメラの投影行列とモデルビュー行列を設定
MyOpenGLWidget -> OpenGL : glEnable(GL_DEPTH_TEST);\n深度テスト（Depth Test）を有効化\nglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);\nアルファブレンディング（透明度の計算）\nglEnable(GL_BLEND);ブレンディングを有効化\nglEnable(GL_POINT_SMOOTH);, glEnable(GL_LINE_SMOOTH);, glEnable(GL_POLYGON_SMOOTH);\n（アンチエイリアス）を有効化\nglHint(GL_POINT_SMOOTH_HINT, GL_NICEST);\nスムージング
MyOpenGLWidget -> drawObject :drawXYZAxes(); drawXYGrid(0.5, 50); drawcube();
@enduml
projection_and_modelview

### キーフレームの追加

@startuml
User -> Timeline : Add keyframe
Timeline -> CustomScene : Insert keyframe
CustomScene -> MainWindow : Notify keyframe added
@enduml


### 視野角（FOV）の変更
 @brief FOVを変更するスロット
ユーザーがスライダーを操作してカメラの視野角を変更します。

 シーケンス図:

 @startuml
 User -> MainWindow : on_fovSlider_valueChanged(int value)\n FOV sliderをいじる
 MainWindow -> MyOpenGLWidget : setCameraFov(float fov)\n カメラFOVを更新
 MyOpenGLWidget -> Camera : setFov(float cameraFov)\n 新しい FOVを設置
  Camera --> MyOpenGLWidget : getFov(float cameraFov)\n FOVを更新
  @enduml
 


 @}

