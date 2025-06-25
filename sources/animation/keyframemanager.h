#ifndef KEYFRAMEMANAGER_H
#define KEYFRAMEMANAGER_H

#include <QList>
#include <QVector3D>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include "CameraKeyframe.h"
#include "Interpolator.h"

class KeyframeManager {
public:
    KeyframeManager();
    void addKeyframe(int frameNumber, const QVector3D& eyePoint, const QVector3D& lookAtPoint, const QVector3D& upVector, const QVector3D& xVector, const QVector3D& yVector, const QVector3D& zVector, float fov, double zoom, QGraphicsScene* scene);
    void deleteKeyframe(int frameNumber, QGraphicsScene* scene);
    CameraKeyframe interpolateKeyframe(int frameNumber) const;
    int calculateFrameNumberFromPosition(qreal positionX, int startPixel, int pixelsPerFrame) const;
    void updateKeyframeLines(QGraphicsScene* scene);

private:
    QList<CameraKeyframe> keyframes;
    QList<QGraphicsPolygonItem*> keyframeItems;
    QList<QPair<QGraphicsPolygonItem*, QGraphicsLineItem*>> keyframeLines;
    Interpolator interpolator;
};

#endif // KEYFRAMEMANAGER_H

