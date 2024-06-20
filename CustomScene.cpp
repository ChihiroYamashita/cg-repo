/*-------------タイムラインに関する描画---------------------*/
#include "CustomScene.h"
#include "mainwindow.h"
#include "qdebug.h"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QFont>
#include <Eigen/Geometry> // Eigenライブラリを使用



CustomScene::CustomScene(MainWindow *mainWindow,QObject *parent) : QGraphicsScene(parent), playhead(nullptr), m_mainWindow(mainWindow)
{
}

void  CustomScene::setupScene(int width, int height)
{
//qDebug() << " height:" <<  height;

    //pixelsPerFrame;30フレームのピクセル数
    pixelsPerFrame=width/10;// 例えば10秒のタイムラインを表示すると仮定


    int keyframenumber = (playheadPositionX - startpixel) / (pixelsPerFrame / 30);
        qDebug() << "playheadPositionX:" << playheadPositionX << "startpixel:" << startpixel<< "pixelsPerFrame:" << pixelsPerFrame;


//qDebug() << " pixelsPerFrameinitalixe" <<  pixelsPerFrame;

clear(); // シーンのアイテムをクリア
qDebug() << "CustomScene_func setupScene's width:" <<  width;
qDebug() << " setupScene's height:" <<  height;
// シーンのサイズを設定
this->setSceneRect(0, 0, width, height); // 例えば1000x120のサイズ



QRectF rect = this->sceneRect();
//qDebug() << "CustomScene_func CustomScene Size: Width =" << rect.width() << ", Height =" << rect.height();;



/*-----プレイヘッド関連-----*/
//プレイヘッドの色
QColor myColor(38,99,161); // 紺色



// テキストアイテムの初期化と追加
QFont playheadFont("Arial Rounded MT Bold", 10); // 初期フォントサイズを10に設定
playheadText = new QGraphicsTextItem(QString::number(keyframenumber));
playheadText->setFont(playheadFont);

// テキストの色を設定
QBrush textBrush(Qt::white); // テキストの色を白に設定
playheadText->setDefaultTextColor(textBrush.color());
playheadText->setZValue(2);

// テキストのバウンディングボックスを取得
QRectF playheadTextRect = playheadText->boundingRect();

// 背景四角形のサイズと位置を調整
QRectF backgroundRect = playheadTextRect;
backgroundRect.adjust(-2, 2, 2, -2); // テキストの周りにマージンを追加

// 角の丸みを持たせた四角形のパスを作成
QPainterPath path;
path.addRoundedRect(backgroundRect,20, 20, Qt::RelativeSize); // 角を丸める



// 背景パスを描画
backgroundItem = new QGraphicsPathItem(path);
backgroundItem->setBrush(QBrush(myColor)); // 背景を青に設定
backgroundItem->setPen(Qt::NoPen); // 枠線なし
backgroundItem->setZValue(1); // テキストより背面に配置



// テキストアイテムのフォントサイズをシーンの高さに応じて調整

qreal maxPlayheadTextHeight = height / 4; // 例として、シーンの高さの20分の1を最大のテキスト高さとする
while (playheadTextRect.height() > maxPlayheadTextHeight && playheadFont.pointSize() > 1) {
    playheadFont.setPointSize(playheadFont.pointSize() - 1); // フォントサイズを1減らす
    playheadText->setFont(playheadFont); // 新しいフォントサイズを適用
    playheadTextRect = playheadText->boundingRect(); // バウンディングボックスを更新
}

// テキストアイテムをシーンに追加する前に背景アイテムを追加
addItem(backgroundItem);
addItem(playheadText);

// テキストアイテムの位置を設定（背景と合わせる）
backgroundItem->setPos(playheadPositionX - playheadTextRect.width() / 2, 0.0); // 適切な位置に配置
playheadText->setPos(playheadPositionX - playheadTextRect.width() / 2, 0.0); // 適切な位置に配置


// プレイヘッドの初期設定
QPen pen(myColor, 3); // 赤色で太さ3のペンを作成
playhead = new QGraphicsLineItem(playheadPositionX, playheadTextRect.height()-2, playheadPositionX, height);
playhead->setPen(pen);
addItem(playhead); // playheadをシーンに追加
playhead->setZValue(1); // テキストより背面に配置


// 水平線の描画
QPen linePen(Qt::black);
linePen.setWidth(2);
addLine(0, playheadTextRect.height(), width, playheadTextRect.height(), linePen); // 水平線

/*---------------フレームの縦線を表示--------------*/
// 縦線を引く
for (int x = startpixel, j=0; x < width; x += pixelsPerFrame,j +=30) {
    addLine(x, playheadTextRect.height(), x, height);
    // 数値を表示するテキストアイテムを作成
    QGraphicsTextItem* textItem = new QGraphicsTextItem(QString::number(j));

    // 最大テキストサイズを計算
    qreal maxheight = height / 4;

    // フォントサイズを動的に決定
    QFont font("Arial Rounded MT Bold", 10); // 仮のフォントサイズで初期化
    textItem->setFont(font);
    QRectF textRect = textItem->boundingRect();
    while (textRect.height() >= maxheight && font.pointSize() > 1) {
        // フォントサイズを小さくして再計算
        font.setPointSize(font.pointSize() - 1);
        textItem->setFont(font);
        textRect = textItem->boundingRect();
    }

    // テキストアイテムを縦線の中央に配置
    textItem->setPos(x - textRect.width() / 2, 0); // X位置を調整して中央に
    // テキストアイテムをシーンに追加
    addItem(textItem);
}
/*-----------------------------*/

/*----------キーフレーム挿入-----------*/











/*-----------------イベントハンドラ-----------------*/
/*
 *
 *
 * QGraphicsLineItem* addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());
 * 引数の説明は以下のとおりです：

x1：直線の開始点のX座標。
y1：直線の開始点のY座標。
x2：直線の終了点のX座標。
y2：直線の終了点のY座標。
pen：直線を描画するために使用するペン。QPenオブジェクトで、色や線のスタイルなどを指定できます。この引数はオプションで、指定しない場合はデフォルトのペンが使用される。
 *
 *
    // 30フレームごと（ここでは例として30ピクセルごと）に縦線を描画
    QPen verticalLinePen(Qt::gray);  // 縦線の色をグレーに設定
    verticalLinePen.setStyle(Qt::DashLine);  // 線のスタイルを点線に設定
    for (qreal x = 0; x <= timelineLength; x += 30) {
        addLine(x, timelineY - 10, x, timelineY + 10, verticalLinePen);
    }

    // キーフレームマーカーを追加
    QBrush markerBrush(QColor("gold")); // マーカーの色を設定
    qreal markerSize = 10; // マーカーのサイズ
    // キーフレームマーカーの例（100ピクセルごとに配置）
    for (qreal x = 0; x <= timelineLength; x += 100) {
        addEllipse(x - markerSize / 2, timelineY - markerSize / 2, markerSize, markerSize, linePen, markerBrush);
    }*/



    // 他にもアイテムを追加するコードなど
}

