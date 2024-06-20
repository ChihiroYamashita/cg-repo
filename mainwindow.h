#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>
#include "CustomScene.h"
#include <QScrollBar>
//#include "openglwidget_camera.h"

/**--------QT_BEGIN_NAMESPACE と QT_END_NAMESPACE マクロを使用して、Qt の名前空間の中に Ui::MainWindow クラスを定義--------*/
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
/**--------QT_BEGIN_NAMESPACE と QT_END_NAMESPACE マクロを使用して、Qt の名前空間の中に Ui::MainWindow クラスを定義--------*/


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //線形補間でカメラワークを更新する
    void updateCamera(const QVector3D& eyePoint, const QVector3D& lookAtPoint, const QVector3D& upVector, float fov, double zoom);
private slots:

    void on_fovSlider_valueChanged(int value);
    void on_OrthoButton_clicked();// 平行投影モードのボタンがONになったときの処理を記述する関数（スロット）（ボタンの色が変わる）
    void on_cameraButtondefalt_clicked();
    void on_cameraButtonup_clicked();
    void on_cameraButtonfront_clicked();
    void on_cameraButtonright_clicked();
    void on_cameraButtonleft_clicked();
    void on_ZoomSlider_ValueChanged(int value);
    void on_keyframeCameraButton_clicked();//キーフレーム登録
    void on_keyframeCameraDeleteButton_clicked();//キーフレーム削除
private:
    Ui::MainWindow *ui;
    /*--------タイムライン関連の変数--------*/
    CustomScene *scene; // CustomSceneのインスタンスを保持するメンバ変数
    QGraphicsLineItem *playhead;// プレイヘッドのためのメンバ変数
    void setupScene(); // シーンのセットアップを行うメソッド
    // シーンの規定の横幅
    const int fixedWidth = 2000;



protected:

    void resizeEvent(QResizeEvent *event) override;

    void showEvent(QShowEvent *event) override;

};
#endif // MAINWINDOW_H
