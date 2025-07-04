#ifndef CONVERSION_UTILS_H
#define CONVERSION_UTILS_H
#pragma once // 重複インクルードを防止

#include <QVector3D>
#include <Eigen/Dense>

// QVector3DからEigen::Vector3dへの変換
inline Eigen::Vector3d toEigen(const QVector3D& v) {
    return Eigen::Vector3d(v.x(), v.y(), v.z());
}

// Eigen::Vector3dからQVector3Dへの変換
inline QVector3D toQt(const Eigen::Vector3d& v) {
    return QVector3D(static_cast<float>(v.x()), static_cast<float>(v.y()), static_cast<float>(v.z()));
}
#endif // CONVERSION_UTILS_H
