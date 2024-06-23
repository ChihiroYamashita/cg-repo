//Interpolator クラス 線形補間の管理を行うクラス

#include "Interpolator.h"
#include "DualQuaternion.h"

Interpolator::Interpolator()
{
}

QQuaternion Interpolator::slerp(const QQuaternion& q1, const QQuaternion& q2, float t) const {
    return q1.slerp(q1, q2, t);
}

// interpolateKeyframe関数　ここですべての補完に関する実装を行う　tで何秒ごとの補完なのかを向こうで読み込むだけ

CameraKeyframe Interpolator::interpolateKeyframe(const CameraKeyframe& kf1, const CameraKeyframe& kf2, float t) const {
    CameraKeyframe result;

    // lookAtPoint の補間＝平行移動の線形補間
    QVector3D lookAtPoint = (1.0f - t) * kf1.lookAtPoint + t * kf2.lookAtPoint;

/*ドゥアルクォータニオン
 * Q=q_1(回転クォータニオン)​+ϵq_2(平行移動クォータニオン）
​
*/
    // ドゥアルクォータニオンを使用して補間
    /*
    DualQuaternion dq1 = DualQuaternion::fromTranslationRotation(kf1.eyePoint, QQuaternion::fromDirection(kf1.lookAtPoint - kf1.eyePoint, kf1.upVector));
    DualQuaternion dq2 = DualQuaternion::fromTranslationRotation(kf2.eyePoint, QQuaternion::fromDirection(kf2.lookAtPoint - kf2.eyePoint, kf2.upVector));
    DualQuaternion dqInterpolated = (dq1 * (1 - t) + dq2 * t).normalized();

    result.eyePoint = dqInterpolated.getTranslation();
    result.lookAtPoint = result.eyePoint + dqInterpolated.getRotation() * QVector3D(0, 0, -1);  // 例として前方向を設定
    result.upVector = dqInterpolated.getRotation() * QVector3D(0, 1, 0);*/  // 例として上方向を設定


    result.eyePoint = kf1.eyePoint * (1 - t) + kf2.eyePoint * t;
    result.lookAtPoint = kf1.lookAtPoint * (1 - t) + kf2.lookAtPoint * t;
    // 他のパラメータも同様に補間する場合はここに追加
    return result;
}