qreal CustomScene::getPlayheadPositionX() const {
return playheadPositionX;
}

/*キーフレーム挿入*/
void CustomScene::addKeyframe(int frameNumber, const QVector3D& eyePoint, const QVector3D& lookAtPoint, const QVector3D& upVector, const QVector3D& xVector, const QVector3D& yVector, const QVector3D& zVector, float fov, double zoom)  {

QPolygonF diamond;
qreal x = currentMousePositionX();
qreal size = 10.0;
// 縦線の中央のY座標を計算
qreal verticalCenterY = ( height()) / 2;

diamond << QPointF(x, verticalCenterY - size)
        << QPointF(x + size, verticalCenterY)
        << QPointF(x, verticalCenterY + size)
        << QPointF(x - size, verticalCenterY);

// 新しいQGraphicsPolygonItemオブジェクト(キーフレームアイコン)を生成
QGraphicsPolygonItem* keyframeItem  = new QGraphicsPolygonItem(diamond);
keyframeItem->setBrush(QColor(224,224,224));
keyframeItem->setPen(QPen(Qt::black));
keyframeItem->setZValue(playhead->zValue() + 1); // プレイヘッドのZ値より大きく設定

// シーンにキーフレームアイコンを追加
addItem(keyframeItem);

// キーフレームアイコンをリストに追加
keyframeItems.append(keyframeItem);

int newFrameNumber = static_cast<int>(m_currentMouseX ); // 新しいフレーム数を計算

frameNumber = static_cast<int>((newFrameNumber - startpixel) / (pixelsPerFrame / 30));



CameraKeyframe keyframe = { frameNumber, eyePoint, lookAtPoint, upVector, xVector, yVector, zVector, fov, zoom };// カメラ構造体のインスタンスを作成
keyframes.append(keyframe);// カメラ構造体のインスタンスを カメラ構造体のリストに追加

qDebug() << "Keyframe" << frameNumber;
qDebug() << "Keyframe added at frame:" <<frameNumber << " eyePoint:" << eyePoint << "lookAtPoint:" << lookAtPoint;

//qDebug() << "playheadBeingDragged=" << playheadPositionX;

// キーフレーム間に線を引く

if (keyframeItems.size() > 1) {
    QGraphicsPolygonItem* previousKeyframe = keyframeItems.at(keyframeItems.size() - 2);
    qreal previousX = previousKeyframe->boundingRect().center().x();
    qreal previousY = previousKeyframe->boundingRect().center().y();

    QGraphicsLineItem* line = new QGraphicsLineItem(previousX, previousY, x, verticalCenterY);
    line->setPen(QPen(QColor(133,134,157),10));
    line->setZValue(playhead->zValue());

    addItem(line);
    keyframeLines.append(qMakePair(keyframeItem, line));
}



}

