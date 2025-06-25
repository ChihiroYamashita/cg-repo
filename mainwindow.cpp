#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myopenglwidget.h"
#include <QOpenGLWidget>
#include <QResizeEvent> // QResizeEventのために追加
#include <QApplication>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <Eigen/Dense>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPushButton>
#include <QtSvg/QSvgRenderer>
#include <QGraphicsLineItem>
#include <Eigen/Dense>
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#define _USE_MATH_DEFINES
#include <math.h>

#include "Camera.h"
#include "CustomScene.h"

#include "myopenglwidget_camera.h"



//MainWindowのコンストラクタ
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    scene(new CustomScene(this))// CustomSceneのインスタンスを初期化

{
    ui->setupUi(this);
    CustomScene *scene = new CustomScene(this); // メインウィンドウのインスタンスを渡す

    // 視野角スライダーのシグナルをスロットに接続
    connect(ui->fovSlider, SIGNAL(valueChanged(int)), this, SLOT(on_fovSlider_valueChanged(int)));
    connect(ui->ZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(on_ZoomSlider_ValueChanged(int)));


    //MyOpenGLWidget クラスから MyOpenGLWidget_camera クラスに cameraFov の値の更新を通知する
    connect(ui->openGLWidget_instance, &MyOpenGLWidget::cameraFovChanged,
            ui->openGLWidget_camera_instance, &MyOpenGLWidget_camera::onCameraFovChanged);




    //平行モードボタンの初期設定
     ui->OrthoButton->setText("平行投影モード[OFF]");



    /*-------------タイムラインに関する記述--------------*/

/*
     // playheadの初期化時にペンを設定して色を変更
     QPen pen(Qt::red); // 赤色のペンを作成
     playhead = new QGraphicsLineItem(10, 0, 10, 100);
     playhead->setPen(pen); // ペン（色）を設定*/


     scene->setupScene(1262, 122); // シーンのセットアップを実行
     ui->graphicsView->setScene(scene); // QGraphicsViewにシーンを設定

     /*-------------ー-----------------------------*/

}

MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
}

/*------------------スロット/シグナル---------------------*/


/**
 * @brief FOVを変更するスロット
 * ユーザーがスライダーを操作してカメラの視野角を変更します。
 *
 * シーケンス図:
 *
 * @startuml
 * User -> MainWindow : on_fovSlider_valueChanged(int value)\n FOV sliderをいじる
 * MainWindow -> MyOpenGLWidget : setCameraFov(float fov)\n カメラFOVを更新
 * MyOpenGLWidget -> Camera : setFov(float cameraFov)\n 新しい FOVを設置
 * Camera --> MyOpenGLWidget : getFov(float cameraFov)\n FOVを更新
 * @enduml
 *
 */
void MainWindow::on_fovSlider_valueChanged(int value)
{
    // ここでカメラの視野角を更新
    float fov = static_cast<float>(value);
    // qDebug() << "fov:" << fov;
    ui->openGLWidget_instance->setCameraFov(fov);
    ui->openGLWidget_instance->update(); // ウィジェットを再描画して変更を反映
}


//Auto-Connection(特定の命名規則に従って名付けられたスロット関数は、対応するウィジェットのシグナルに自動的に接続される特性)を利用
void MainWindow::on_OrthoButton_clicked()
{
    bool mode = !ui->openGLWidget_instance->getOrthoMode(); // 現在のモードを取得し、反転させる
    qDebug() << "orthomode:" << mode;
    ui->openGLWidget_instance->setOrthoMode(mode);
    if (mode) {
        // ONの時の処理
        ui->OrthoButton->setText("平行投影モード[ON]");
        ui->OrthoButton->setStyleSheet("background-color: green; color: white;");
    } else {
        // OFFの時の処理
        ui->OrthoButton->setText("平行投影モード[OFF]");
        ui->OrthoButton->setStyleSheet("");
    }
}

