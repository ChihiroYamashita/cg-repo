#include "myopenglwidget.h"
#include <QOpenGLWidget>
#include <QResizeEvent> // QResizeEventのために追加
#include <QApplication>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <Eigen/Dense>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPushButton>
#include <QtSvg/QSvgRenderer>
#include <QOpenGLFunctions>
#include <GL/glu.h>
#include "drawObject.h"
#include <iostream>

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#define _USE_MATH_DEFINES
#include <math.h>

#include "Camera.h"


int width = 500;
int height = 480;




MyOpenGLWidget::MyOpenGLWidget(QWidget *parent):
    QOpenGLWidget(parent),orthoMode(false) // 初期化リストでorthoModeをfalseに設定
{
    QSurfaceFormat format;
    cameraFov=45;


    format.setSamples(4); // MSAA(マルチサンプリングアンチエイリアス)を有効にする

    setFormat(format); // このウィジェットにフォーマットを設定


}






/*---------------自動的に呼び出されるメソッド-------------------*/


//OpenGLコンテキストが最初に作成された時に、OpenGLの初期化用に一度だけ呼び出される
void MyOpenGLWidget::initializeGL(){
initializeButtons();
    //背景色指定
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST); // 深度テストを有効にする

    //カメラ設定

    g_Camera.setEyePoint(QVector3D( 1.0, 1.0, 3.0 ));
    g_Camera.lookAt(QVector3D{ 0.0, 0.0, 0.0 }, QVector3D{ 0.0, 1.0, 0.0 });

    qDebug() << "cameraFov2initializeGL():" << cameraFov;

}


void MyOpenGLWidget::resizeGL(int w, int h)
{
width = w;
height = h;


updateProjectionMatrix();


if (button != nullptr) {
    qDebug() << "Button is valid";
    int buttonWidth = w / 8;
    int buttonHeight = h / 8;
    int buttonX = w - buttonWidth;
    int buttonY = h - buttonHeight - 10;
;
    button->setGeometry(buttonX, 0, buttonWidth, buttonHeight);
    button->setIconSize(QSize(buttonWidth, buttonHeight)); // アイコンのサイズを設定

    zoombutton->setGeometry(buttonX, buttonHeight, buttonWidth, buttonHeight);
    zoombutton->setIconSize(QSize(buttonWidth, buttonHeight)); // アイコンのサイズを設定

    // qDebug() << "Geometry set for button";
} else {
    //qDebug() << "Button is nullptr";
}
}




void MyOpenGLWidget::paintGL(){
updateProjectionMatrix(); //実際のモデルビュー・視野変換の適用は、オブジェクトが実際に描画される際にGPU内で行われる
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア
glViewport(0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y);

projection_and_modelview(g_Camera);
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

//drawPlaneInCameraCoords(g_Camera, 1);

}

/*------------------------------------------------------------*/


//画面サイズからアスペクト比を計算し、OpenGLの投影行列を設定するメソッド
void MyOpenGLWidget::updateProjectionMatrix() {
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
        gluPerspective(cameraFov, aspect, 0.01f, 100.0f);

    }

    glMatrixMode(GL_MODELVIEW);
}


void MyOpenGLWidget::projection_and_modelview(const Camera& in_Camera) {
    // ②視界変換
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const QVector3D eyePoint = in_Camera.getEyePoint();
   // qDebug() << "EyePoint:" <<in_Camera.getEyePoint();
    const QVector3D lookAtPoint = in_Camera.getLookAtPoint();
    const QVector3D upVector = in_Camera.getYVector();

    gluLookAt(eyePoint.x(), eyePoint.y(), eyePoint.z(),  // カメラの位置
              lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z(),  // 注視点
              upVector.x(), upVector.y(), upVector.z());  // 上方向ベクトル
}

/*------------------------------------------------------------*/



void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        lastMousePos = event->pos();
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::RightButton) {



        const int _dx =  event->pos().x() - lastMousePos.x();
        const int _dy =  event->pos().y() - lastMousePos.y();
        lastMousePos = event->pos();

        const double dx = double(_dx) / double(width);
        const double dy = double(_dy) / double(height);
        const double scale = 2.0;

        g_Camera.rotateCameraInLocalFrameFixLookAt(dx * scale,dy* scale);

         qDebug() << "EyePoint:" <<g_Camera.getEyePoint();

        QWidget::update();
    }
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        // 右ボタンが離された時の処理
    }
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event) {
    // ホイールの回転量を取得
    int delta = event->angleDelta().y();

    // ホイールの回転に応じてズーム量を設定
    double zoomAmount = delta > 0 ? 0.1 : -0.1;

    // カメラのズームを実行
    g_Camera.zoomCamera(zoomAmount);

    // シーンの再描画を要求
    update();
}