qreal CustomScene::currentMousePositionX() const {
return m_currentMouseX;  // 現在のマウス X 座標を返す
}

/*==============線形補間=========================================================*/


QQuaternion CustomScene::slerp(const QQuaternion& q1, const QQuaternion& q2, float t) {
return q1.slerp(q1, q2, t);
}


CameraKeyframe CustomScene::interpolateKeyframe(const CameraKeyframe& kf1, const CameraKeyframe& kf2, float t) const {
CameraKeyframe result;

result.eyePoint = kf1.eyePoint * (1 - t) + kf2.eyePoint * t;
result.lookAtPoint = kf1.lookAtPoint * (1 - t) + kf2.lookAtPoint * t;
//result.upVector = kf1.upVector * (1 - t) + kf2.upVector * t;
//result.xVector = kf1.xVector * (1 - t) + kf2.xVector * t;
//result.yVector = kf1.yVector * (1 - t) + kf2.yVector * t;
//result.zVector = kf1.zVector * (1 - t) + kf2.zVector * t;
//result.fov = kf1.fov * (1 - t) + kf2.fov * t;
//result.zoom = kf1.zoom * (1 - t) + kf2.zoom * t;

return result;
}

void CustomScene::updateCameraForFrame(int frameNumber) {
if (keyframes.size() < 2) return; // キーフレームが2つ以上ない場合は補間しない

//qDebug() << "キーフレーム数" << keyframes.size();

CameraKeyframe kf1, kf2;
bool found = false;

// キーフレームを探索
for (int i = 0; i < keyframes.size() - 1; ++i) {
    if (keyframes[i].frameNumber <= frameNumber && frameNumber <= keyframes[i + 1].frameNumber) {
        kf1 = keyframes[i];
        kf2 = keyframes[i + 1];
        found = true;
        break;
    }
}

if (!found) return; // 対応するキーフレームが見つからない場合は補間しない

// 補間パラメータ t の計算
float t = static_cast<float>(frameNumber - kf1.frameNumber) / (kf2.frameNumber - kf1.frameNumber);

// 補間されたキーフレームを取得
CameraKeyframe interpolatedKeyframe = interpolateKeyframe(kf1, kf2, t);

// メインウィンドウの関数を呼び出してカメラの状態を更新
m_mainWindow->updateCamera(interpolatedKeyframe.eyePoint, interpolatedKeyframe.lookAtPoint, interpolatedKeyframe.upVector, interpolatedKeyframe.fov, interpolatedKeyframe.zoom);
}


