#include "myopenglwidget_camera.h"
#include "drawObject.h"
#include "Camera.h"
#include <GL/glu.h>
#include "drawfilm.h"
#include "TriMesh.h"
#include "GLPreview.h"
#include "Intersection.h"
#include "PathTracer.h"
#include "Ray.h"
#include "RayHit.h"
#include <QTimer>

MyOpenGLWidget_camera::MyOpenGLWidget_camera(QWidget* parent)
    : MyOpenGLWidget(parent) {
    // 初期化コードをここに記述

    // ★オブジェクトを読み込む
    if (loadObj( m_objFileName, g_Obj)) {
        qDebug() << "sphere.obj loaded successfully.";


        // レンダリング状態を初期化
        m_isDirty = false; // ★ 最初は計算しない
        m_progress_i = 0;
        m_progress_j = 0;


        m_targetSamplesPerPixel = 16; // 目標サンプル数を設定 (UIなどで変更可能にすると良い)
        m_currentSampleCount = 0;//今のサンプル数


        m_timer = new QTimer(this);

        //✅ QTimer が一定間隔で時間切れ（timeout）になるたびに
        //✅ MyOpenGLWidget_camera::updateRayTracing() 関数を自動的に呼び出す
        connect(m_timer, &QTimer::timeout, this, &MyOpenGLWidget_camera::updateRayTracing);

        //1ミリ秒ごとに timeout シグナルが発生。
        m_timer->start(1);// ほぼ最速で実行

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

        m_film.init(width, height);
            createDebugPattern();

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

    // !!!実行前に
    //delete[] g_FilmBuffer;
    //g_FilmBuffer = nullptr;
    doneCurrent();

    // 新しいサイズでテクスチャを初期化
    initializeFilmTexture();



    updateProjectionMatrix();
}



// ★★★ この関数が再レンダリングのトリガーになる ★★★
//視点をカメラ1→カメラ2に変換する
void MyOpenGLWidget_camera::setCamerakeyframe(const Eigen::Vector3d& eyePoint,const Eigen::Vector3d& lookAtPoint){

    setCameraEyePoint2(eyePoint);
    setlookAtPoint2(lookAtPoint);
    //qDebug() << "Parent's cameraFov:" << updatedFov;
    g_Camera2.setFov(updatedFov);

    updateProjectionMatrix();
    update();
    // ★ レンダリングをリセットして開始する
    resetRendering();

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
 // main.cppのidle()関数のロジックをここに移植

/**
 * @brief レイトレーシングを1フレーム分進める
 * 詳細は @ref updateRayTracing_doxygen "updateRayTracing()" を参照。
 */
void MyOpenGLWidget_camera::updateRayTracing()
{


    // 【注意】この実装は一度に全ピクセルを計算するため、UIが一時的に固まります。
    // 　まずは動作確認のためにこの方法を使い、次のステップで分割計算に改良します。

    // ★ 再レンダリングが不要な場合は、スキップ（何もしない）
    //if m_isDirty is false,quit
    if (!m_isDirty) {

        return;
    }

    // ★ 1フレームで処理するピクセル数（パフォーマンスと滑らかさのバランス）
    const int pixelsPerFrame = 2000;

    // ★ width分だけあるピクセルの内pixelsPerFrame 分だけピクセルごとにレイを飛ばす
    for (int k = 0; k < pixelsPerFrame; ++k) {

        // ★ 各ピクセルがサンプル数までレンダリングが完了しているか、毎ピクセルチェックする
        if (m_currentSampleCount >= m_targetSamplesPerPixel) {
            m_isDirty = false; // 目標に達したらレンダリングを停止
            qint64 elapsed_ms = m_renderTimer.elapsed(); // 経過時間をミリ秒で取得
            float elapsed_s = elapsed_ms / 1000.0f;     // 秒に変換

            qDebug() << "Rendering finished. (" << m_currentSampleCount << " samples)";
            qDebug() << "Total time:" << elapsed_s << "seconds.";


            break; // このフレームの処理を中断
        }

        //★今どこのピクセルを処理中かを0～1に正規化する。
        double p_x = (double)m_progress_i / width;
        double p_y = (double)m_progress_j / height;

        //★OutRay作成
        Ray ray;
        g_Camera2.screenView(p_x, p_y, ray);
        ray.prev_mesh_idx = -99;
        ray.prev_primitive_idx = -1;
        ray.depth = 0; // ★深度を初期化

        RayHit ray_hit;
        rayTracing(g_Obj, g_AreaLights, ray, ray_hit); // ★g_AreaLightsを渡す
        Eigen::Vector3d color = Eigen::Vector3d::Zero();
        // ★ヒットした場合のみシェーディングを計算
        if (ray_hit.primitive_idx >= 0) {
            color = computeShading(ray, ray_hit, g_Obj, g_AreaLights);
        }

        //★レイがシーンとぶつかった場所の法線ベクトルをRGB色にして取得。
        //Eigen::Vector3d color = debug_computeNormalColor(ray);
        //int index = (m_progress_j * width + m_progress_i) * 3;

        //★そのピクセルに色を記録（フィルムバッファに追加）
        m_film.addSample(m_progress_i, m_progress_j, color);
        // ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲


        // 次に計算するピクセルへ
        //m_progress_i と m_progress_j によって、「どこまで処理したか」が記録されている
        m_progress_i++;

        if (m_progress_i >= width) {
            m_progress_i = 0;
            m_progress_j++;
            if (m_progress_j >= height) {
                // ★ 全ピクセルの計算が終わったら、dirtyフラグをfalseにして計算を停止
                // 画像の最後まで到達した時の処理
                m_progress_j = 0; // ピクセル座標をリセット
                m_currentSampleCount++; // サンプル数を1増やす
                qDebug() << "Sample pass" << m_currentSampleCount << "/" << m_targetSamplesPerPixel << "done.";
            }
        }
    }

    // 蓄積バッファから平均を計算して、描画用バッファを更新
    m_film.updateFilmBuffer();

    // テクスチャ更新と再描画
    makeCurrent();
    glBindTexture(GL_TEXTURE_2D, m_filmTexture);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, g_FilmBuffer);

    // ★getFilmBufferPtr()で更新されたバッファを取得して転送
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_film.getWidth(), m_film.getHeight(), GL_RGB, GL_FLOAT, m_film.getFilmBufferPtr());

    glBindTexture(GL_TEXTURE_2D, 0);
    doneCurrent();


    update(); // paintGL()の再描画をスケジュールする
}


