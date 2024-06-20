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

//視野角スライダー

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

    ui->openGLWidget_instance->setCameraEyePoint(QVector3D(1.0, 1.0, 3.0));
    ui->openGLWidget_instance->setlookAtPoint(QVector3D(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonup_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(QVector3D(0.00, 3.0, -0.001));
    ui->openGLWidget_instance->setlookAtPoint(QVector3D(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonfront_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(QVector3D(0.0, 0.0, 3.0));
    ui->openGLWidget_instance->setlookAtPoint(QVector3D(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonright_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(QVector3D(2.0, 0.0, 0.0));
    ui->openGLWidget_instance->setlookAtPoint(QVector3D(0.0, 0.0, 0.0));

}

void MainWindow::on_cameraButtonleft_clicked()
{

    ui->openGLWidget_instance->setCameraEyePoint(QVector3D(-2.0, 0.0, 0.0));
    ui->openGLWidget_instance->setlookAtPoint(QVector3D(0.0, 0.0, 0.0));

}

// onZoomSliderValueChanged スロット
void MainWindow::on_ZoomSlider_ValueChanged(int value)
{
    qreal scale = value / 10.0;
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(scale, 1);



    //qDebug() << "QGraphicsView Size:" << viewportSize;
}







/*==========================keyframe登録ボタン=========================*/
void MainWindow::on_keyframeCameraButton_clicked()
{


    QVector3D EyePoint=(ui->openGLWidget_instance->getEyePoint());
    QVector3D lookAtPoint=(ui->openGLWidget_instance->getlookAtPoint());

    QVector3D upVector = ui->openGLWidget_instance->getUpVector();
    QVector3D xVector = ui->openGLWidget_instance->getXVector();
    QVector3D yVector = ui->openGLWidget_instance->getYVector();
    QVector3D zVector =( ui->openGLWidget_instance->getZVector());
    float fov = ui->openGLWidget_instance->getCameraFov();
    double zoom = ui->openGLWidget_instance->getZoom();
    int frameNumber = static_cast<int>((scene->getPlayheadPositionX() - scene->startpixel) / scene->pixelsPerFrame);

    //ui->openGLWidget_instance->setCamerakeyframe(); // ウィジェットを再描画して変更を反映
     frameNumber = static_cast<int>((scene->getPlayheadPositionX() - scene->startpixel) / scene->pixelsPerFrame);


    ui->openGLWidget_camera_instance->setCamerakeyframe( EyePoint,lookAtPoint);

    ui->openGLWidget_instance->update();
    ui->openGLWidget_camera_instance->update();

    //insert keyframe to customscene
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

void MainWindow::updateCamera(const QVector3D& eyePoint, const QVector3D& lookAtPoint, const QVector3D& upVector, float fov, double zoom) {
    ui->openGLWidget_camera_instance->setCamerakeyframe( eyePoint,lookAtPoint);

    ui->openGLWidget_camera_instance->update();
}
