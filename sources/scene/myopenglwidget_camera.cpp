#include "myopenglwidget_camera.h"
#include "drawObject.h"
#include "Camera.h"
#include <GL/glu.h>
#include "drawfilm.h"
#include "TriMesh.h"
#include "GLPreview.h"
#include "Intersection.h"
#include "Ray.h"
#include "RayHit.h"
#include <QTimer>

MyOpenGLWidget_camera::MyOpenGLWidget_camera(QWidget* parent)
    : MyOpenGLWidget(parent) {
    // 初期化コードをここに記述

    // ★オブジェクトを読み込む
    if (loadObj( m_objFileName, g_Obj)) {
        qDebug() << "sphere.obj loaded successfully.";
        //applyMaterialColorToVertices(g_Obj);
        /*
        // ★★★★★★★★★★★ 詳細デバッグここから ★★★★★★★★★★★
        qDebug() << "--- Starting Detailed Color Debug ---";
        qDebug() << "Number of meshes found:" << g_Obj.meshes.size();

        if (!g_Obj.meshes.empty()) {
            // 最初のメッシュ（meshes[0]）を検査対象にします
            const auto& first_mesh = g_Obj.meshes[0];

            // 1. applyMaterialColorToVerticesが使うはずの「元の色」をまず確認します
            const auto& material_kd = first_mesh.material.kd;
            qDebug() << "[Check 1] Material kd for first mesh:" << material_kd.x() << material_kd.y() << material_kd.z();

            // 2. ここで色を適用する関数を呼び出します
            applyMaterialColorToVertices(g_Obj);

            // 3. 適用後の頂点カラーがどうなったかを確認します
            const auto& color_after_apply = g_Obj.meshes[0].vertex_colors[0];
            qDebug() << "[Check 2] Vertex 0 color is now:" << color_after_apply.x() << color_after_apply.y() << color_after_apply.z();
        }
        qDebug() << "--- End of Detailed Color Debug ---";*/
        // ★★★★★★★★★★★ 詳細デバッグここまで ★★★★★★★★★★★

        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &MyOpenGLWidget_camera::updateRayTracing);
        m_timer->start(16); // 約60FPSでタイムアウト信号を出す

    } else {
        qWarning() << "Failed to load box.obj.";
    }
    // ★ ここにライトの初期化処理を追加 ★
    initAreaLights(g_AreaLights);
    // ★★★★★★★★★★★★★★★★★★★★★★★

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
    //drawcube();

    // ★読み込んだオブジェクトを描画する
    applyMaterialColorToVertices(g_Obj);
    computeGLShading(g_Obj, g_AreaLights);

    drawObject(g_Obj);

    //qDebug() << "Number of lights to draw:" << g_AreaLights.size();
    drawLights(g_AreaLights);

    //drawFloor();

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




//視点をカメラ2に変換する
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

void MyOpenGLWidget_camera::updateRayTracing()
{
    /*
    // main.cppのidle()関数のロジックをここに移植
    // 例：1フレームで一定数のピクセルを計算する
    for(int i = 0; i < 1000; ++i) { // 数値は調整可能
        shadeNextPixel();
    }

    updateFilm(); // 計算結果をテクスチャバッファに反映*/

    // 【注意】この実装は一度に全ピクセルを計算するため、UIが一時的に固まります。
    // 　まずは動作確認のためにこの方法を使い、次のステップで分割計算に改良します。

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            // 1. ピクセルに対応するレイを生成
            double p_x = (double)i / width;
            double p_y = (double)j / height;
            Ray ray;
            g_Camera2.screenView(p_x, p_y, ray);
            ray.prev_mesh_idx = -99;
            ray.prev_primitive_idx = -1;

            // 2. デバッグ用関数で色を計算
            Eigen::Vector3d color = debug_computeNormalColor(ray);

            // 3. 計算結果をフィルムバッファに書き込む
            int index = (j * width + i) * 3;
            m_filmBuffer[index + 0] = color.x();
            m_filmBuffer[index + 1] = color.y();
            m_filmBuffer[index + 2] = color.z();
        }
    }

    // テクスチャを更新して再描画
    makeCurrent();
    glBindTexture(GL_TEXTURE_2D, m_filmTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, m_filmBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    doneCurrent();

    update(); // paintGL()の再描画をスケジュールする
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


// ★追加:デバッグ用

// myopenglwidget_camera.cpp に追加
Eigen::Vector3d MyOpenGLWidget_camera::debug_computeNormalColor(const Ray& ray)
{
    RayHit ray_hit;
    // 第2引数のAreaLightsはまだ使わないので空でOK
    rayTracing(g_Obj, {}, ray, ray_hit);

    // ★★★ 修正：ここでヒットしたかどうかをチェックする ★★★
    if (ray_hit.mesh_idx >= 0) {
        // ヒットした場合のみ、法線を計算して色として返す
        Eigen::Vector3d normal = computeRayHitNormal(g_Obj, ray_hit);
        return Eigen::Vector3d(normal.x() * 0.5 + 0.5, normal.y() * 0.5 + 0.5, normal.z() * 0.5 + 0.5);
    } else {
        // 何にも当たらなかった場合は背景色（黒）を返す
        return Eigen::Vector3d::Zero();
    }

    // 何にも当たらなかった場合は背景色（黒）
    return Eigen::Vector3d::Zero();
}

