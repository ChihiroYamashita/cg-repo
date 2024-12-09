
# シーケンス図一覧
@brief プロジェクト内のシーケンス図をまとめたグループ
@{

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
@details update()関数によってpaintGL()が自動で呼び出されたあとの挙動

@startuml
actor User


MyOpenGLWidget -> MyOpenGLWidget : paintGL()
MyOpenGLWidget -> MyOpenGLWidget : 投影変換を適用（gluPerspective / glOrtho）
MyOpenGLWidget -> OpenGL : ビューポート変換を適用（glViewport）
MyOpenGLWidget -> OpenGL : シーンをレンダリング
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

