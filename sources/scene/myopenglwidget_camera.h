#ifndef OPENGLWIDGET_CAMERA_H
#define OPENGLWIDGET_CAMERA_H
#include <QOpenGLWidget>
#include <QObject>
#include "myopenglwidget.h" // MyOpenGLWidget クラスをインクルード
#include <QOpenGLFunctions>
#include "TriMesh.h"
#include "Light.h"
class MyOpenGLWidget_camera :public MyOpenGLWidget {
    Q_OBJECT

public:
   explicit MyOpenGLWidget_camera(QWidget* parent = nullptr);
    void setCamerakeyframe(const Eigen::Vector3d& eyePoint,const Eigen::Vector3d& lookAtPoint);//カメラ（画面2用）
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
    void initAreaLights();
private:
    Camera g_Camera2;
    void drawCube();
    GLdouble vertex[8][3];
    int face[6][4];
    GLdouble color[6][3];
    float updatedFov;

    //レイトレ用
     void initializeFilmTexture();
     GLuint m_filmTexture = 0; // フィルムテクスチャのID
     float* m_filmBuffer = nullptr; // テスト用のテクスチャデータ


     std::vector<AreaLight> g_AreaLights;// 読み込んだオブジェクトを保持
     Object g_Obj;// シーンの光源を保持

};

#endif // MYOPENGLWIDGET_CAMERA_H
