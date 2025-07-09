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
class MyOpenGLWidget_camera :public MyOpenGLWidget {
    Q_OBJECT

public:
   explicit MyOpenGLWidget_camera(QWidget* parent = nullptr);
    void setCamerakeyframe(const Eigen::Vector3d& eyePoint,const Eigen::Vector3d& lookAtPoint);//カメラ1からのいち情報をもらう関数（画面2用）
    void setCameraEyePoint2(const Eigen::Vector3d& eyePoint);//MyOpenGLWidgetの外からカメラの画角を設定する関数
    void setlookAtPoint2(const Eigen::Vector3d& lookAtPoint);
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
    //レイトレ用Qtimer
     QTimer *m_timer; // タイマーのポインタ


     // ★★★ 追加：レンダリングの進捗と状態を管理する変数 ★★★
     bool m_isDirty;     // 再レンダリングが必要かどうかのフラグ
     int m_progress_i;   // 次に計算するピクセルの横位置(i)
     int m_progress_j;   // 次に計算するピクセルの縦位置(j)

private:
    Camera g_Camera2;
    void drawCube();
    GLdouble vertex[8][3];
    int face[6][4];
    GLdouble color[6][3];
    float updatedFov;

    //--------------レイトレ用---------------------------------------
     void initializeFilmTexture();
     GLuint m_filmTexture = 0; // フィルムテクスチャのID
     float* g_FilmBuffer = nullptr; // テスト用のテクスチャデータ


     std::vector<AreaLight> g_AreaLights;// 読み込んだオブジェクトを保持
     Object g_Obj;// シーンの光源を保持
     FilmBuffer m_film;


     //デバッグ用
     Eigen::Vector3d debug_computeNormalColor(const Ray& ray);
     //------------------------------------------------------------

     //レイトレ用スロット
 private slots:
     void updateRayTracing(); // タイマーで呼び出すカスタムスロット
};

#endif // MYOPENGLWIDGET_CAMERA_H
