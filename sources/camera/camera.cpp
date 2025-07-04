//
//  Camera.cpp
//
//  Created by Yonghao Yue on 2019/09/28.
//  Updated 2021/10/06
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#include "camera.h"
#include <QVector3D>
#include <QtMath> // Qtの数学関数用
#include <QDebug>

#include "conversion_utils.h"

Camera::Camera()
    : m_EyePoint(0.0, 0.0, 0.0)
    , m_xVector(1.0, 0.0, 0.0)
    , m_yVector(0.0, 1.0, 0.0)
    , m_zVector(0.0, 0.0, 1.0)
    , m_DistanceToObject( 1.0 )
    , m_FocalLength( 0.035 )
    , m_ScreenWidth( 0.036 )
    , m_ScreenHeight( 0.024 )
    , m_LastLookAtPoint(0.0, 0.0, 0.0)
{

}

void Camera::setEyePoint( const Eigen::Vector3d& in_eyePoint )
{
    m_EyePoint =in_eyePoint;//置換

}

void Camera::setDistanceToObject( const double& in_DistanceToObject )
{
    m_DistanceToObject = in_DistanceToObject;
}

void Camera::setFocalLength( const double& in_FocalLength )
{
    m_FocalLength = in_FocalLength;
}

/*---------------追加コード-------------------------------------------*/
void Camera::setFov(const float& in_Fov) {
    m_Fov = in_Fov;
}

float Camera:: getFov() const {
    return m_Fov;
}

/*--------------------------------------------------------------*/

void Camera::lookAt( const Eigen::Vector3d& in_LookAt, const Eigen::Vector3d& in_Up )
{

    //in_LookAt、in_Up置換
    const Eigen::Vector3d lookAtEigen = in_LookAt; // 変換
    const Eigen::Vector3d upEigen = in_Up;       // 変換

    //置換終わり

    // armの定義
     const Eigen::Vector3d arm = lookAtEigen - m_EyePoint;



    m_DistanceToObject = arm.norm();
    // m_zVectorはlookatpointからeyepoint方向に伸びている(中心→カメラ)
    m_zVector = - arm / m_DistanceToObject;

    const double dot_up_z = upEigen.dot(m_zVector);
    m_yVector =  (upEigen - dot_up_z * m_zVector).normalized();
    m_xVector = m_yVector.cross(m_zVector);
}

void Camera::moveInGlobalFrame( const Eigen::Vector3d& in_delta )
{
    m_EyePoint += in_delta;
}

void Camera::moveInLocalFrame( const Eigen::Vector3d& in_delta )
{
    m_EyePoint += in_delta.x() * m_xVector + in_delta.y() * m_yVector + in_delta.z() * m_zVector;

}

void Camera::moveInGlobalFrameFixLookAt( const Eigen::Vector3d& in_delta )
{
    const Eigen::Vector3d lookAtPoint = getLookAtPoint();
    const Eigen::Vector3d up = m_yVector;

    moveInGlobalFrame( in_delta );
    lookAt( lookAtPoint, up );
}

void Camera::moveInLocalFrameFixLookAt( const Eigen::Vector3d& in_delta )
{
    const Eigen::Vector3d lookAtPoint = getLookAtPoint();
    const Eigen::Vector3d up = m_yVector;

    moveInLocalFrame( in_delta );
    lookAt( lookAtPoint, up );
}



//カメラの視点を回転させる関数
//この回転は特定の点（LookAtポイント）を中心に円軌道で行われる
void Camera::rotateCameraInLocalFrameFixLookAt( const double& in_HorizontalAngle, const double& in_VerticalAngle)
{

    /* LookAtポイントの取得　 */
    const Eigen::Vector3d  lookAtPoint = getLookAtPoint();

    /* m_DistanceToObject(距離のみ) * m_zVector(正規化ベクトル)により、カメラの位置を特定の対象物を中心に回転させる際に使われるベクトル（アーム）を計算　 */
     Eigen::Vector3d arm= m_DistanceToObject * m_zVector;

     const Eigen::Vector3d worldUp { 0.0, 1.0, 0.0 };

//qDebug() << "lookAtPointbefore:" <<lookAtPoint;

    // 入力 ( 回転させたいベクトル, 回転軸を表すベクトル,回転させる角度（ラジアン単位）)

     m_xVector = toEigen(rotateVector( toQt(m_xVector), toQt(worldUp), in_HorizontalAngle ));
     m_yVector = toEigen(rotateVector( toQt(m_yVector), toQt(worldUp), in_HorizontalAngle ));
     m_zVector = toEigen(rotateVector( toQt(m_zVector), toQt(worldUp), in_HorizontalAngle ));
     arm = toEigen(rotateVector(toQt(arm), toQt(worldUp), in_HorizontalAngle));
    m_xVector.normalize();
    m_yVector.normalize();
    m_zVector.normalize();

    // rotate around x-axis(カメラローカルx座標)

    //arm= arm - lookAtPoint;

    m_xVector = toEigen(rotateVector(toQt(m_xVector), toQt(m_xVector), in_VerticalAngle));
    m_yVector = toEigen(rotateVector(toQt(m_yVector), toQt(m_xVector), in_VerticalAngle));
    m_zVector = toEigen(rotateVector(toQt(m_zVector), toQt(m_xVector), in_VerticalAngle));
    arm = toEigen(rotateVector(toQt(arm), toQt(m_xVector), in_VerticalAngle));
    //arm = arm + lookAtPoint;


    m_xVector.normalize();
    m_yVector.normalize();
    m_zVector.normalize();

    m_EyePoint = lookAtPoint + arm;
    m_LastLookAtPoint=lookAtPoint;

//qDebug() << "lookAtPoint:" <<lookAtPoint;
// qDebug() << "EyePoint:" << m_EyePoint;

}

void Camera::zoomCamera(const double delta){
    m_DistanceToObject += -delta;
    m_EyePoint += -delta * m_zVector;
}




Eigen::Vector3d Camera::getLookAtPoint() const
{

    Eigen::Vector3d currentLookAtPoint = m_EyePoint - m_DistanceToObject * m_zVector;
    double threshold = 1e-5; // 閾値の設定

    // 前回のLookAtPointとの差が閾値以下なら、前回の値を返す
    if ((currentLookAtPoint - m_LastLookAtPoint).norm() <= threshold) {
        return m_LastLookAtPoint;
    } else {
        // 変更がある場合は、現在のLookAtPointを保存し、返す
        const_cast<Camera*>(this)->m_LastLookAtPoint = currentLookAtPoint; // constメンバ関数内での値変更のためconst_castを使用
        return currentLookAtPoint;
    }
}

Eigen::Vector3d Camera::getEyePoint() const
{
    return m_EyePoint;
}

Eigen::Vector3d Camera::getXVector() const
{
    return m_xVector;
}

Eigen::Vector3d Camera::getYVector() const
{
    return m_yVector;
}

Eigen::Vector3d Camera::getZVector() const
{
    return m_zVector;
}

double Camera::getDistanceToObject() const
{
    return m_DistanceToObject;
}

double Camera::getFocalLength() const
{
    return m_FocalLength;
}

double Camera::getScreenWidth() const
{
    return m_ScreenWidth;
}

double Camera::getScreenHeight() const
{
    return m_ScreenHeight;
}

