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

    , m_LastLookAtPoint(0.0, 0.0, 0.0)
    ,  m_aspectRatio (4.0 / 3.0)// デフォルトのアスペクト比 (例: 4:3)
    , m_ScreenHeight( m_ScreenWidth / m_aspectRatio )

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

     m_xVector = rotateVector(m_xVector, worldUp, in_HorizontalAngle);
     m_yVector = rotateVector(m_yVector, worldUp, in_HorizontalAngle);
     m_zVector = rotateVector(m_zVector, worldUp, in_HorizontalAngle);
     arm       = rotateVector(arm, worldUp, in_HorizontalAngle);
    m_xVector.normalize();
    m_yVector.normalize();
    m_zVector.normalize();

    // rotate around x-axis(カメラローカルx座標)

    //arm= arm - lookAtPoint;

    // m_yVector, m_zVector, armを、新しいm_xVectorを軸として回転
    m_yVector = rotateVector(m_yVector, m_xVector, in_VerticalAngle);
    m_zVector = rotateVector(m_zVector, m_xVector, in_VerticalAngle);
    arm       = rotateVector(arm, m_xVector, in_VerticalAngle);
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

/**
 * @brief カメラのスクリーン平面のアスペクト比を設定します。
 * @param[in] aspect 新しいアスペクト比 (width / height)
 */
void Camera::setAspectRatio(double aspect)
{
    if (aspect <= 0) return; // 0以下の無効な値は無視

    m_aspectRatio = aspect;
    m_ScreenHeight = m_ScreenWidth / m_aspectRatio; // アスペクト比に合わせて高さを再計算
}


/**
* @brief カメラのスクリーン座標に基づいてレイを生成する関数
* @details 画面上の2D座標（in_x, in_y）を受け取り、その座標を基準にしたレイの起点（`out_Ray.o`）と方向（`out_Ray.d`）を計算します。
* カメラの現在の視点（`m_EyePoint`）とスクリーンの寸法（`m_ScreenWidth`, `m_ScreenHeight`）、焦点距離（`m_FocalLength`）を使用してレイを生成します。
* スクリーン上の座標が正規化されている（0.0 ～ 1.0）ことを前提としています。
*
* @param[in] in_x スクリーン上のx座標（スクリーン座標系: 0.0 ～ 1.0の値を持つ）
* @param[in] in_y スクリーン上のy座標（スクリーン座標系: 0.0 ～ 1.0の値を持つ）
* @param[out] out_Ray 計算されたレイ 始点(orgin)と方向(direction)を持つ
*
* @startuml
*
* main -> Camera : screenView(in_x, in_y, out_Ray)
* Camera -> Camera : レイ生成処理を開始
* Camera -> Camera : s = (in_x - 0.5) * m_ScreenWidth \nスクリーン上のx座標(0~1)をカメラ空間の物理的な横の長さに変換
* Camera -> Camera : t = (0.5 - in_y) * m_ScreenHeight \nスクリーン上のy座標をカメラ空間の物理的な縦方向距離に変換
* Camera -> Camera : out_Ray.o = m_EyePoint \nレイの起点（origin）をカメラの視点位置（m_EyePoint）に設定
* Camera -> Camera : out_Ray.d = m_xVector * s + m_yVector * t - m_zVector * m_FocalLength \nレイの方向ベクトルを計算
* Camera -> Camera : out_Ray.d.normalize() \nレイの方向ベクトルを正規化し、単位ベクトルに変換
*
* @enduml
*
* レイの起点（`out_Ray.o`）と方向（`out_Ray.d`）を保存
*/
void Camera::screenView( const double in_x, const double in_y, Ray& out_Ray )
{
    const double s = ( in_x - 0.5 ) * m_ScreenWidth;
    const double t = ( 0.5 - in_y ) * m_ScreenHeight;

    out_Ray.o = m_EyePoint;
    out_Ray.d = m_xVector * s + m_yVector * t - m_zVector * m_FocalLength;
    out_Ray.d.normalize();
}
