#ifndef RAY_H
#define RAY_H
#include <QVector3D>
#include <Eigen/Dense>

struct Ray
{
    Eigen::Vector3d o;
    Eigen::Vector3d d;
    int depth;
    int prev_mesh_idx;
    int prev_primitive_idx;
};


#endif // RAY_H
