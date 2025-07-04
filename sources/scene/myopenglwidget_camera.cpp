#include "myopenglwidget_camera.h"
#include "drawObject.h"
#include "Camera.h"
#include <GL/glu.h>
#include "drawfilm.h"




MyOpenGLWidget_camera::MyOpenGLWidget_camera(QWidget* parent)
    : MyOpenGLWidget(parent) {
    // 初期化コードをここに記述
}


void MyOpenGLWidget_camera::initializeGL() {
    //背景色指定
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST); // 深度テストを有効にする
    g_Camera2.setEyePoint(Eigen::Vector3d( -1.0, 2.0, 3.0 ));
    g_Camera2.lookAt(Eigen::Vector3d{ 0.0, 0.0, 0.0 }, Eigen::Vector3d{ 0.0, 1.0, 0.0 });
    updatedFov=45;

    // ★レイトレ追加: テスト用テクスチャを初期化
    initializeFilmTexture();

    //checkOpenGLVersion();
}
void MyOpenGLWidget_camera::paintGL() {
    updateProjectionMatrix(); //実際のモデルビュー・視野変換の適用は、オブジェクトが実際に描画される際にGPU内で行われる
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア
    glViewport(0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y);

    projection_and_modelview(g_Camera2);
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    drawXYZAxes();
    drawXYGrid(0.5, 50);
    drawcube();

    // ★追加: フィルム（テストテクスチャ）を描画
    // 深度テストを一時的に無効にして、常に最前面に表示されるようにする
    glDisable(GL_DEPTH_TEST);
    drawFilm(g_Camera2, m_filmTexture);
    glEnable(GL_DEPTH_TEST); // 深度テストを元に戻す
   //qDebug() << "Child sees camerafov as " <<updatedFov;
}



void MyOpenGLWidget_camera::resizeGL(int width, int height)
{
    //width = w;
    //height = h;


    updateProjectionMatrix();
}




//視点をカメラに変換する
void MyOpenGLWidget_camera::setCamerakeyframe(const Eigen::Vector3d& eyePoint,const Eigen::Vector3d& lookAtPoint){

    setCameraEyePoint2(eyePoint);
    setlookAtPoint2(lookAtPoint);
    //qDebug() << "Parent's cameraFov:" << updatedFov;
    g_Camera2.setFov(updatedFov);

    updateProjectionMatrix();
    update();

}

void MyOpenGLWidget_camera::setCameraEyePoint2(const Eigen::Vector3d& eyePoint){
    g_Camera2.setEyePoint(eyePoint);
    update(); // カメラの状態が変わったら描画を更新する
}
void MyOpenGLWidget_camera::setlookAtPoint2(const Eigen::Vector3d& lookAtPoint){
    // 現在のカメラ位置を取得
    Eigen::Vector3d eyePoint = g_Camera2.getEyePoint();

    // カメラの上方向ベクトルを取得（仮定または既知の値を使用）
    Eigen::Vector3d upVector =Eigen::Vector3d(0.0, 1.0, 0.0);
        //g_Camera.getYVector(); // 通常はY軸方向（0,1,0）が使用される

    // CameraクラスのlookAtメソッドを呼び出し、新しい注視点に基づいてカメラの向きを設定
    g_Camera2.lookAt(lookAtPoint, upVector);

    // シーンの再描画をトリガー
    update();
}


//画面サイズからアスペクト比を計算し、OpenGLの投影行列を設定するメソッド
void MyOpenGLWidget_camera::updateProjectionMatrix() {
    //③投影変換
    float aspect = float(width) / float(height ? height : 1); // アスペクト比の計算

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 投影行列をリセットし、新しいパラメータで透視投影行列を設定
    if (orthoMode) {
        // 平行投影行列を設定
        float orthoSize = 1.0; // 平行投影のサイズや範囲を適宜設定
        glOrtho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, -100.0, 100.0);
    } else {
        // 透視投影行列を設定
        gluPerspective(updatedFov, aspect, 0.01f, 100.0f);

    }

    glMatrixMode(GL_MODELVIEW);
}

void MyOpenGLWidget_camera::onCameraFovChanged(float newFov) {
    // newFovを使用して必要な処理を実行
    updatedFov=newFov;
    qDebug() << "Updated cameraFov in child:" << updatedFov;
    // 必要な場合はここでカメラの設定を更新
}

void MyOpenGLWidget_camera::saveCameraState()
{
    // カメラ状態を保存するロジックをここに実装
}

void MyOpenGLWidget_camera::loadCameraState()
{
    // 保存したカメラ状態を読み込むロジックをここに実装
}


void MyOpenGLWidget_camera::mousePressEvent(QMouseEvent *event)
{
    // 何もしない
}

void MyOpenGLWidget_camera::mouseMoveEvent(QMouseEvent *event)
{
    // 何もしない
}

void MyOpenGLWidget_camera::mouseReleaseEvent(QMouseEvent *event)
{
    // 何もしない
}

void MyOpenGLWidget_camera::wheelEvent(QWheelEvent *event)
{
    // 何もしない
}


/*-----レイトレ用-------------*/

// ★追加: テスト用テクスチャを生成する関数の実装
void MyOpenGLWidget_camera::initializeFilmTexture() {
    const int filmWidth = 256;
    const int filmHeight = 256;

    // 1. テスト用の画像データ（m_filmBuffer）を生成
    m_filmBuffer = new float[filmWidth * filmHeight * 3];
    for (int j = 0; j < filmHeight; ++j) {
        for (int i = 0; i < filmWidth; ++i) {
            int index = (j * filmWidth + i) * 3;
            m_filmBuffer[index + 0] = (float)i / (filmWidth - 1);   // Red: 横方向のグラデーション
            m_filmBuffer[index + 1] = (float)j / (filmHeight - 1);  // Green: 縦方向のグラデーション
            m_filmBuffer[index + 2] = 0.5f;                         // Blue: 固定
        }
    }

    // 2. OpenGLのテクスチャを生成
    glGenTextures(1, &m_filmTexture);
    glBindTexture(GL_TEXTURE_2D, m_filmTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, filmWidth, filmHeight, 0, GL_RGB, GL_FLOAT, m_filmBuffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // テクスチャのバインドを解除
    glBindTexture(GL_TEXTURE_2D, 0);
}