void MainWindow::on_cameraButtondefalt_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(Eigen::Vector3d(1.0, 1.0, 3.0));
    ui->openGLWidget_instance->setlookAtPoint(Eigen::Vector3d(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonup_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(Eigen::Vector3d(0.00, 3.0, -0.001));
    ui->openGLWidget_instance->setlookAtPoint(Eigen::Vector3d(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonfront_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(Eigen::Vector3d(0.0, 0.0, 3.0));
    ui->openGLWidget_instance->setlookAtPoint(Eigen::Vector3d(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonright_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(Eigen::Vector3d(2.0, 0.0, 0.0));
    ui->openGLWidget_instance->setlookAtPoint(Eigen::Vector3d(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonleft_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(Eigen::Vector3d(-2.0, 0.0, 0.0));
    ui->openGLWidget_instance->setlookAtPoint(Eigen::Vector3d(0.0, 0.0, 0.0));

}

// onZoomSliderValueChanged スロット
void MainWindow::on_ZoomSlider_ValueChanged(int value)
{
    qreal scale = value / 10.0;
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(scale, 1);



    //qDebug() << "QGraphicsView Size:" << viewportSize;
}




//gcamera→gcamera2への視点と注視点の受け渡し


/*==========================keyframe登録ボタン=========================*/
/**
 * @brief カメラのキーフレームを登録するスロット関数
 * @details 現在のカメラの視点情報（EyePoint、LookAtPoint、UpVectorなど）を取得し、
 * タイムライン上の現在位置にカメラのキーフレームとして登録します。
 * また、`MyOpenGLWidget_camera` にもカメラ情報を転送してビューを更新します。
 *
 * @param なし（ボタンがクリックされた際に自動呼び出しされるスロット）
 * @return なし
 *
 * ### 処理の流れ
 *
 * #### 1. カメラ情報の取得
 * メインビューのOpenGLウィジェット（`MyOpenGLWidget`）から以下のカメラ情報を取得：
 * - EyePoint（視点）
 * - LookAtPoint（注視点）
 * - Up/X/Y/Z ベクトル
 * - FOV（視野角）
 * - Zoom（ズーム値）
 *
 * #### 2. フレーム番号の計算
 * `CustomScene::getPlayheadPositionX()` から現在のプレイヘッドX座標を取得し、タイムライン上のフレーム番号に変換します。
 *
 * #### 3. キーフレームの登録
 * `CustomScene::addKeyframe()` を呼び出して、取得したカメラ情報を登録。
 *
 * #### 4. カメラビューの同期
 * `MyOpenGLWidget_camera` の `setCamerakeyframe()` を呼び出し、視点と注視点をカメラビューに反映します。
 *
 * ### シーケンス図
 * @startuml
 * User -> MainWindow : on_keyframeCameraButton_clicked()
 * MainWindow -> MyOpenGLWidget : getEyePoint(), getlookAtPoint(), getUpVector(), getFov(), getZoom()
 * MainWindow -> CustomScene : getPlayheadPositionX()
 * MainWindow -> CustomScene : addKeyframe()
 * MainWindow -> MyOpenGLWidget_camera : setCamerakeyframe()
 * @enduml
 *
 * @see CustomScene::addKeyframe
 * @see MyOpenGLWidget::getEyePoint
 * @see MyOpenGLWidget::getlookAtPoint
 * @see MyOpenGLWidget_camera::setCamerakeyframe
 */
void MainWindow::on_keyframeCameraButton_clicked()
{

// 1. メインカメラビュー(openGLWidget_instance)からカメラ情報を取得
    Eigen::Vector3d EyePoint=(ui->openGLWidget_instance->getEyePoint());
    Eigen::Vector3d lookAtPoint=(ui->openGLWidget_instance->getlookAtPoint());

    Eigen::Vector3d upVector = ui->openGLWidget_instance->getUpVector();
    Eigen::Vector3d xVector = ui->openGLWidget_instance->getXVector();
    Eigen::Vector3d yVector = ui->openGLWidget_instance->getYVector();
    Eigen::Vector3d zVector =( ui->openGLWidget_instance->getZVector());

    float fov = ui->openGLWidget_instance->getCameraFov();
    double zoom = ui->openGLWidget_instance->getZoom();

    //fov（視野角）や zoom（ズーム倍率）

    //2. 現在のフレーム番号を計算
    int frameNumber = static_cast<int>((scene->getPlayheadPositionX() - scene->startpixel) / scene->pixelsPerFrame);

    //ui->openGLWidget_instance->setCamerakeyframe(); // ウィジェットを再描画して変更を反映
     frameNumber = static_cast<int>((scene->getPlayheadPositionX() - scene->startpixel) / scene->pixelsPerFrame);


    //3.パストレカメラビュー（サブビュー）への反映
    ui->openGLWidget_camera_instance->setCamerakeyframe( EyePoint,lookAtPoint);

    //4.両ビューを再描画
    ui->openGLWidget_instance->update();
    ui->openGLWidget_camera_instance->update();

    // 5. タイムラインにキーフレームを登録
    qreal x = scene->currentMousePositionX();  // 現在のマウスX座標を取得する方法を実装する必要がある

    // キーフレームを追加
    scene->addKeyframe(frameNumber, EyePoint, lookAtPoint, upVector, xVector, yVector, zVector, fov, zoom);
}


/*==========================keyframeボタン=========================*/


void MainWindow::on_keyframeCameraDeleteButton_clicked() {
    int frameNumber = scene->calculateFrameNumberFromPlayhead();
    qDebug() << "mainwindowで計算されたフレームナンバー:" << frameNumber;
    // 選択されたキーフレームを削除
    scene->deleteSelectedKeyframe(frameNumber);
}


/*-------------------------------------------------------*/


/*----------タイムラインに関する記述------------*/

/*---------------------------------------*/

/*--------------イベントハンドラ----------------*/
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    // ビューポートのサイズを取得
    QSize viewportSize = ui->graphicsView->viewport()->size();

    // シーンのサイズをビューポートのサイズに合わせる
    scene->setSceneRect(0, 0, fixedWidth, viewportSize.height());


    // シーンを再セットアップする（必要に応じて）
    scene->setupScene(fixedWidth, viewportSize.height());

    // QGraphicsViewにシーンを再設定
    ui->graphicsView->setScene(scene);

}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    // QGraphicsViewの表示領域のサイズを取得
    QSize viewportSize = ui->graphicsView->viewport()->size();

    // QGraphicsSceneの表示領域をビューポートのサイズに合わせて設定
    scene->setSceneRect(0, 0, fixedWidth, viewportSize.height());

    // setupSceneメソッドを呼び出してシーンを再セットアップする（必要に応じて）
    scene->setupScene(fixedWidth, viewportSize.height());

    // QGraphicsViewにシーンを再設定
    ui->graphicsView->setScene(scene);
    //qDebug() << "QGraphicsView Size:" << ui->graphicsView->size();

    // 水平スクロールバーの位置を左端に設定（0フレームから表示する、左寄せにするための処理）
    ui->graphicsView->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->minimum());
}

/*----------線形補間に関係する関数------------*/

void MainWindow::updateCamera(const Eigen::Vector3d& eyePoint, const Eigen::Vector3d& lookAtPoint, const Eigen::Vector3d& upVector, float fov, double zoom) {
    ui->openGLWidget_camera_instance->setCamerakeyframe( eyePoint,lookAtPoint);

    ui->openGLWidget_camera_instance->update();
}
