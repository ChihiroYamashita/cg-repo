//Interpolator クラス 線形補間の管理を行うクラス

#include "Interpolator.h"
//#include "DualQuaternion.h"
#include "qmath.h"

Interpolator::Interpolator()
{
}
/*
 *
QQuaternion Interpolator::slerp(const QQuaternion& q1, const QQuaternion& q2, float t) const {

    /*球面線形補間
 *2つのクォータニオン（𝑞1と𝑞2）の間を球面上の大円を辿る形で補間
 *数学的背景
    *<１>クォータニオンの内積を計算:
    *  cos(θ)=q1⋅q2
    *
    *<2>補間角度 𝜃を計算:
    *
    *θ=arccos(cos(θ))
    *
    *<3>補間係数を計算:
    *         sin((1−t)θ)            sin(tθ)
    *  q(t)=  ------------ q 1 +  ------------ q 2
    *           sin(θ)                sin(θ)
    *
 *
    //<１>クォータニオンの内積を計算:
    float dotProduct = QVector3D::dotProduct(q1.vector(), q2.vector());
    //<2>補間角度 𝜃を計算:
    float theta = qAcos(dotProduct);

    if (qFuzzyIsNull(theta)) {
        // If the angle is very small, linear interpolation is sufficient
        return q1 * (1.0f - t) + q2 * t;
    }

    //<3>補間係数を計算:
    float sinTheta = qSin(theta);
    float a = qSin((1.0f - t) * theta) / sinTheta;
    float b = qSin(t * theta) / sinTheta;

    return (q1 * a) + (q2 * b);

    //通常は以下の式で容易に実装できるreturn q1.slerp(q1, q2, t);
}
*/

/*
// 回転ベクトルから回転クォータニオンを生成する関数
QQuaternion Interpolator::rotationVectorToQuaternion(const QVector3D &vector) const {
    float angle = vector.length();
    QVector3D axis = vector.normalized();
    return QQuaternion::fromAxisAndAngle(axis, qRadiansToDegrees(angle));
}
*/


// interpolateKeyframe関数　ここですべての補完に関する実装を行う　tで何秒ごとの補完なのかを向こうで読み込むだけ

