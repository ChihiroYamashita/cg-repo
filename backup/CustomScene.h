//タイムラインに関する記述
#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include "qvector3d.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QList>
#include <QPair>

class MainWindow; // 前方宣言

// キーフレーム構造体の定義
struct CameraKeyframe {
    int frameNumber;
    QVector3D eyePoint;
    QVector3D lookAtPoint;
    QVector3D upVector;
    QVector3D xVector;
    QVector3D yVector;
    QVector3D zVector;
    float fov;
    double zoom;
};

class CustomScene : public QGraphicsScene
{
    Q_OBJECT



private:
    QGraphicsLineItem* playhead ;
    QGraphicsPathItem* backgroundItem;
    qreal playheadPositionX = 10.0; // プレイヘッドのX位置を保持する変数
    bool playheadBeingDragged = false; // ドラッグ状態を追跡するための変数
    QGraphicsTextItem* playheadText = nullptr; // playheadのフレーム数を表示するテキスト
     int frameRate = 30; // 1秒あたりのフレーム数

     qreal m_currentMouseX;  // マウスの現在のX座標を保存するメンバ変数



     /*==========キーフレームアイコンに関する実装===========*/

     QGraphicsPolygonItem* keyframeItem;//キーフレームアイコン
     QList<QGraphicsPolygonItem*> keyframeItems; // キーフレームアイコンのリスト
     QList<QPair<QGraphicsPolygonItem*, QGraphicsLineItem*>> keyframeLines; // キーフレームと線をペアで管理するリスト

     /*---------------線形補間---------------*/

     QList<CameraKeyframe> keyframes; // カメラデータ構造体CameraKeyframeのリスト

    //mainwindowからスロットをもらうためのポインタ
     MainWindow *m_mainWindow; // メインウィンドウのポインタ
     //キーフレームアイコンのリスト

     int sceneWidth; // シーンの幅を保存するメンバ変数



public:
    explicit CustomScene(MainWindow *mainWindow,QObject *parent = nullptr);
    void setupScene(int width, int height); // サイズをパラメータとして受け取るように変更  // シーンのセットアップ関数
    void addKeyframe(int frameNumber, const QVector3D& eyePoint, const QVector3D& lookAtPoint, const QVector3D& upVector, const QVector3D& xVector, const QVector3D& yVector, const QVector3D& zVector, float fov, double zoom);

    qreal currentMousePositionX() const;  // マウス位置を取得するメソッド
    qreal getPlayheadPositionX() const;//クラス外にPlayheadPositionX()を教える

    int startpixel=10;//タイムラインの開始位置
    int pixelsPerFrame;

    /*==========線形補間に関する実装===========*/
    //線形補間した値を適応するメソッド
    void updateCameraForFrame(int frameNumber);

    //キーフレムを選択して削除するためのポインタ変数
    QGraphicsPolygonItem* selectedKeyframe = nullptr; // 選択されたキーフレーム

    //キーフレームアイコンとそのデータを削除する変数
    void deleteSelectedKeyframe(int frameNumber);
    //mainwidow等に渡すキーフレーム数
    int calculateFrameNumberFromPlayhead() const;




protected:
   // イベントハンドラ
   bool eventFilter(QObject *watched, QEvent *event) override;
   void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
   void wheelEvent(QGraphicsSceneWheelEvent *event) override;
   void updatePlayheadText(qreal newPos);
   //線形補間関数
   CameraKeyframe interpolateKeyframe(const CameraKeyframe& kf1, const CameraKeyframe& kf2, float t) const;
};

#endif // CUSTOMSCENE_H