//特定のイベントを捕捉してカスタムの処理を行う
bool MyOpenGLWidget::eventFilter(QObject *obj, QEvent *event) {
     //ズームボタンの処理一覧
    if (obj == zoombutton && event->type() == QEvent::MouseMove) {
        //ズームボタン
        //イベントが発生したオブジェクトがzoombuttonで、かつイベントの種類がMouseMove（マウス移動）である場合、onZoomButtonMoved関数を呼び出し
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        onZoomButtonMoved(mouseEvent);
        return true;
    }
    //平行移動ボタンの処理一覧
    else if (obj == button && event->type() == QEvent::MouseMove) {
        //平行移動ボタンでマウス移動イベントが発生した場合の処理
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        onButtonMoved(mouseEvent);
        return true;
    }
    else if (obj == button && event->type() == QEvent::MouseButtonPress) {

        // イベントが発生したオブジェクトがbuttonで、かつイベントの種類がMouseButtonPressである場合
        auto *mouseEvent = static_cast<QMouseEvent *>(event); // イベントをQMouseEventにキャスト
        onButtonPressed(mouseEvent); // onButtonPressedにQMouseEventを渡して呼び出す
        return true;

    }



    //全てのボタン共通処理
    else if (event->type() == QEvent::MouseButtonRelease) {
        //イベントの種類がMouseButtonRelease（マウスボタンが放された）である場合、onButtonReleased関数を呼び出し
        onButtonReleased();
        return true;
    }

    //そうでないとき、普通のイベントを実行する
    return QOpenGLWidget::eventFilter(obj, event);
}

//ズームボタンの処理

void MyOpenGLWidget::onZoomButtonPressed() {
    zoomButtonPressed = true;
    lastZoomButtonPos = zoombutton->pos();
    originalMousePos = QCursor::pos(); // 現在のマウスポインタの位置を記録
    qDebug() << "originalMousePos_zoom:" << originalMousePos;
    QApplication::setOverrideCursor(Qt::BlankCursor); // マウスポインタを非表示にする
}

void MyOpenGLWidget::onZoomButtonReleased() {
    zoomButtonPressed = false;
    QCursor::setPos(originalMousePos); // マウスポインタを元の位置に戻す
    QApplication::restoreOverrideCursor(); // マウスポインタを再表示する

    //qDebug() << "Gn";
}
void MyOpenGLWidget::onZoomButtonMoved(QMouseEvent *event) {
    if (zoomButtonPressed) {
        int dy = event->pos().y() - lastZoomButtonPos.y();
        double zoomAmount = dy * -0.01; // ズーム量を調整
        g_Camera.zoomCamera(zoomAmount);
        //qDebug() << "dy:" <<dy<< "zoomAmount:" <<zoomAmount;
        qDebug() << "pos().y():" << pos().y() << "lastButtonPos.y():" << lastZoomButtonPos.y() ;
        lastZoomButtonPos = event->pos();
        update();
    }
}

//平行移動ボタンの処理

void MyOpenGLWidget::onButtonPressed(QMouseEvent *event) {
    ButtonPressed = true;
    lastButtonPos = event->pos(); // QMouseEventから直接ローカル座標を取得
    originalMousePos =event->globalPos(); // ウィジェットのローカル座標系からグローバル座標系に変換して記録

    //qDebug() << "originalMousePos:" << originalMousePos;
    QApplication::setOverrideCursor(Qt::BlankCursor); // マウスポインタを非表示にする
}

void MyOpenGLWidget::onButtonReleased() {
    ButtonPressed = false;
    QCursor::setPos(originalMousePos); // マウスポインタを元の位置に戻す
    QApplication::restoreOverrideCursor(); // マウスポインタを再表示する

    //qDebug() << "Gn";
}



void MyOpenGLWidget::onButtonMoved(QMouseEvent *event) {

    if (ButtonPressed) {
        // マウスの移動量を取得
        int dy = event->pos().y() - lastButtonPos.y();
        int dx = event->pos().x() - lastButtonPos.x();

        // 移動量に基づいて平行移動の量を計算
        // ここでは、Y軸方向の移動量に基づいてカメラを上下に移動。
        // 実際には、マウスの移動方向やアプリケーションの要件に応じて調整が必要。
        QVector3D delta(dx * 0.005, dy * -0.005, 0); // Y軸方向に動かすため、Y成分を使用

        // デバッグ出力
       // qDebug() << "Mouse delta X:" << dx << ", Y:" << dy << ", Move amount:" << delta;
       //qDebug() << "pos().x():" << pos().x() << "lastButtonPos.x():" << lastButtonPos.x() ;



        // カメラをローカルフレームで移動させる
        g_Camera.moveInLocalFrame(delta);

        // lastButtonPosを更新
        lastButtonPos = event->pos();

        // 画面を更新して変更を反映させる
        update();
    }
}





/*------------------------ゲッター/セッター群---------------------------------------*/



void MyOpenGLWidget::setOrthoMode(bool mode) {
    orthoMode = mode;
    //qDebug() << "mode:" <<orthoMode; 成功
    updateProjectionMatrix(); // モードが変わったら投影行列を更新
    update(); // 再描画
}