/*----------------------キーフレームと周りのデータを削除するメソッド---------------------*/
void CustomScene::deleteSelectedKeyframe(int frameNumber) {
// 選択されたキーフレームアイコンがあるかどうかを確認
if (selectedKeyframe != nullptr) {
    // シーンから選択されたキーフレームアイコンを削除
    removeItem(selectedKeyframe);
    // キーフレームアイテムリストから削除
    keyframeItems.removeOne(selectedKeyframe);

    // デバッグ出力: 削除前のキーフレーム数
    qDebug() << "削除前のキーフレーム数:" << keyframes.size();
    qDebug() << "削除対象のフレーム番号:" << frameNumber;
    for (const CameraKeyframe& kf : keyframes) {
        qDebug() << "Frame:" << kf.frameNumber << "Eye:" << kf.eyePoint << "LookAt:" << kf.lookAtPoint;
    }

    // 選択されたキーフレームに結びついていたカメラデータを削除
    bool removed = false;
    for (int i = 0; i < keyframes.size(); ++i) {
        if (keyframes[i].frameNumber == frameNumber) {
            qDebug() << "削除するキーフレーム: Frame:" << keyframes[i].frameNumber;
                                                                        keyframes.removeAt(i);
            removed = true;
            break;
        }
    }

    // デバッグ出力: 削除後のキーフレーム数
    qDebug() << "削除後のキーフレーム数:" << keyframes.size();
    qDebug() << "削除されたか:" << removed;
        for (const CameraKeyframe& kf : keyframes) {
        qDebug() << "Frame:" << kf.frameNumber << "Eye:" << kf.eyePoint << "LookAt:" << kf.lookAtPoint;
    }

    // 関連する線を削除
    for (int i = keyframeLines.size() - 1; i >= 0; --i) {
        QPair<QGraphicsPolygonItem*, QGraphicsLineItem*> pair = keyframeLines.at(i);
        if (pair.first == selectedKeyframe || pair.first->contains(selectedKeyframe->boundingRect().center())) {
            removeItem(pair.second);
            keyframeLines.removeAt(i);
        }
    }

    // 他のキーフレームが残っている場合は、前後のキーフレームを再接続
    int index = keyframeItems.indexOf(selectedKeyframe);
    if (index > 0 && index < keyframeItems.size() - 1) {
        QGraphicsPolygonItem* prevKeyframe = keyframeItems.at(index - 1);
        QGraphicsPolygonItem* nextKeyframe = keyframeItems.at(index + 1);

        qreal prevX = prevKeyframe->boundingRect().center().x();
        qreal prevY = prevKeyframe->boundingRect().center().y();
        qreal nextX = nextKeyframe->boundingRect().center().x();
        qreal nextY = nextKeyframe->boundingRect().center().y();

        QGraphicsLineItem* newLine = new QGraphicsLineItem(prevX, prevY, nextX, nextY);
        newLine->setPen(QPen(Qt::black));
        newLine->setZValue(playhead->zValue());

        addItem(newLine);
        keyframeLines.append(qMakePair(nextKeyframe, newLine));
    }


    // 選択されたキーフレームをリセット
    selectedKeyframe = nullptr;

    // シーンを再描画
    update();
}
}

/*===============================キーフレーム数をmainwindowに返すメソッド=======================================*/

int CustomScene::calculateFrameNumberFromPlayhead() const {
//qDebug() << "pixelsPerFramereturn =" << pixelsPerFrame ;

//qDebug() << "calculateFrameNumberFromPlayhead() playheadPositionX:" << playheadPositionX << "startpixel:" << startpixel<< "pixelsPerFrame:" << pixelsPerFrame;
//qDebug() << "mainwindow前で計算されたフレームナンバー:" << static_cast<int>((playheadPositionX - startpixel) / (pixelsPerFrame / 30));
//return static_cast<int>((playheadPositionX - startpixel) / (pixelsPerFrame / 30));
return (playheadPositionX - startpixel) / (pixelsPerFrame / 30);
qDebug() <<"calculateFrameNumberFromPlayhead(pixelsPerFrame:" << pixelsPerFrame;

}



/*======================================================================*/

/*============================イベントハンドラ==========================================*/

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
qreal mouseX = event->scenePos().x();
qreal playheadX = playhead->line().x1(); // playheadのX座標


/*----------------キーフレームアイコンの選択,選択解除----------------*/

//今マウスでクリックされたところがキーフレームアイコンかどうかを記録するための変数
//bool clickedOnKeyframe = false;

// 既に選択されているキーフレームアイテムがあれば、その選択を解除


if (selectedKeyframe) {
    //if (selectedKeyframe)：もし selectedKeyframe 変数が nullptr でない場合（すなわち、現在選択されているキーフレームがある場合）
    selectedKeyframe->setBrush(QColor(224,224,224));//アイコンの白色を戻す
    selectedKeyframe = nullptr;//selectedKeyframeポインタを何も差さないように戻す（選択されているキーフレームアイテムをリセット）
}

// キーフレームアイテムをチェック
for (QGraphicsPolygonItem* keyframeItem : keyframeItems) { //リスト keyframeItems のすべてのキーフレームアイコンを調べる

    if (keyframeItem->contains(event->scenePos())) {//キーフレームアイコンの位置（ひし形の当たり判定の中に）にクリックされた位置が入っているか
        keyframeItem->setBrush(QColor(255,183,82)); // 色を赤に変更
        selectedKeyframe = keyframeItem;//クリックされたキーフレームアイテムを selectedKeyframe に設定
        //clickedOnKeyframe = true;//キーフレームアイテムがクリックされたことを記録
        break;
    }
}



// マウス位置とplayheadの距離を計算
qreal distance = qAbs(mouseX - playheadX);

// 距離が閾値以内なら、ドラッグ開始と見なす(当たり判定)
if (distance <= 10.0) {
    playheadBeingDragged = true;
    qDebug() << "playheadBeingDragged=" << playheadBeingDragged;
}
}



