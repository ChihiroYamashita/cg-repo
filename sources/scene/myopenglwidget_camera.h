#ifndef OPENGLWIDGET_CAMERA_H
#define OPENGLWIDGET_CAMERA_H
#include <QOpenGLWidget>
#include <QObject>
#include "myopenglwidget.h" // MyOpenGLWidget クラスをインクルード
#include <QOpenGLFunctions>
#include "TriMesh.h"
#include "Light.h"
#include "Ray.h"
#include "film_buffer.h"
#include <QElapsedTimer>
#include <QtConcurrent/QtConcurrent>
class MyOpenGLWidget_camera :public MyOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget_camera(QWidget* parent = nullptr);
    void setCamerakeyframe(const Eigen::Vector3d& eyePoint,const Eigen::Vector3d& lookAtPoint);//カメラ1からのいち情報をもらう関数（画面2用）
    void setCameraEyePoint2(const Eigen::Vector3d& eyePoint);//MyOpenGLWidgetの外からカメラの画角を設定する関数
    void setlookAtPoint2(const Eigen::Vector3d& lookAtPoint);


signals:
         // ♥★★★ レンダリングの1パス完了を通知するシグナル ★★★
    void renderingProgressUpdated();

public slots:
    void onCameraFovChanged(float newFov); // シグナルに応答するスロット
protected:
    void initializeGL()override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void updateProjectionMatrix() override;


    // カメラの状態を保存・設定するためのメソッド（必要に応じて実装）



    void saveCameraState();
    void loadCameraState();
    //レイトレ用
    //void initAreaLights();
    void resetRendering();


    QElapsedTimer m_renderTimer; // ★★★ 計測用のタイマーを追加 ★★★

    // ▼▼▼ 以下のタイマー関連変数は不要になるため削除 ▼▼▼
    // 追加：レンダリングの進捗と状態を管理する変数
    //レイトレ用Qtimer
    QTimer *m_timer; // タイマーのポインタ
    bool m_isDirty;     // 再レンダリングが必要かどうかのフラグ
    int m_progress_i;   // 次に計算するピクセルの横位置(i)
    int m_progress_j;   // 次に計算するピクセルの縦位置(j)
    // ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲




private slots:
    // ♥★★★ シグナルを受け取ってUIを更新するスロット ★★★
    void onRenderingProgressUpdated();

private:
    Camera g_Camera2;
    void drawCube();
    GLdouble vertex[8][3];
    int face[6][4];
    GLdouble color[6][3];
    float updatedFov;

    //--------------レイトレ用---------------------------------------
    void initializeFilmTexture();

    void renderTask(); // ♥★★★ レンダリング処理を行うワーカースレッド用の関数 ★★★

    /**
     * @var m_filmTexture
 * @brief フィルム画像を表示するためのOpenGLテクスチャID
 *
 * @details この変数は、OpenGLのテクスチャオブジェクトを識別する「番号（ID）」です。
 * 実際の色データがこの変数に格納されるわけではなく、このIDが指し示す**GPU上のテクスチャ領域**に、
 * 色のピクセル情報（m_filmBuffer）を転送します。
 *
 * ### 🎨 フィルムテクスチャって何？
 * - レイトレーシングで計算された画像（`m_filmBuffer`）を画面に映すために必要な、
 *   **GPU上の画像メモリ**です。
 * - このID（`m_filmTexture`）は「どのテクスチャか」を指定するための**番号ラベル**のようなものです。
 * - 正確には、色データが直接 int 型の m_filmTexture に送り込まれるわけではありません。
 * m_filmTexture はテクスチャの「ID」や「名前」であり、そのIDが指し示すOpenGLのテクスチャオブジェクト（GPU上のメモリ領域）に色データが送り込まれます。
 *
 * ### 🔁 どうやって使われるの？
 * 1. `glGenTextures()` で新しいテクスチャID（番号）を作成し、`m_filmTexture` に格納します。
 * 2. `glBindTexture()` でそのIDを「今から操作するテクスチャ」として選択します。
 * 3. `glTexImage2D()` や `glTexSubImage2D()` を使って、`m_filmBuffer` にあるピクセルの色データをそのテクスチャに転送します。
 * 4. `paintGL()` の中でこのテクスチャを画面いっぱいに描画します（`drawFilm()` 関数などを通じて）。
 *
 * * @code
 * glGenTextures(1, &m_filmTexture);              // テクスチャIDの生成
 * glBindTexture(GL_TEXTURE_2D, m_filmTexture);   // このIDのテクスチャを選択
 * glTexSubImage2D(..., m_filmBuffer);            // 色データをGPUに送る
 * @endcode
 *
 * ### 🧠 よくある誤解
 * - `m_filmTexture` は `GLuint` 型（ただの数値）で、**データ本体ではありません**。
 * - データは `glTexSubImage2D()` などを使って、このIDが指すGPUメモリに転送されます。
 *
 * ### ✨ 例えると...
 * - `m_filmBuffer` は **絵の具で描いた絵**、
 * - `m_filmTexture` は **その絵を貼るためのスクリーン番号**、
 * - OpenGLの関数は **そのスクリーンに絵を貼って、画面に表示してくれるスタッフ** のようなものです。
 *

 *
 * @see FilmBuffer::getFilmBufferPtr()
 * @see drawFilm()
 * @see glBindTexture()
 * @see glTexSubImage2D()
 */
    GLuint m_filmTexture = 0; // フィルムテクスチャのID

    FilmBuffer m_film;

    void createDebugPattern();
    std::vector<AreaLight> g_AreaLights;// 読み込んだオブジェクトを保持
    Object g_Obj;// シーンの光源を保持

    int m_targetSamplesPerPixel; // 目標とする1ピクセルあたりのサンプル数
    int m_currentSampleCount;    // 現在の蓄積サンプル数


    //デバッグ用
    Eigen::Vector3d debug_computeNormalColor(const Ray& ray);
    //------------------------------------------------------------

    //レイトレ用スロット
private slots:
    void updateRayTracing(); // タイマーで呼び出すカスタムスロット


};

#endif // MYOPENGLWIDGET_CAMERA_H
