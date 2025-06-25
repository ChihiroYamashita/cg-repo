#ifndef OPENGLWIDGET_CAMERA_H
#define OPENGLWIDGET_CAMERA_H
#include <QOpenGLWidget>
#include <QObject>
#include "myopenglwidget.h" // MyOpenGLWidget クラスをインクルード


class MyOpenGLWidget_camera :public MyOpenGLWidget {
    Q_OBJECT

public:
   explicit MyOpenGLWidget_camera(QWidget* parent = nullptr);
    void setCamerakeyframe(const QVector3D& eyePoint,const QVector3D& lookAtPoint);//カメラ（画面2用）
    void setCameraEyePoint2(const QVector3D& eyePoint);//MyOpenGLWidgetの外からカメラの画角を設定する関数
    void setlookAtPoint2(const QVector3D& lookAtPoint);
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
private:
    Camera g_Camera2;
    void drawCube();
    GLdouble vertex[8][3];
    int face[6][4];
    GLdouble color[6][3];
    float updatedFov;
};

#endif // MYOPENGLWIDGET_CAMERA_H
