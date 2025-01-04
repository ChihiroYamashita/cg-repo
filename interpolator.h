/**
 * @file interpolator.h
 * @brief 補間クラス
 *
 * @details
 * Interpolatorクラスは、カメラのキーフレーム間で線形補間や球面線形補間（SLERP）を行うための機能を提供します。
 * スムーズなカメラアニメーションを実現できます。
 */
#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QVector3D>
#include <QQuaternion>
#include "CameraKeyframe.h" // CameraKeyframe 構造体を使用するために必要

class Interpolator
{
public:
    Interpolator();
    QQuaternion slerp(const QQuaternion& q1, const QQuaternion& q2, float t) const;
    CameraKeyframe interpolateKeyframe(const CameraKeyframe& kf1, const CameraKeyframe& kf2, float t) const;
};

#endif // INTERPOLATOR_H

