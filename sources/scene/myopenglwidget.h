#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include "camera.h"
#include <QOpenGLWidget>
#include <QPushButton>
#include <QDebug>       // QDebugのために追加
#include <QOpenGLFunctions>
#include <QString>
#include "TriMesh.h"
#include "Light.h"
class Camera; // 前方宣言

class MyOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
private:
    Camera g_Camera;
    QPoint lastMousePos; // 最後のマウス位置
    const float desiredAspectRatio = 16.0f / 9.0f;
    QPushButton *button; // ボタンのメンバー変数
    QPushButton *zoombutton; // ボタンのメンバー変数


    //移動ボタン用
    QPoint lastButtonPos;
    bool ButtonPressed = false;

    //ズームボタン用
    QPoint lastZoomButtonPos;
    bool zoomButtonPressed = false;
    QPoint originalMousePos;
    float cameraFov ; // カメラの視野角を保持する変数




     // 平行投影モードの状態を保持する変数

public:
    explicit MyOpenGLWidget(QWidget *parent = 0);

    void setCameraFov(float fov); // 視野角設定用メソッド
    void setOrthoMode(bool mode); // メソッドの宣言
    bool getOrthoMode();
    Eigen::Vector3d getEyePoint() const;
    Eigen::Vector3d getlookAtPoint() const;
    Eigen::Vector3d getUpVector()const;
    Eigen::Vector3d getXVector()const;
    Eigen::Vector3d getYVector()const;
    Eigen::Vector3d getZVector()const;
    double getZoom()const;

    void setCameraEyePoint(const Eigen::Vector3d& eyePoint);//MyOpenGLWidgetの外からカメラの画角を設定する関数
    void setlookAtPoint(const Eigen::Vector3d& lookAtPoint);
    //カメラ（画面2用）



    void checkOpenGLVersion();
    float getWidth() const { return this->width; }
    float getHeight() const { return this->height; }
    float getCameraFov() const;

    //objファイルの名前



signals:
    void cameraFovChanged(float newFov); // 新しいcameraFovの値を通知するシグナル
protected slots:
    void onButtonPressed(QMouseEvent *event);
    void onButtonReleased();
    void onButtonMoved(QMouseEvent *event);
    void onZoomButtonPressed();
    void onZoomButtonReleased();
    void onZoomButtonMoved(QMouseEvent *event);


 protected:

    //Camera g_Camera2; //カメラとして利用
    float width;
    float height;
    double g_FrameSize_WindowSize_Scale_x = 1.0;
    double g_FrameSize_WindowSize_Scale_y = 1.0;
    bool orthoMode;
    //視野角用

    void initializeGL()override;        //  OpenGL 初期化
    virtual void resizeGL(int w, int h)override;    //  ウィジットリサイズ時のハンドラ
    virtual void updateProjectionMatrix();
    void paintGL()override;             //  描画処理
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;    // マウスムーブイベント
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;        // マウスホイールイベント
    bool eventFilter(QObject *obj, QEvent *event) override;
    void projection_and_modelview(const Camera& in_Camera);

    void initializeButtons();

    //レイトレ用
    void MyOpenGLWidget::initAreaLights(std::vector<AreaLight>& lights);
    std::vector<AreaLight> g_AreaLights_preview;// 読み込んだオブジェクトを保持
    Object g_Obj_preview;// シーンの光源を保持

    //Obj名
    const std::string m_objFileName = "box2.obj";












};

#endif // MYOPENGLWIDGET_H