/*-----レイトレ用---------------------------------------*/

// ★追加: テスト用テクスチャを生成する関数の実装
void MyOpenGLWidget_camera::initializeFilmTexture() {
    // widthとheightが0以下の場合は何もしない（エラー防止）
    if (width <= 0 || height <= 0) return;

    makeCurrent(); // OpenGLの命令をこのウィジェットで実行するために必須

    // もし既にテクスチャが存在していたら、削除してメモリリークを防ぐ
    if (m_filmTexture != 0) {
        glDeleteTextures(1, &m_filmTexture);
    }

    // 1. 新しいテクスチャを1つ生成し、そのIDを m_filmTexture に格納
    glGenTextures(1, &m_filmTexture);

    // 2. 生成したテクスチャを操作対象として「バインド」する
    glBindTexture(GL_TEXTURE_2D, m_filmTexture);

    // 3. テクスチャのメモリをGPU上に確保する
    //    最後の引数をnullptrにすると、データ転送なしで領域だけ確保できる
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

    // 4. テクスチャの拡大・縮小時のフィルタリング方法を設定
    //    GL_NEAREST にすると、ピクセルがぼやけずクッキリ表示される
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // 5. 操作が終わったらテクスチャのバインドを解除（安全のため）
    glBindTexture(GL_TEXTURE_2D, 0);

    doneCurrent(); // makeCurrentと対になる
}


