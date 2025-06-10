/**
 * @file drawObject.h
 * @brief OpenGL描画ユーティリティ
 *
 * @details
 * OpenGLで3Dオブジェクトを描画するための関数群を提供します。
 * キューブやグリッド、座標軸などの基本図形を簡単に描画できます。
 */
#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H
#include "camera.h"
#include <QOpenGLFunctions>

void drawcube();
void drawFloor();
void  drawcamera(const Eigen::Vector3d &eyePoint, const Eigen::Vector3d &lookAtPoint);
void drawPlaneInCameraCoords(const Camera& camera, float size);
void drawXYZAxes();
void drawXYGrid(float gridSize, int gridCount) ;
#endif // DRAWOBJECT_H
