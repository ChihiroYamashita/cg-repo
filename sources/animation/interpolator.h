#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "CameraKeyframe.h" // CameraKeyframe 構造体を使用するために必要
#include "camera.h"
#include <Eigen/Dense>
#include <Eigen/Geometry> // EigenのQuaternionを使うために必要
class Interpolator
{
public:
    Interpolator();
    //QQuaternion slerp(const QQuaternion& q1, const QQuaternion& q2, float t) const;
    CameraKeyframe interpolateKeyframe(const CameraKeyframe& kf1, const CameraKeyframe& kf2, float t) const;

    // 回転ベクトルから回転クォータニオンを生成する関数
    //QQuaternion rotationVectorToQuaternion(const QVector3D& rotationVector) const;




};

#endif // INTERPOLATOR_H

