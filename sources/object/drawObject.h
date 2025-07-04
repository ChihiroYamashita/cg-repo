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
