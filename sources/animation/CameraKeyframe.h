#ifndef CAMERAC_KEYFRAME_H
#define CAMERAC_KEYFRAME_H
#include <Eigen/Dense>


// キーフレーム構造体の定義
struct CameraKeyframe {
    int frameNumber;
    Eigen::Vector3d eyePoint;
    Eigen::Vector3d lookAtPoint;
    Eigen::Vector3d upVector;
    Eigen::Vector3d xVector;
    Eigen::Vector3d yVector;
    Eigen::Vector3d zVector;
    float fov;
    double zoom;
};

#endif // CAMERAC_KEYFRAME_H

