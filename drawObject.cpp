#include "drawObject.h"
#include "TriMesh.h"
#include <QOpenGLFunctions>

#include "myopenglwidget.h"
#include <QOpenGLWidget>
#include <QResizeEvent> // QResizeEventのために追加
#include <QApplication>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <Eigen/Dense>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPushButton>
#include <QtSvg/QSvgRenderer>
#include <QOpenGLFunctions>
#include <GL/glu.h>
#include "drawObject.h"
#include <iostream>
#include "Light.h"

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#define _USE_MATH_DEFINES
#include <math.h>

#include "Camera.h"

#include <GL/gl.h>


void drawFloor()
{
    glBegin(GL_TRIANGLES);
    for (int j = -20; j < 20; j++)
    {
        for (int i = -20; i < 20; i++)
        {
            int checker_bw = (i + j) % 2;
            if (checker_bw == 0)
            {
                glColor3f(0.3, 0.3, 0.3);

                glVertex3f(i * 0.5, 0.0, j * 0.5);
                glVertex3f(i * 0.5, 0.0, (j + 1) * 0.5);
                glVertex3f((i + 1) * 0.5, 0.0, j * 0.5);

                glVertex3f(i * 0.5, 0.0, (j + 1) * 0.5);
                glVertex3f((i + 1) * 0.5, 0.0, (j + 1) * 0.5);
                glVertex3f((i + 1) * 0.5, 0.0, j * 0.5);
            }
        }
    }
    glEnd();
}

void drawcube(){

    /*
    // ①モデリング変換の適用
    glMatrixMode(GL_MODELVIEW); // モデルビュー行列を選択
    glPushMatrix(); // 現在の行列をスタックに保存

    // 1. オブジェクトを平行移動する
    glTranslatef(1.0f, 1.0f, -0.0f);

    // 2. オブジェクトをY軸周りに45度回転する
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

    // 3. オブジェクトのサイズを倍に拡大する
    glScalef(2.0f, 2.0f, 2.0f);*/

    // 立方体の描画
    glBegin(GL_TRIANGLES);

    //①青
    //glEnable(GL_CULL_FACE); // カリングを有効に
    //glCullFace(GL_BACK); // 裏面をカリング
    //glFrontFace(GL_CCW); // 左回りを表

    glColor3f(0.5, 0.5, 1.0);
    glVertex3f(-0.1, 0.2, 0.3);
    glColor3f(0.5, 0.5, 1.0);
    glVertex3f(0.1, 0.2, 0.3);
    glColor3f(0.5, 0.5, 1.0);
    glVertex3f(0.1, -0.2, 0.3);

    glColor3f(0.5, 0.5, 1.0);
    glVertex3f(-0.1, 0.2, 0.3);
    glColor3f(0.5, 0.5, 1.0);
    glVertex3f(-0.1, -0.2, 0.3);
    glColor3f(0.5, 0.5, 1.0);
    glVertex3f(0.1, -0.2, 0.3);

    //②
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(-0.1, 0.2, 0.3);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(-0.1, -0.2, 0.3);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(-0.1, 0.2, -0.3);

    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(-0.1, 0.2, -0.3);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(-0.1, -0.2, -0.3);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(-0.1, -0.2, 0.3);

    //③
    glColor3f(1.0, 0.5, 0.5);
    glVertex3f(-0.1, 0.2, -0.3);
    glColor3f(1.0, 0.5, 0.5);
    glVertex3f(-0.1, -0.2, -0.3);
    glColor3f(1.0, 0.5, 0.5);
    glVertex3f(0.1, 0.2, -0.3);

    glColor3f(1.0, 0.5, 0.5);
    glVertex3f(-0.1, -0.2, -0.3);
    glColor3f(1.0, 0.5, 0.5);
    glVertex3f(0.1, 0.2, -0.3);
    glColor3f(1.0, 0.5, 0.5);
    glVertex3f(0.1, -0.2, -0.3);

    //④
    glColor3f(1.0, 1.0, 0.5);
    glVertex3f(0.1, 0.2, -0.3);
    glColor3f(1.0, 1.0, 0.5);
    glVertex3f(0.1, -0.2, -0.3);
    glColor3f(1.0, 1.0, 0.5);
    glVertex3f(0.1, 0.2, 0.3);

    glColor3f(1.0, 1.0, 0.5);
    glVertex3f(0.1, 0.2, 0.3);
    glColor3f(1.0, 1.0, 0.5);
    glVertex3f(0.1, -0.2, 0.3);
    glColor3f(1.0, 1.0, 0.5);
    glVertex3f(0.1, -0.2, -0.3);

    //⑤
    glColor3f(1.0, 0.5, 1.0);
    glVertex3f(-0.1, 0.2, -0.3);
    glColor3f(1.0, 0.5, 1.0);
    glVertex3f(0.1, 0.2, -0.3);
    glColor3f(1.0, 0.5, 1.0);
    glVertex3f(0.1, 0.2, 0.3);

    glColor3f(1.0, 0.5, 1.0);
    glVertex3f(-0.1, 0.2, 0.3);
    glColor3f(1.0, 0.5, 1.0);
    glVertex3f(0.1, 0.2, 0.3);
    glColor3f(1.0, 0.5, 1.0);
    glVertex3f(-0.1, 0.2, -0.3);

    //⑥
    glColor3f(0.5, 1.0, 0.5);
    glVertex3f(-0.1, -0.2, 0.3);
    glColor3f(0.5, 1.0, 0.5);
    glVertex3f(0.1, -0.2, 0.3);
    glColor3f(0.5, 1.0, 0.5);
    glVertex3f(-0.1, -0.2, -0.3);

    glColor3f(0.5, 1.0, 0.5);
    glVertex3f(-0.1, -0.2, -0.3);
    glColor3f(0.5, 1.0, 0.5);
    glVertex3f(0.1, -0.2, -0.3);
    glColor3f(0.5, 1.0, 0.5);
    glVertex3f(0.1, -0.2, 0.3);

    glEnd();
    //glPopMatrix(); // 変換前の行列をスタックから取り出す（元に戻す）
}