// ★追加:デバッグ用

// myopenglwidget_camera.cpp に追加
Eigen::Vector3d MyOpenGLWidget_camera::debug_computeNormalColor(const Ray& ray)
{
    RayHit ray_hit;
    // ★レイがシーン内のどの三角形（またはエリアライト）と最初に交差するかを探す
    //第2引数のAreaLightsはまだ使わないので空でOK
    rayTracing(g_Obj, g_AreaLights, ray, ray_hit);

    // 最初に、そもそも何かにヒットしたかをチェックする
    if (ray_hit.primitive_idx < 0) {
        // ヒットなし: 背景色（黒）を返す
        return Eigen::Vector3d::Zero();
    }

    // ヒットした場合、それがオブジェクトかライトかを判別する
    if (ray_hit.mesh_idx >= 0) {
        // オブジェクトにヒット (mesh_idxが0以上): 法線ベクトルを色として返す
        Eigen::Vector3d normal = computeRayHitNormal(g_Obj, ray_hit);
        return Eigen::Vector3d(normal.x() * 0.5 + 0.5, normal.y() * 0.5 + 0.5, normal.z() * 0.5 + 0.5);
    } else {
        // エリアライトにヒット (mesh_idxが-1): ライトの色を返す
        // primitive_idx にヒットしたライトのインデックスが格納されている
        return g_AreaLights[ray_hit.primitive_idx].color;
    }
}

/**
 * @brief デバッグ用のグラデーションパターンを生成し、フィルムバッファに書き込む関数
 */
void MyOpenGLWidget_camera::createDebugPattern() {
    // ウィジェットのサイズが不正な場合は何もしない
    if (width <= 0 || height <= 0) return;

    qDebug() << "Generating debug gradient pattern...";

    // フィルムバッファを一旦リセット
    m_film.reset();

    // 全てのピクセルをループして色を計算
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            // X座標で赤色、Y座標で緑色が変わるグラデーション
            Eigen::Vector3d color;
            color.x() = static_cast<double>(i) / (width - 1);
            color.y() = static_cast<double>(j) / (height - 1);
            color.z() = 0.5;

            // FilmBufferクラスのメソッドを使ってピクセルデータを追加
            m_film.addSample(i, j, color);
        }
    }

    // バッファの平均化処理を実行（今回はサンプルが1つなのでそのままの値になる）
    m_film.updateFilmBuffer();

    // 完成したデータをOpenGLのテクスチャに転送
    makeCurrent();
    glBindTexture(GL_TEXTURE_2D, m_filmTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, m_film.getFilmBufferPtr());
    glBindTexture(GL_TEXTURE_2D, 0);
    doneCurrent();
}

void MyOpenGLWidget_camera::resetRendering()
{
    m_isDirty = true; // 再レンダリングが必要だとマーク
    m_progress_i = 0;
    m_progress_j = 0;

    // フィルムバッファをクリアして、前の画像が残らないようにする
    /*if (g_FilmBuffer) {
        memset(g_FilmBuffer, 0, sizeof(float) * width * height * 3);
    }*/
    // ★フィルムバッファをクリア
    m_film.reset();

    // OpenGLのテクスチャもクリアしておく
    if (m_film.getWidth() > 0 && m_film.getHeight() > 0) {
        std::vector<float> clearBuffer(m_film.getWidth() * m_film.getHeight() * 3, 0.0f);
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, m_filmTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_film.getWidth(), m_film.getHeight(), GL_RGB, GL_FLOAT, clearBuffer.data());
        glBindTexture(GL_TEXTURE_2D, 0);
        doneCurrent();
    }
    m_renderTimer.start(); // ★★★ レンダリング開始と同時にタイマーをスタート ★★★
    qDebug() << "Rendering started...";
}
