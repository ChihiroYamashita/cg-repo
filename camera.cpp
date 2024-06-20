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

void Camera::setEyePoint( const QVector3D& in_eyePoint )
{
    m_EyePoint = in_eyePoint;

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

void Camera::lookAt( const QVector3D& in_LookAt, const QVector3D& in_Up )
{
    // armの定義
    const QVector3D arm = in_LookAt - m_EyePoint;
    m_DistanceToObject = arm.length();
    // m_zVectorはlookatpointからeyepoint方向に伸びている(中心→カメラ)
    m_zVector = - arm / m_DistanceToObject;

    const double dot_up_z = QVector3D::dotProduct(in_Up, m_zVector);;
    m_yVector = in_Up - dot_up_z * m_zVector;
    m_yVector.normalize();
    m_xVector = QVector3D::crossProduct(m_yVector, m_zVector);
}

void Camera::moveInGlobalFrame( const QVector3D& in_delta )
{
    m_EyePoint += in_delta;
}

void Camera::moveInLocalFrame( const QVector3D& in_delta )
{
    m_EyePoint += in_delta.x() * m_xVector + in_delta.y() * m_yVector + in_delta.z() * m_zVector;

}

void Camera::moveInGlobalFrameFixLookAt( const QVector3D& in_delta )
{
    const QVector3D lookAtPoint = getLookAtPoint();
    const QVector3D up = m_yVector;

    moveInGlobalFrame( in_delta );
    lookAt( lookAtPoint, up );
}

void Camera::moveInLocalFrameFixLookAt( const QVector3D& in_delta )
{
    const QVector3D lookAtPoint = getLookAtPoint();
    const QVector3D up = m_yVector;

    moveInLocalFrame( in_delta );
    lookAt( lookAtPoint, up );
}



//カメラの視点を回転させる関数
//この回転は特定の点（LookAtポイント）を中心に円軌道で行われる
void Camera::rotateCameraInLocalFrameFixLookAt( const double& in_HorizontalAngle, const double& in_VerticalAngle)
{

    /* LookAtポイントの取得　 */
    const QVector3D lookAtPoint = getLookAtPoint();

    /* m_DistanceToObject(距離のみ) * m_zVector(正規化ベクトル)により、カメラの位置を特定の対象物を中心に回転させる際に使われるベクトル（アーム）を計算　 */
    QVector3D arm = m_DistanceToObject * m_zVector;

    const QVector3D worldUp { 0.0, 1.0, 0.0 };

//qDebug() << "lookAtPointbefore:" <<lookAtPoint;

    // 入力 ( 回転させたいベクトル, 回転軸を表すベクトル,回転させる角度（ラジアン単位）)

    m_xVector = rotateVector( m_xVector, worldUp, in_HorizontalAngle );
    m_yVector = rotateVector( m_yVector, worldUp, in_HorizontalAngle );
    m_zVector = rotateVector( m_zVector, worldUp, in_HorizontalAngle );
    arm = rotateVector(arm, worldUp, in_HorizontalAngle);
    m_xVector.normalize();
    m_yVector.normalize();
    m_zVector.normalize();

    // rotate around x-axis(カメラローカルx座標)

    //arm= arm - lookAtPoint;

    m_xVector = rotateVector(m_xVector, m_xVector, in_VerticalAngle);
    m_yVector = rotateVector(m_yVector, m_xVector, in_VerticalAngle);
    m_zVector = rotateVector(m_zVector, m_xVector, in_VerticalAngle);
    arm = rotateVector(arm, m_xVector, in_VerticalAngle);
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




QVector3D Camera::getLookAtPoint() const
{

    QVector3D currentLookAtPoint = m_EyePoint - m_DistanceToObject * m_zVector;
    double threshold = 1e-5; // 閾値の設定

    // 前回のLookAtPointとの差が閾値以下なら、前回の値を返す
    if ((currentLookAtPoint - m_LastLookAtPoint).length() <= threshold) {
        return m_LastLookAtPoint;
    } else {
        // 変更がある場合は、現在のLookAtPointを保存し、返す
        const_cast<Camera*>(this)->m_LastLookAtPoint = currentLookAtPoint; // constメンバ関数内での値変更のためconst_castを使用
        return currentLookAtPoint;
    }
}

QVector3D Camera::getEyePoint() const
{
    return m_EyePoint;
}

QVector3D Camera::getXVector() const
{
    return m_xVector;
}

QVector3D Camera::getYVector() const
{
    return m_yVector;
}

QVector3D Camera::getZVector() const
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