// XYZ軸を描画する関数
void drawXYZAxes() {
    glLineWidth(2); // 線の太さを設定
    glBegin(GL_LINES);

    // X軸: 赤色
    glColor3f(1.0, 0.0, 0.0); // 赤色を設定
    glVertex3f(0.0, 0.0, 0.0); // 始点: 原点
    glVertex3f(20.0, 0.0, 0.0); // 終点: X軸の正の方向

    // Y軸: 緑色
    glColor3f(0.0, 1.0, 0.0); // 緑色を設定
    glVertex3f(0.0, 0.0, 0.0); // 始点: 原点
    glVertex3f(0.0, 20.0, 0.0); // 終点: Y軸の正の方向

    // Z軸: 青色
    glColor3f(0.0, 0.0, 1.0); // 青色を設定
    glVertex3f(0.0, 0.0, 0.0); // 始点: 原点
    glVertex3f(0.0, 0.0, 20.0); // 終点: Z軸の正の方向

    glEnd();
}

void  drawcamera(const Eigen::Vector3d &eyePoint, const Eigen::Vector3d &lookAtPoint) {};

// カメラ座標系のXY平面に平面を描画する関数
void drawPlaneInCameraCoords(const Camera& camera, float size) {
    Eigen::Vector3d eyePoint = camera.getEyePoint();
    Eigen::Vector3d lookAtPoint = camera.getLookAtPoint();
    Eigen::Vector3d upVector = camera.getYVector();

    // カメラ座標系に変換
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyePoint.x(), eyePoint.y(), eyePoint.z(),
              lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z(),
              upVector.x(), upVector.y(), upVector.z());

    qDebug() << "EyePoint:"
             << QString("(%1, %2, %3)").arg(eyePoint.x()).arg(eyePoint.y()).arg(eyePoint.z());
    // 平面を描画
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0); // 赤色で描画
    glVertex3f(-size / 2, -size / 2, 0.0); // 左下の頂点
    glVertex3f( size / 2, -size / 2, 0.0); // 右下の頂点
    glVertex3f( size / 2,  size / 2, 0.0); // 右上の頂点
    glVertex3f(-size / 2,  size / 2, 0.0); // 左上の頂点
    glEnd();
}