bool MyOpenGLWidget:: getOrthoMode()  {
    return orthoMode;
}

void MyOpenGLWidget::setCameraEyePoint(const QVector3D& eyePoint) {
    g_Camera.setEyePoint(eyePoint);
    update(); // カメラの状態が変わったら描画を更新する
}

void MyOpenGLWidget::setlookAtPoint(const QVector3D& lookAtPoint) {
    // 現在のカメラ位置を取得
    QVector3D eyePoint = g_Camera.getEyePoint();
    // カメラの上方向ベクトルを取得（仮定または既知の値を使用）
    QVector3D upVector =QVector3D(0.0, 1.0, 0.0);
        //g_Camera.getYVector(); // 通常はY軸方向（0,1,0）が使用される

    // CameraクラスのlookAtメソッドを呼び出し、新しい注視点に基づいてカメラの向きを設定
    g_Camera.lookAt(lookAtPoint, upVector);

    // シーンの再描画をトリガー
    update();
}

/*--------------------------camera2-------------------------------------*/


void MyOpenGLWidget::setCameraFov(float fov) {

    //qDebug() << "setCameraFov called"; 成功
    cameraFov = fov;
    emit cameraFovChanged(cameraFov); // シグナルを発行

    qDebug() << "cameraFov:" << getCameraFov();
    g_Camera.setFov(cameraFov);
    //qDebug() << "cameraFov2:" << g_Camera.getFov();
    updateProjectionMatrix();
    update(); // 画面の再描画を要求
}


QVector3D MyOpenGLWidget::getEyePoint() const {
    return g_Camera.getEyePoint();
}

QVector3D MyOpenGLWidget::getlookAtPoint() const {
    return g_Camera.getLookAtPoint();
}

QVector3D MyOpenGLWidget::getUpVector() const {
    return g_Camera.getYVector();
}

QVector3D MyOpenGLWidget::getXVector() const {
    return g_Camera.getXVector();
}

QVector3D MyOpenGLWidget::getYVector() const {
    return g_Camera.getYVector();
}

QVector3D MyOpenGLWidget::getZVector() const {
    return g_Camera.getZVector();
}

double MyOpenGLWidget::getZoom() const {
    return (double)g_Camera.getFocalLength();
}

float MyOpenGLWidget::getCameraFov() const {
    return cameraFov;
}





/*---------------------------------------------------------------*/


void MyOpenGLWidget::checkOpenGLVersion() {
    const GLubyte* version = glGetString(GL_VERSION);
    qDebug() << "OpenGL Version:" << reinterpret_cast<const char*>(version);
}

void MyOpenGLWidget::initializeButtons(){
    // 平行移動ボタン
    // 平行移動ボタンの初期化
    button = new QPushButton(this);
    button->setGeometry(400, 10, 100, 50); // 適切なサイズに設定

    // SVG アイコンを設定
    QIcon buttonIcon(":/new/prefix1/resorce/drag-move-2-line.svg");
    button->setIcon(buttonIcon);
    button->setIconSize(QSize(24, 24)); // アイコンのサイズを設定

    // ボタンのカーソルを握った手の形状に設定
    button->setCursor(Qt::PointingHandCursor);

    // ボタンの背景を透明に設定
    button->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    //buttonを押したとき、onZoomButtonPressedを実行
    //buttonを離したとき、onZoomButtonReleasedを実行
    //connect(button, &QPushButton::pressed, this, &MyOpenGLWidget::onButtonPressed);
    connect(button, &QPushButton::released, this, &MyOpenGLWidget::onButtonReleased);
    //イベントフィルタを有効にする
    button->installEventFilter(this);

    // ズームボタン
    // ズームボタンの初期化
    zoombutton = new QPushButton(this);
    zoombutton->setGeometry(400, 60, 100, 50); // 適切なサイズに設定

    // SVG アイコンを設定
    QIcon buttonIcon2(":/new/prefix1/resorce/zoom-in-line.svg");
    zoombutton->setIcon(buttonIcon2);
    zoombutton->setIconSize(QSize(24, 24)); // アイコンのサイズを設定

    // ボタンのカーソルを握った手の形状に設定
    zoombutton->setCursor(Qt::PointingHandCursor);

    // ボタンの背景を透明に設定
    zoombutton->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    //zoombuttonを押したとき、onZoomButtonPressedを実行
    //zoombuttonを離したとき、onZoomButtonReleasedを実行
    connect(zoombutton, &QPushButton::pressed, this, &MyOpenGLWidget::onZoomButtonPressed);
    /*
    connect(zoombutton, &QPushButton::released, this, &MyOpenGLWidget::onZoomButtonReleased);
    bool connected = connect(zoombutton, &QPushButton::released, this, &MyOpenGLWidget::onZoomButtonReleased);
    if (!connected) {
        qWarning() << "Connection failed!";
    }*/
    //イベントフィルタを有効にする
    zoombutton->installEventFilter(this);
}
