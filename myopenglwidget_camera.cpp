#include "myopenglwidget_camera.h"
#include "drawObject.h"
#include "GLPreview.h"
#include "Camera.h"
#include "RayTracingInternalData.h"
#include "random.h"
#include "ray.h"
#include "RayHit.h"
#include "RayTracer.h"

#include <GL/glu.h>
#include <QTimer>
#include <Eigen/Dense>
const int g_FilmWidth = 640;
const int g_FilmHeight = 480;
bool g_DrawFilm = true;
GLuint g_FilmTexture = 0;
float* g_FilmBuffer = nullptr;
float* g_AccumulationBuffer = nullptr;
int* g_CountBuffer = nullptr;
int nSamplesPerPixel = 4;

Object g_Obj;
std::vector<AreaLight> g_AreaLights;


RayTracingInternalData g_RayTracingInternalData;

MyOpenGLWidget_camera::MyOpenGLWidget_camera(QWidget* parent)
    : MyOpenGLWidget(parent) {
    // 初期化コードをここに記述


    //idle()相当処理用
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MyOpenGLWidget_camera::updateFrame);
    timer->start(1000 / 60); // 60fps 相当
}


void MyOpenGLWidget_camera::initializeGL() {
    //背景色指定
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST); // 深度テストを有効にする
    g_Camera2.setEyePoint(Eigen::Vector3d( -1.0, 2.0, 3.0 ));
    g_Camera2.lookAt(Eigen::Vector3d{ 0.0, 0.0, 0.0 }, Eigen::Vector3d{ 0.0, 1.0, 0.0 });
    updatedFov=45;

    //checkOpenGLVersion();
    initFilm();
}

/**
 * @brief OpenGL描画処理を行う関数
 * @details この関数は、QtのOpenGLウィジェット内で呼び出され、3Dシーンの描画を行います。
 * 投影行列やモデルビュー行列の設定、バッファのクリア、アンチエイリアス設定、座標軸やグリッドの描画を行います。
 *
 * @details **処理の流れ**:
 *
 * #### 1. プロジェクション行列の更新
 * 投影行列を更新し、カメラの設定を適用します。
 * 実際のモデルビュー・視野変換はGPU内でオブジェクト描画時に行われます。
 * @code
 * updateProjectionMatrix();
 * @endcode
 *
 * #### 2. バッファのクリア
 * カラーバッファと深度バッファをクリアします。
 * @code
 * glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 * @endcode
 *
 * #### 3. ビューポート設定
 * ウィジェットのサイズとスケールに基づいてビューポートを設定します。
 * @code
 * glViewport(0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y);
 * @endcode
 *
 * #### 4. 投影とモデルビュー行列の設定
 * カメラの投影とモデルビュー行列を設定します。
 * @code
 * projection_and_modelview(g_Camera2);
 * @endcode
 *
 * #### 5. 描画の前処理
 * - 深度テストを有効化します（`GL_DEPTH_TEST`）。
 * - アルファブレンドとアンチエイリアスの設定を行います。
 * @code
 * glEnable(GL_DEPTH_TEST);
 * glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 * glEnable(GL_BLEND);
 * glEnable(GL_POINT_SMOOTH);
 * glEnable(GL_LINE_SMOOTH);
 * glEnable(GL_POLYGON_SMOOTH);
 * glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
 * glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
 * glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
 * @endcode
 *
 * #### 6. シーンの描画
 * - 座標軸（XYZ軸）を描画します。
 * - グリッド（XY平面のグリッド）を描画します。
 * - キューブを描画します。
 * @code
 * drawXYZAxes();
 * drawXYGrid(0.5, 50);
 * drawcube();
 * @endcode
 *
 * #### 7. デバッグ出力
 * カメラの視野角をデバッグ出力します（`qDebug`）。
 * @code
 * qDebug() << "Child sees camerafov as " << updatedFov;
 * @endcode
 *
 * ### シーケンス図
 * @startuml
 * participant OpenGLWidget as W
 * participant OpenGL as GL
 * participant Camera as C
 *
 * W -> C: updateProjectionMatrix()
 * W -> GL: Clear buffers (Color & Depth)
 * W -> GL: Set viewport
 * W -> C: Apply projection and modelview matrices
 * W -> GL: Enable depth test, blending, and smoothing
 * W -> GL: Draw XYZ Axes
 * W -> GL: Draw XY Grid
 * W -> GL: Draw Cube
 * W -> W: Output debug information
 * @enduml
 *
 * @see glClear
 * @see glViewport
 * @see glEnable
 * @see glBlendFunc
 * @see glHint
 */

void MyOpenGLWidget_camera::paintGL() {
    updateCameraScreenSize();
    updateProjectionMatrix(); //実際のモデルビュー・視野変換の適用は、オブジェクトが実際に描画される際にGPU内で行われる
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア

// qDebug() << " g_FrameSize_WindowSize_Scale_x:" <<  g_FrameSize_WindowSize_Scale_x;

    glViewport(0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y);

    projection_and_modelview(g_Camera2);
    glEnable(GL_DEPTH_TEST);

//qDebug() << "Camera Screen Width:" << g_Camera2.getScreenWidth();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
/*
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    */
    drawXYZAxes();
    drawXYGrid(0.5, 50);
    drawcube();
    drawFloor();
    // drawFilm 関数を呼び出す
    if (g_DrawFilm) { // g_DrawFilm が true の場合に呼び出す
        drawFilm(g_Camera2, g_FilmTexture); // g_FilmTexture が初期化されていることを確認
    }

   //qDebug() << "Child sees camerafov as " <<updatedFov;
}



