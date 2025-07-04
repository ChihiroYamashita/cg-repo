#ifndef KEYFRAMEMANAGER_H
#define KEYFRAMEMANAGER_H

#include <QList>
#include <QVector3D>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include "CameraKeyframe.h"
#include "Interpolator.h"
#include <Eigen/Dense>

class KeyframeManager {
public:
    KeyframeManager();
    void addKeyframe(int frameNumber, const Eigen::Vector3d& eyePoint, const Eigen::Vector3d& lookAtPoint, const Eigen::Vector3d& upVector, const Eigen::Vector3d& xVector, const Eigen::Vector3d& yVector, const Eigen::Vector3d& zVector, float fov, double zoom, QGraphicsScene* scene);
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