CameraKeyframe Interpolator::interpolateKeyframe(const CameraKeyframe& kf1, const CameraKeyframe& kf2, float t) const {
    //CameraKeyframe result;

    /*ドゥアルクォータニオン
 * Q=q_1(回転クォータニオン)​+ϵq_2(平行移動クォータニオン）
 * 平行移動部分は線形補間で、回転部分はSLERP（球面線形補間）で補間することで、実質的にデュアルクォータニオンとしての機能を果たすことができる。今回はそのようなソースコードになっている。
 *
 * <q_1(回転クォータニオン)を求める>
     * 回転クォータニオン 𝑞は、回転軸 𝑎 と回転角度 𝜃を使って次のように定義され：
     *
     * 𝑞=cos(𝜃/2)+𝑢sin(𝜃/2)
     * ここで、𝑢=𝑎⋅𝑖+𝑎⋅𝑗+𝑎⋅𝑘
     *
     * つまり、回転軸 𝑎 と回転角度 𝜃が求まればよい
     *
     * 1.回転角度𝜃をもとめる
     * 2.回転軸を求める
        * 回転軸と回転角度の計算
        * eyepoint1 と eyepoint2 が含まれる平面は、lookAtPoint を通る平面（輪切り）。この平面上で eyepoint が lookAtPoint の周りを回転することになります。
        * 視線ベクトルをlookAtVector= yePoint -lookAtPointと定義すると、
        * 視線ベクトル２つの外積を取ることで回転軸を計算できる。
        *
 * <q_1(回転クォータニオン)を補間する>
    * 球面線形補間を使用　詳しくはslerp(const QQuaternion& q1, const QQuaternion& q2, float t)参照
    *

 *<q_2(平行移動クォータニオン）を補間する>
    *線形補間を用いて行う
​
*/

/*

    //<q_1(回転クォータニオン)を求める>
    // 視線ベクトルの計算
    QVector3D lookAtVector1 = kf1.eyePoint - kf1.lookAtPoint;
    QVector3D lookAtVector2 = kf2.eyePoint - kf2.lookAtPoint;

    lookAtVector1=lookAtVector1.normalized();
    lookAtVector2=lookAtVector2.normalized();

    // 回転軸の計算
    QVector3D rotationAxis = QVector3D::crossProduct(lookAtVector1, lookAtVector2).normalized();

    // 回転角度の計算
    float dotProduct = QVector3D::dotProduct(lookAtVector1.normalized(), lookAtVector2.normalized());
    float angle = qAcos(dotProduct);


    QQuaternion quaternion= QQuaternion::fromAxisAndAngle(rotationAxis, qRadiansToDegrees(angle));


    // 回転クォータニオンを用いた球面線形補間（SLERP）補間
    QQuaternion quaternion1 = rotationVectorToQuaternion(lookAtVector1);
    QQuaternion quaternion2 = rotationVectorToQuaternion(lookAtVector2);
    QQuaternion interpolatedQuaternion = QQuaternion::slerp(quaternion1, quaternion2, t);

    //interpolatedQuaternionでt時間の回転クォータニオンが生成された。後はこれをベクトルにかけて回転させてあげる。


    // q_2(平行移動クォータニオン）を補間する
    QVector3D interpolatedEyePoint = kf1.eyePoint * (1 - t) + kf2.eyePoint * t;
    QVector3D interpolatedLookAtPoint = kf1.lookAtPoint * (1 - t) + kf2.lookAtPoint * t;

    // デュアルクォータニオンの計算
    QQuaternion dqReal = interpolatedQuaternion;
    QQuaternion dqDual = QQuaternion(0, interpolatedEyePoint) * interpolatedQuaternion * 0.5;


    // 補間結果を反映
    result.eyePoint = interpolatedEyePoint;
    result.lookAtPoint = interpolatedLookAtPoint;

    // upVector, xVector, yVector, zVector の補間
    QVector3D upVector1 = kf1.upVector.normalized();
    QVector3D upVector2 = kf2.upVector.normalized();
    QVector3D interpolatedUpVector = toQt(rotateVector(toEigen(upVector1), toEigen(interpolatedQuaternion.vector()), interpolatedQuaternion.scalar()));

    QVector3D xVector1 = kf1.xVector.normalized();
    QVector3D xVector2 = kf2.xVector.normalized();
    QVector3D interpolatedXVector = toQt(rotateVector(toEigen(xVector1), toEigen(interpolatedQuaternion.vector()), interpolatedQuaternion.scalar()));

    QVector3D yVector1 = kf1.yVector.normalized();
    QVector3D yVector2 = kf2.yVector.normalized();
    QVector3D interpolatedYVector = toQt(rotateVector(toEigen(yVector1), toEigen(interpolatedQuaternion.vector()), interpolatedQuaternion.scalar()));

    QVector3D zVector1 = kf1.zVector.normalized();
    QVector3D zVector2 = kf2.zVector.normalized();
    QVector3D interpolatedZVector = toQt(rotateVector(toEigen(zVector1), toEigen(interpolatedQuaternion.vector()), interpolatedQuaternion.scalar()));

    result.upVector = interpolatedUpVector;
    result.xVector = interpolatedXVector;
    result.yVector = interpolatedYVector;
    result.zVector = interpolatedZVector;

    // fov と zoom の補間
    result.fov = kf1.fov * (1 - t) + kf2.fov * t;
    result.zoom = kf1.zoom * (1 - t) + kf2.zoom * t;


    return result;*/

    CameraKeyframe result;

    // 平行移動部分の線形補間
    result.eyePoint = kf1.eyePoint * (1.0 - t) + kf2.eyePoint * t;
    result.lookAtPoint = kf1.lookAtPoint * (1.0 - t) + kf2.lookAtPoint * t;

    // 回転部分の球面線形補間 (SLERP)
    // EigenのQuaternionはベクトルから直接初期化できないため、
    // lookAtベクトルから回転を表現するクォータニオンを生成します。
    // ここでは単純にup, x, y, zベクトルを直接SLERPします。
    // DualQuaternionのような複雑な実装は、多くの場合、各基底ベクトルのSLERPで代替できます。

    // 基底ベクトルの正規化
    Eigen::Quaterniond q_x1 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitX(), kf1.xVector.normalized());
    Eigen::Quaterniond q_y1 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitY(), kf1.yVector.normalized());
    Eigen::Quaterniond q_z1 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitZ(), kf1.zVector.normalized());
    Eigen::Quaterniond q_up1 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitY(), kf1.upVector.normalized());

    Eigen::Quaterniond q_x2 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitX(), kf2.xVector.normalized());
    Eigen::Quaterniond q_y2 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitY(), kf2.yVector.normalized());
    Eigen::Quaterniond q_z2 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitZ(), kf2.zVector.normalized());
    Eigen::Quaterniond q_up2 = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d::UnitY(), kf2.upVector.normalized());

    // 各基底ベクトルをSLERPで補間
    result.xVector = q_x1.slerp(t, q_x2) * Eigen::Vector3d::UnitX();
    result.yVector = q_y1.slerp(t, q_y2) * Eigen::Vector3d::UnitY();
    result.zVector = q_z1.slerp(t, q_z2) * Eigen::Vector3d::UnitZ();
    result.upVector = q_up1.slerp(t, q_up2) * Eigen::Vector3d::UnitY();

    // fov と zoom の線形補間
    result.fov = kf1.fov * (1.0f - t) + kf2.fov * t;
    result.zoom = kf1.zoom * (1.0 - t) + kf2.zoom * t;

    return result;
}