void MyOpenGLWidget_camera::resizeGL(int width, int height)
{
    //width = w;
    //height = h;


    updateProjectionMatrix();
}


void MyOpenGLWidget_camera::updateCameraScreenSize() {
    //float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float aspectRatio = static_cast<float>(height) / static_cast<float>(width);

    double baseWidth = 0.036;
    g_Camera2.setScreenWidth( baseWidth);
    g_Camera2.setScreenHeight(aspectRatio * baseWidth);
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


/*---------pass_tracing追加-----------*/

void resetFilm()
{
    memset( g_AccumulationBuffer, 0, sizeof(float) * g_FilmWidth * g_FilmHeight * 3 );
    memset( g_CountBuffer, 0, sizeof(int) * g_FilmWidth * g_FilmHeight );
}

void initFilm()
{
    g_FilmBuffer = (float*)malloc( sizeof(float) * g_FilmWidth * g_FilmHeight * 3 );
    g_AccumulationBuffer = (float*)malloc( sizeof(float) * g_FilmWidth * g_FilmHeight * 3 );
    g_CountBuffer = (int*)malloc( sizeof(int) * g_FilmWidth * g_FilmHeight );
    resetFilm();

    glGenTextures( 1, &g_FilmTexture );
    glBindTexture( GL_TEXTURE_2D, g_FilmTexture );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, g_FilmWidth, g_FilmHeight, 0, GL_RGB, GL_FLOAT, g_FilmBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
     qDebug() << "initFilm is executed sucessfully";
}

void updateFilm()
{
     for( int i=0; i<g_FilmWidth * g_FilmHeight; i++ )
     {
        if( g_CountBuffer[i] > 0 )
        {
            g_FilmBuffer[i*3] = g_AccumulationBuffer[i*3] / g_CountBuffer[i];
            g_FilmBuffer[i*3+1] = g_AccumulationBuffer[i*3+1] / g_CountBuffer[i];
            g_FilmBuffer[i*3+2] = g_AccumulationBuffer[i*3+2] / g_CountBuffer[i];
        }
        else
        {
            g_FilmBuffer[i*3] = 0.0;
            g_FilmBuffer[i*3+1] = 0.0;
            g_FilmBuffer[i*3+2] = 0.0;
        }
     }

     glBindTexture( GL_TEXTURE_2D, g_FilmTexture );
     glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, g_FilmWidth, g_FilmHeight, GL_RGB, GL_FLOAT, g_FilmBuffer);
}

//idle()相当処理用
void MyOpenGLWidget_camera::updateFrame()
{
     // 1フレーム分のピクセルを処理
     for (int i = 0; i < g_FilmWidth * g_FilmHeight; i++) {
        shadeNextPixel();
     }

     // テクスチャ更新
     updateFilm();

     // Qtで再描画をリクエスト
     update();
}

//仕様はVSの方に記載
void MyOpenGLWidget_camera::shadeNextPixel()
{
     stepToNextPixel( g_RayTracingInternalData );

     const int pixel_flat_idx = g_RayTracingInternalData.nextPixel_j * g_FilmWidth + g_RayTracingInternalData.nextPixel_i;

     Eigen::Vector3d I = Eigen::Vector3d::Zero();

     for( int k=0; k<nSamplesPerPixel; k++ )
     {
        double p_x = ( g_RayTracingInternalData.nextPixel_i + randomMT() ) / g_FilmWidth;
        double p_y = ( g_RayTracingInternalData.nextPixel_j + randomMT() ) / g_FilmHeight;

        Ray ray; ray.depth = 0;
        g_Camera2.screenView( p_x, p_y, ray );
        ray.prev_mesh_idx = -99; ray.prev_primitive_idx = -1;

        RayHit ray_hit;
        rayTracing( g_Obj, g_AreaLights, ray, ray_hit );

        if( ray_hit.primitive_idx >= 0 )
        {
            I += computeShading( ray, ray_hit, g_Obj, g_AreaLights );
        }
     }

     g_AccumulationBuffer[pixel_flat_idx*3] += I.x();
     g_AccumulationBuffer[pixel_flat_idx*3+1] += I.y();
     g_AccumulationBuffer[pixel_flat_idx*3+2] += I.z();
     g_CountBuffer[pixel_flat_idx] += nSamplesPerPixel;
}

void stepToNextPixel( RayTracingInternalData& io_data )
{
     io_data.nextPixel_i++;
     if( io_data.nextPixel_i >= g_FilmWidth )
     {
        io_data.nextPixel_i = 0;
        io_data.nextPixel_j++;

        if( io_data.nextPixel_j >= g_FilmHeight )
        {
            io_data.nextPixel_j = 0;
        }
     }
}

