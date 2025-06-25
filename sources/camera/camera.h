#ifndef CAMERA_H
#define CAMERA_H
#include <QVector3D>
#include <QtMath> // Qtの数学関数用
#include <QQuaternion>
//
//  Camera.h
//
//  Created by Yonghao Yue on 2019/09/28.
//  Updated: 2020/09/23
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//



#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#include <Eigen/Dense>

class Camera
{
public:
    Camera();

    void setEyePoint( const QVector3D& in_eyePoint );
    void setDistanceToObject( const double& in_DistanceToObject );
    void setFocalLength( const double& in_FocalLength );
    void lookAt( const QVector3D& in_LookAt, const QVector3D& in_Up );

    void moveInGlobalFrame( const QVector3D& in_delta );
    void moveInLocalFrame( const QVector3D& in_delta );

    void zoomCamera(const double delta);//カメラズーム

    void moveInGlobalFrameFixLookAt( const QVector3D& in_delta );
    void moveInLocalFrameFixLookAt( const QVector3D& in_delta );

    void setFov(const float& in_Fov);
    float getFov() const;



    void rotateCameraInLocalFrameFixLookAt(const double& in_HorizontalAngle, const double& in_VerticalAngle);

    QVector3D getLookAtPoint() const;
    QVector3D getEyePoint() const;
    QVector3D getXVector() const;
    QVector3D getYVector() const;
    QVector3D getZVector() const;




    double getDistanceToObject() const;
    double getFocalLength() const;
    double getScreenWidth() const;
    double getScreenHeight() const;

protected:
    QVector3D m_EyePoint;
    QVector3D m_xVector;
    QVector3D m_yVector;
    QVector3D m_zVector;

    float m_Fov;

    double m_DistanceToObject;
    double m_FocalLength;
    double m_ScreenWidth;
    double m_ScreenHeight;

    QVector3D m_LastLookAtPoint;//新規追加　getlookatpointの丸め誤差用
};

// rotate a given vector in_v around a given axis in_axis for a given angle in_angle_rad,
// using quaternion

// 入力 ( 回転させたいベクトル, 回転軸を表すベクトル,回転させる角度（ラジアン単位）)
inline QVector3D rotateVector( const QVector3D& in_v, const QVector3D& in_axis, const double& in_angle_rad )
{
    /*QVector3D axis = in_axis.normalized();

    const double cos_a = cos( in_angle_rad * 0.5 );
    const double sin_a = sin( in_angle_rad * 0.5 );
    const double m_sin_a = -sin_a;

    // compute (r * p * q).v, where r, p, and q are quaternions given by
    // p = (0, px, py, pz)
    // q = (cos_a, axis_x * sin_a, axis_y * sin_a, axis_z * sin_a)
    // r = (cos_a, axis_x * m_sin_a, axis_y * m_sin_a, axis_z * m_sin_a)

    const double r_p_t = -m_sin_a * QVector3D::dotProduct(axis, in_v);
    const QVector3D r_p  = cos_a * in_v + m_sin_a * QVector3D::crossProduct(axis, in_v);
    return sin_a * r_p_t * axis + cos_a * r_p + sin_a * QVector3D::crossProduct(axis, r_p);*/

    QQuaternion rotation = QQuaternion::fromAxisAndAngle(in_axis, qRadiansToDegrees(in_angle_rad));
    return rotation.rotatedVector(in_v);
}




#endif // CAMERA_H
