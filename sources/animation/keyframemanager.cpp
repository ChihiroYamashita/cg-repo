#include "KeyframeManager.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QDebug>

KeyframeManager::KeyframeManager() {
}

void KeyframeManager::addKeyframe(int frameNumber, const QVector3D& eyePoint, const QVector3D& lookAtPoint, const QVector3D& upVector, const QVector3D& xVector, const QVector3D& yVector, const QVector3D& zVector, float fov, double zoom, QGraphicsScene* scene) {
    CameraKeyframe keyframe = { frameNumber, eyePoint, lookAtPoint, upVector, xVector, yVector, zVector, fov, zoom };
    keyframes.append(keyframe);

    qreal x = frameNumber; // フレーム数に基づいたX座標（仮）
    qreal size = 10.0;
    qreal verticalCenterY = scene->height() / 2;

    QPolygonF diamond;
    diamond << QPointF(x, verticalCenterY - size)
            << QPointF(x + size, verticalCenterY)
            << QPointF(x, verticalCenterY + size)
            << QPointF(x - size, verticalCenterY);

    QGraphicsPolygonItem* keyframeItem = new QGraphicsPolygonItem(diamond);
    keyframeItem->setBrush(QColor(224,224,224));
    keyframeItem->setPen(QPen(Qt::black));
    scene->addItem(keyframeItem);
    keyframeItems.append(keyframeItem);

    updateKeyframeLines(scene);
}

void KeyframeManager::deleteKeyframe(int frameNumber, QGraphicsScene* scene) {
    for (int i = 0; i < keyframes.size(); ++i) {
        if (keyframes[i].frameNumber == frameNumber) {
            keyframes.removeAt(i);
            scene->removeItem(keyframeItems[i]);
            delete keyframeItems[i];
            keyframeItems.removeAt(i);
            break;
        }
    }

    updateKeyframeLines(scene);
}

CameraKeyframe KeyframeManager::interpolateKeyframe(int frameNumber) const {
    if (keyframes.size() < 2) return CameraKeyframe();

    CameraKeyframe kf1, kf2;
    bool found = false;

    for (int i = 0; i < keyframes.size() - 1; ++i) {
        if (keyframes[i].frameNumber <= frameNumber && frameNumber <= keyframes[i + 1].frameNumber) {
            kf1 = keyframes[i];
            kf2 = keyframes[i + 1];
            found = true;
            break;
        }
    }

    if (!found) return CameraKeyframe();

    float t = static_cast<float>(frameNumber - kf1.frameNumber) / (kf2.frameNumber - kf1.frameNumber);
    return interpolator.interpolateKeyframe(kf1, kf2, t);
}

int KeyframeManager::calculateFrameNumberFromPosition(qreal positionX, int startPixel, int pixelsPerFrame) const {
    float frameInterval = static_cast<float>(pixelsPerFrame) / 30.0f;
    return static_cast<int>((positionX - startPixel) / frameInterval);
}

void KeyframeManager::updateKeyframeLines(QGraphicsScene* scene) {
    for (const auto& pair : keyframeLines) {
        scene->removeItem(pair.second);
        delete pair.second;
    }
    keyframeLines.clear();

    for (int i = 1; i < keyframeItems.size(); ++i) {
        QGraphicsPolygonItem* prevKeyframe = keyframeItems[i - 1];
        QGraphicsPolygonItem* currKeyframe = keyframeItems[i];

        qreal prevX = prevKeyframe->boundingRect().center().x();
        qreal prevY = prevKeyframe->boundingRect().center().y();
        qreal currX = currKeyframe->boundingRect().center().x();
        qreal currY = currKeyframe->boundingRect().center().y();

        QGraphicsLineItem* line = new QGraphicsLineItem(prevX, prevY, currX, currY);
        line->setPen(QPen(Qt::black));
        scene->addItem(line);
        keyframeLines.append(qMakePair(currKeyframe, line));
    }
}

