#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H
#include "camera.h"
#include <QOpenGLFunctions>

void drawcube();
void drawFloor();
void  drawcamera(const QVector3D &eyePoint, const QVector3D &lookAtPoint);
void drawPlaneInCameraCoords(const Camera& camera, float size);
void drawXYZAxes();
void drawXYGrid(float gridSize, int gridCount) ;
#endif // DRAWOBJECT_H
