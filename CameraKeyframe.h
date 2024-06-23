#ifndef CAMERAC_KEYFRAME_H
#define CAMERAC_KEYFRAME_H

#include <QVector3D>
#include <QQuaternion>

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

#endif // CAMERAC_KEYFRAME_H