void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
if (playheadBeingDragged) {
    // シーンの幅を取得 この幅でプレイヘッドが移動可能な範囲の上限を定義
    qreal sceneWidth = this->width();

    // マウスの現在位置を取得し、プレイヘッドはユーザーがマウスでドラッグする範囲内でのみ移動する
    m_currentMouseX = event->scenePos().x();
    m_currentMouseX = qMax(static_cast<qreal>(startpixel), qMin(m_currentMouseX, sceneWidth));
    //qMin(m_currentMouseX, sceneWidth) は、m_currentMouseX と sceneWidth のうち小さい方の値を返す



    // playheadの位置を更新
    playhead->setLine(m_currentMouseX, playhead->line().y1(), m_currentMouseX, playhead->line().y2());

    // playheadTextの位置と内容を更新
    int newFrameNumber = static_cast<int>(m_currentMouseX ); // 新しいフレーム数を計算
    int frameNumber = calculateFrameNumberFromPlayhead();
    playheadText->setPlainText(QString::number(frameNumber)); // テキスト内容を更新


    // テキストバウンディングボックス(playheadTextの後ろ)を更新
    QRectF textRect = playheadText->boundingRect();
    // 背景の位置とサイズをテキストに合わせて更新
    QPainterPath path;
    QRectF backgroundRect = textRect.adjusted(-2, 2, 2, -2); // マージンを追加
    path.addRoundedRect(backgroundRect, 5, 5);
    backgroundItem->setPath(path); // 背景アイテムの形状を更新
    backgroundItem->setPos(m_currentMouseX - textRect.width() / 2, playheadText->pos().y());

    playheadText->setPos(QPointF(m_currentMouseX - playheadText->boundingRect().width() / 2, playheadText->pos().y())); // テキスト位置を更新
    // playheadのX位置を更新
    playheadPositionX = m_currentMouseX;

    //qDebug() << "playheadBeingDragged=" <<  m_currentMouseX;

    // カメラの更新
    updateCameraForFrame(frameNumber);

}
}

void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
playheadBeingDragged = false;
//qDebug() << "playheadBeingDragged =" << playheadBeingDragged;
}


bool CustomScene::eventFilter(QObject *watched, QEvent *event) {
if (event->type() == QEvent::Wheel) {
    QGraphicsSceneWheelEvent *wheelEvent = static_cast<QGraphicsSceneWheelEvent *>(event);
    this->wheelEvent(wheelEvent);
    return true;
}
return QGraphicsScene::eventFilter(watched, event);
}



void CustomScene::wheelEvent(QGraphicsSceneWheelEvent *event) {
int delta = event->delta(); // ホイールの回転量を取得

qreal moveAmount = delta / 120.0 * 10;  // 移動量を調整

// playheadのX座標を計算し直す
qreal newPlayheadPositionX = playheadPositionX + moveAmount;

// playheadの新しい位置がシーンの幅内に収まるように制限
newPlayheadPositionX = qMax(static_cast<qreal>(startpixel), qMin(newPlayheadPositionX, this->sceneRect().width()));

// playheadの位置を更新
playhead->setLine(newPlayheadPositionX, playhead->line().y1(), newPlayheadPositionX, playhead->line().y2());

// playheadTextの位置と内容を更新
int newFrameNumber = static_cast<int>(newPlayheadPositionX); // 新しいフレーム数を計算
int frameNumber = static_cast<int>((newFrameNumber - startpixel) / (this->width() / 300));
playheadText->setPlainText(QString::number(frameNumber)); // テキスト内容を更新

// テキストバウンディングボックス(playheadTextの後ろ)を更新
QRectF textRect = playheadText->boundingRect();
// 背景の位置とサイズをテキストに合わせて更新
QPainterPath path;
QRectF backgroundRect = textRect.adjusted(-2, 2, 2, -2); // マージンを追加
path.addRoundedRect(backgroundRect, 5, 5);
backgroundItem->setPath(path); // 背景アイテムの形状を更新
backgroundItem->setPos(newPlayheadPositionX - textRect.width() / 2, playheadText->pos().y());

playheadText->setPos(QPointF(newPlayheadPositionX - playheadText->boundingRect().width() / 2, playheadText->pos().y())); // テキスト位置を更新

// playheadのX位置を更新
playheadPositionX = newPlayheadPositionX;

qDebug() << "playheadPositionXDragged=" << playheadPositionX;
// 線形補間のカメラ適応
updateCameraForFrame(frameNumber);

event->accept();
QGraphicsScene::wheelEvent(event); // 必要に応じて基底クラスのイベントハンドラを呼び出す
}



