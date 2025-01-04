/**
 * @file CameraKeyframe.h
 * @brief カメラのキーフレーム定義
 *
 * このファイルには、3D空間におけるカメラのキーフレーム情報を格納するための
 * CameraKeyframe構造体が定義されています。
 *
 * @details
 * CameraKeyframe構造体は、カメラのアニメーションを管理する際に使用されるデータ構造です。
 * キーフレームごとに以下の情報を記録します:
 *
 * - **frameNumber**: キーフレームが適用されるフレーム番号
 * - **eyePoint**: カメラの視点座標
 * - **lookAtPoint**: カメラが注視する目標点
 * - **upVector**: カメラの上方向を示すベクトル
 * - **xVector, yVector, zVector**: カメラのローカル座標軸
 * - **fov**: 視野角（Field of View）
 * - **zoom**: カメラのズーム倍率
 *
 * この構造体は主に以下の用途に使用されます:
 * - アニメーションの補間: キーフレーム間で値を補間し、滑らかなカメラ移動を実現します。
 * - カメラの動きの記録と再現: 過去の動きを保存し、再現可能な形式で保持します。
 *
 * CameraKeyframeを用いることで、カメラアニメーションの制御が容易になります。
 */
#ifndef CAMERAC_KEYFRAME_H
#define CAMERAC_KEYFRAME_H

#include <QVector3D>
#include <QQuaternion>

// キーフレーム構造体の定義
struct CameraKeyframe {
    int frameNumber;
    QVector3D eyePoint;
    QVector3D lookAtPoint;
    QVector3D upVector;
    QVector3D xVector;
    QVector3D yVector;
    QVector3D zVector;
    float fov;
    double zoom;
};

#endif // CAMERAC_KEYFRAME_H

