#include "myopenglwidget_camera.h"
#include "drawObject.h"
#include "Camera.h"
#include <GL/glu.h>
#include "drawfilm.h"
#include "TriMesh.h"



MyOpenGLWidget_camera::MyOpenGLWidget_camera(QWidget* parent)
    : MyOpenGLWidget(parent) {
    // 初期化コードをここに記述

    // ★オブジェクトを読み込む
    if (loadObj( "box.obj", g_Obj)) {
        qDebug() << "box.obj loaded successfully.";
    } else {
        qWarning() << "Failed to load box.obj.";
    }
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

/**
 * @brief OpenGLでシーン全体とフィルムバッファを描画する関数
 * @details この関数は、カメラ設定に基づいたプロジェクション行列の適用、OpenGLバッファの初期化、
 * 軸やグリッドなどの補助図形、そしてフィルムバッファ（レイトレーシング結果）を描画します。
 * モデルビュー変換と投影変換を適用後、描画対象をGPU上にレンダリングします。
 *
 * @details **処理の流れ**:
 *
 * ### 1. 投影行列と視野の更新
 * カメラ設定に基づいて OpenGL の投影行列を更新します。
 * @code
 * updateProjectionMatrix();
 * @endcode
 *
 * ### 2. バッファの初期化
 * カラーバッファと深度バッファをクリアし、描画領域（ビューポート）を設定します。
 * @code
 * glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 * glViewport(0, 0, width * スケールX, height * スケールY);
 * @endcode
 *
 * ### 3. モデルビュー変換の設定
 * カメラの位置と注視点に基づいて、モデルビュー変換行列を `gluLookAt` により設定します。
 * @code
 * projection_and_modelview(g_Camera2);
 * @endcode
 *
 * ### 4. 補助描画の有効化と描画
 * アルファブレンドやアンチエイリアスの設定後、軸・グリッド・立方体を描画します。
 * @code
 * drawXYZAxes();
 * drawXYGrid();
 * drawcube();
 * @endcode
 *
 * ### 5. フィルムバッファの描画
 * 深度テストを一時的に無効にし、画面前面にフィルムバッファ（レイトレの結果）を貼り付けて表示します。
 * @code
 * glDisable(GL_DEPTH_TEST);
 * drawFilm(g_Camera2, m_filmTexture);
 * glEnable(GL_DEPTH_TEST);
 * @endcode
 *
 * @see updateProjectionMatrix
 * @see projection_and_modelview
 * @see drawXYZAxes
 * @see drawFilm
 */
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
    //drawFilm(g_Camera2, m_filmTexture);
    glEnable(GL_DEPTH_TEST); // 深度テストを元に戻す
   //qDebug() << "Child sees camerafov as " <<updatedFov;
}



void MyOpenGLWidget_camera::resizeGL(int width, int height)
{
    //width = w;
    //height = h;
    // メンバ変数のwidthとheightを更新
    this->width = width;
    this->height = height;

    qDebug() <<"width"<<width;
    qDebug() <<"height"<< height;

    // ウィジェットのアスペクト比を計算
    const float aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;

    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★
    // ★ ここでCameraオブジェクトのアスペクト比も更新する ★
    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★
    g_Camera2.setAspectRatio(aspect);


    // 以前のテクスチャとバッファが存在すれば削除（メモリリーク防止）
    makeCurrent();
    if (m_filmTexture != 0) {
        glDeleteTextures(1, &m_filmTexture);
        m_filmTexture = 0;
    }
    delete[] m_filmBuffer;
    m_filmBuffer = nullptr;
    doneCurrent();

    // 新しいサイズでテクスチャを初期化
    initializeFilmTexture();



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


/*-----レイトレ用---------------------------------------*/

// ★追加: テスト用テクスチャを生成する関数の実装
void MyOpenGLWidget_camera::initializeFilmTexture() {
    // widthとheightが0以下の場合は何もしない（エラー防止）
    if (width <= 0 || height <= 0) return;

    // メンバ変数のwidthとheightを使用
    m_filmBuffer = new float[width * height * 3];
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            int index = (j * width + i) * 3;
            m_filmBuffer[index + 0] = (float)i / (width - 1);
            m_filmBuffer[index + 1] = (float)j / (height - 1);
            m_filmBuffer[index + 2] = 0.5f;
        }
    }

    makeCurrent();
    glGenTextures(1, &m_filmTexture);
    glBindTexture(GL_TEXTURE_2D, m_filmTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, m_filmBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    doneCurrent();
}

void MyOpenGLWidget_camera:: initAreaLights()
{
    AreaLight light1;
    light1.pos << -1.2, 1.2, 1.2;
    light1.arm_u << 1.0, 0.0, 0.0;
    light1.arm_v = -light1.pos.cross( light1.arm_u );
    light1.arm_v.normalize();
    light1.arm_u = light1.arm_u * 0.3;
    light1.arm_v = light1.arm_v * 0.2;

    light1.color << 1.0, 0.8, 0.3;
    //light1.color << 1.0, 1.0, 1.0;
    //light1.intensity = 64.0;
    light1.intensity = 48.0;

    AreaLight light2;
    light2.pos << 1.2, 1.2, 0.0;
    light2.arm_u << 1.0, 0.0, 0.0;
    light2.arm_v = -light2.pos.cross( light2.arm_u );
    light2.arm_v.normalize();
    light2.arm_u = light2.arm_u * 0.3;
    light2.arm_v = light2.arm_v * 0.2;

    //light2.color << 0.3, 0.3, 1.0;
    light2.color << 1.0, 1.0, 1.0;
    //light2.intensity = 64.0;
    light2.intensity = 30.0;

    g_AreaLights.push_back( light1 );
    g_AreaLights.push_back( light2 );
}
// ★追加: `drawfilm.cpp`から持ってきた描画関数の実装