// XY平面上に升目（グリッド）を描画する関数
void drawXYGrid(float gridSize, int gridCount) {
    glLineWidth(1); // 線の太さを設定
    glColor3f(0.75, 0.75, 0.75); // グレー色を設定

    glBegin(GL_LINES);

    // 升目の単位距離ごとに線を描画
    for (int i = -gridCount; i <= gridCount; i++) {
        // X方向の線
        glVertex3f(i * gridSize, 0, -gridCount * gridSize);
        glVertex3f(i * gridSize,  0, gridCount * gridSize);

        // Y方向の線
        glVertex3f(-gridCount * gridSize,0,  i * gridSize);
        glVertex3f( gridCount * gridSize,0,  i * gridSize);
    }

    glEnd();
}


void drawObject( const Object& in_Object )
{
    for( int i=0; i<in_Object.meshes.size(); i++ )
        drawMesh( in_Object.meshes[i] );
}

Eigen::Vector3d computeGLDirectShading( const Eigen::Vector3d& x, const Eigen::Vector3d& n, const Material& material, const std::vector<AreaLight>& lights )
{
    Eigen::Vector3d I = Eigen::Vector3d::Zero();

    for( int i=0; i<lights.size(); i++ )
    {
        Eigen::Vector3d L = lights[i].pos - x;
        const double dist = L.norm();
        L.normalize();
        const double cos_theta = std::max<double>( 0.0, L.dot( n ) );

        I += lights[i].color.cwiseProduct( material.kd ) * lights[i].intensity * cos_theta / ( dist * dist );
    }

    return I;
}

void computeGLShading( TriMesh& io_Mesh, const std::vector<AreaLight>& lights )
{
    for( int i=0; i<io_Mesh.vertices.size(); i++ )
    {
        Eigen::Vector3d p = io_Mesh.vertices[i];
        Eigen::Vector3d n = io_Mesh.vertex_normals[i];

        io_Mesh.vertex_colors[i] = computeGLDirectShading( p, n, io_Mesh.material, lights );
    }
}

void computeGLShading( Object& io_Object, const std::vector<AreaLight>& lights )
{
    for( int i=0; i<io_Object.meshes.size(); i++ )
    {
        computeGLShading( io_Object.meshes[i], lights );
    }
}

void drawMesh( const TriMesh& in_Mesh )
{
    if( in_Mesh.material.texture != 0 )
    {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, in_Mesh.material.texture );
    }

    glBegin( GL_TRIANGLES );

    for( int j=0; j<in_Mesh.triangles.size(); j++ )
    {
        for( int i=0; i<3; i++ )
        {
            const int vid = in_Mesh.triangles[j](i);
            glColor3d( in_Mesh.vertex_colors[vid].x(), in_Mesh.vertex_colors[vid].y(), in_Mesh.vertex_colors[vid].z() );
            glTexCoord2d( in_Mesh.tex_coords[vid].x() , in_Mesh.tex_coords[vid].y() );
            glVertex3d( in_Mesh.vertices[vid].x(), in_Mesh.vertices[vid].y(), in_Mesh.vertices[vid].z() );
        }
    }

    glEnd();

    if( in_Mesh.material.texture != 0 )
    {
        glDisable( GL_TEXTURE_2D );
    }
}
