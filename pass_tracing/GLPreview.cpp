//
//  GLPreview.cpp
//  gl3d_raytracing_v2
//
//  Created by Yonghao Yue on 2019/11/19.
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

#include "GLPreview.h"

void projection_and_modelview( const Camera& in_Camera, const int width, const int height )
{
  const double fovy_deg = ( 2.0 * 180.0 / M_PI ) * atan( 0.024 * 0.5 / in_Camera.getFocalLength() );
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( fovy_deg, double( width ) / double( height ), 0.01 * in_Camera.getFocalLength(), 1000.0 );
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  const Eigen::Vector3d lookAtPoint = in_Camera.getLookAtPoint();
  gluLookAt( in_Camera.getEyePoint().x(), in_Camera.getEyePoint().y(), in_Camera.getEyePoint().z(), lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z(), in_Camera.getYVector().x(), in_Camera.getYVector().y(), in_Camera.getYVector().z() );
}

/*
void drawLight( const Light& in_Light )
{
  const double size = 0.05;
  glBegin( GL_TRIANGLES );
  
  glColor3f( in_Light.color.x(), in_Light.color.y(), in_Light.color.z() );
  Eigen::Vector3d pos = in_Light.pos;
  
  glVertex3f( pos.x(), pos.y() + size, pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() + size );
  glVertex3f( pos.x() + size, pos.y(), pos.z() );
  
  glVertex3f( pos.x(), pos.y() + size, pos.z() );
  glVertex3f( pos.x() + size, pos.y(), pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() - size );
  
  glVertex3f( pos.x(), pos.y() + size, pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() - size );
  glVertex3f( pos.x() - size, pos.y(), pos.z() );
  
  glVertex3f( pos.x(), pos.y() + size, pos.z() );
  glVertex3f( pos.x() - size, pos.y(), pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() + size );
  
  glVertex3f( pos.x(), pos.y() - size, pos.z() );
  glVertex3f( pos.x() + size, pos.y(), pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() + size );
  
  glVertex3f( pos.x(), pos.y() - size, pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() - size );
  glVertex3f( pos.x() + size, pos.y(), pos.z() );
  
  glVertex3f( pos.x(), pos.y() - size, pos.z() );
  glVertex3f( pos.x() - size, pos.y(), pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() - size );
  
  glVertex3f( pos.x(), pos.y() - size, pos.z() );
  glVertex3f( pos.x(), pos.y(), pos.z() + size );
  glVertex3f( pos.x() - size, pos.y(), pos.z() );
  
  glEnd();
}

void drawLights( const std::vector<Light>& in_Lights )
{
  for( int i=0; i<in_Lights.size(); i++ )
  {
    drawLight( in_Lights[i] );
  }
}

//*/

void drawLights( const std::vector<AreaLight>& in_Lights )
{
  for( int i=0; i<in_Lights.size(); i++ )
  {
    Eigen::Vector3d p1 = in_Lights[i].pos + in_Lights[i].arm_u + in_Lights[i].arm_v;
    Eigen::Vector3d p2 = in_Lights[i].pos - in_Lights[i].arm_u + in_Lights[i].arm_v;
    Eigen::Vector3d p3 = in_Lights[i].pos - in_Lights[i].arm_u - in_Lights[i].arm_v;
    Eigen::Vector3d p4 = in_Lights[i].pos + in_Lights[i].arm_u - in_Lights[i].arm_v;
    
    glBegin( GL_TRIANGLES );
    
    glColor3f( in_Lights[i].color.x(), in_Lights[i].color.y(), in_Lights[i].color.z() );
    
    glVertex3d( p1.x(), p1.y(), p1.z() );
    glVertex3d( p2.x(), p2.y(), p2.z() );
    glVertex3d( p3.x(), p3.y(), p3.z() );
    
    glVertex3d( p1.x(), p1.y(), p1.z() );
    glVertex3d( p3.x(), p3.y(), p3.z() );
    glVertex3d( p4.x(), p4.y(), p4.z() );
    
    glEnd();
  }
}

/**
 * @brief フィルムバッファをスクリーンに描画する関数
 * @details この関数は、カメラの視点を基準にフィルムバッファを四角形（矩形）として3D空間に配置し、
 * OpenGL を用いてスクリーンに描画します。フィルムバッファの内容はテクスチャとして貼り付けられます。
 *
 * @param[in] in_Camera カメラ情報（視点、スクリーンサイズ、焦点距離などを含む）
 * @param[in] in_FilmTexture フィルムバッファを格納した OpenGL のテクスチャオブジェクト
 *
 * @details **処理の流れ**:
 *
 * #### 1. スクリーンの頂点を計算
 * カメラのスクリーン位置を基準に、四角形の4つの頂点を計算します。
 * @code
 * Eigen::Vector3d screen_center = in_Camera.getEyePoint() - in_Camera.getZVector() * in_Camera.getFocalLength();
 * Eigen::Vector3d p1 = screen_center - in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 - in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
 * Eigen::Vector3d p2 = screen_center + in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 - in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
 * Eigen::Vector3d p3 = screen_center + in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 + in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
 * Eigen::Vector3d p4 = screen_center - in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 + in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
 * @endcode
 *
 * #### 2. OpenGL のテクスチャモードを有効化
 * 描画対象となるテクスチャを OpenGL にバインドします。
 * @code
 * glEnable(GL_TEXTURE_2D);
 * glBindTexture(GL_TEXTURE_2D, in_FilmTexture);//in_FilmTextureを貼り付ける
 * @endcode
 *
 * #### 3. テクスチャを貼り付けた四角形を描画
 * テクスチャの UV 座標と対応する頂点座標を指定し、三角形を2つ描画して四角形を形成します。
 * glTexCoord2f(u, v): UV 座標を指定。
 * glVertex3f(x, y, z): 頂点の 3D 空間座標を指定。
 * @code
 * glBegin(GL_TRIANGLES);
 * glTexCoord2f(0.0, 1.0);
 * glVertex3f(p1.x(), p1.y(), p1.z());
 * glTexCoord2f(1.0, 1.0);
 * glVertex3f(p2.x(), p2.y(), p2.z());
 * glTexCoord2f(1.0, 0.0);
 * glVertex3f(p3.x(), p3.y(), p3.z());
 *
 * glTexCoord2f(0.0, 1.0);
 * glVertex3f(p1.x(), p1.y(), p1.z());
 * glTexCoord2f(1.0, 0.0);
 * glVertex3f(p3.x(), p3.y(), p3.z());
 * glTexCoord2f(0.0, 0.0);
 * glVertex3f(p4.x(), p4.y(), p4.z());
 * glEnd();
 * @endcode
 *
 * #### 4. OpenGL のテクスチャモードを無効化
 * テクスチャ描画が終了した後、テクスチャモードを無効化します。
 * @code
 * glDisable(GL_TEXTURE_2D);
 * @endcode
 *
 * ### シーケンス図
 * @startuml
 * participant Main as M
 * participant Camera as C
 * participant OpenGL as GL
 *
 * M -> C: Retrieve screen center and vectors
 * C -> M: Return screen center, X, Y, Z vectors
 * M -> GL: Enable GL_TEXTURE_2D
 * M -> GL: Bind in_FilmTexture
 * M -> GL: Draw textured rectangle with vertex and UV mapping
 * GL --> M: Rendering complete
 * @enduml
 *
 * @see glEnable
 * @see glBindTexture
 * @see glBegin
 * @see glTexCoord2f
 * @see glVertex3f
 * @see glDisable
 */


void drawFilm( const Camera& in_Camera, GLuint in_FilmTexture )
{
  const Eigen::Vector3d screen_center = in_Camera.getEyePoint() - in_Camera.getZVector() * in_Camera.getFocalLength();
  const Eigen::Vector3d p1 = screen_center - in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 - in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
  const Eigen::Vector3d p2 = screen_center + in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 - in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
  const Eigen::Vector3d p3 = screen_center + in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 + in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;
  const Eigen::Vector3d p4 = screen_center - in_Camera.getXVector() * in_Camera.getScreenWidth() * 0.5 + in_Camera.getYVector() * in_Camera.getScreenHeight() * 0.5;

  //glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // 青色
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glEnable( GL_TEXTURE_2D );
  //glBindTexture( GL_TEXTURE_2D, in_FilmTexture );

  glBegin( GL_TRIANGLES );
  glColor3f( 0.0, 1.0, 1.0 );

  //glTexCoord2f( 0.0, 1.0 );
  glVertex3f( p1.x(), p1.y(), p1.z() );
  //glTexCoord2f( 1.0, 1.0 );
  glVertex3f( p2.x(), p2.y(), p2.z() );
  //glTexCoord2f( 1.0, 0.0 );
  glVertex3f( p3.x(), p3.y(), p3.z() );

  //glTexCoord2f( 0.0, 1.0 );
  glVertex3f( p1.x(), p1.y(), p1.z() );
  //glTexCoord2f( 1.0, 0.0 );
  glVertex3f( p3.x(), p3.y(), p3.z() );
  //glTexCoord2f( 0.0, 0.0 );
  glVertex3f( p4.x(), p4.y(), p4.z() );

  glEnd();

  //glDisable( GL_TEXTURE_2D );
  //qDebug() << "drawFilm is executed sucessfully";



}

/*
void drawFloor()
{
  glBegin( GL_TRIANGLES );
  for( int j=-20; j<20; j++ )
  {
    for( int i=-20; i<20; i++ )
    {
      int checker_bw = ( i+j ) % 2;
      if( checker_bw == 0 )
      {
        glColor3f( 0.3, 0.3, 0.3 );
        
        glVertex3f( i*0.5, 0.0, j*0.5 );
        glVertex3f( i*0.5, 0.0, (j+1)*0.5 );
        glVertex3f( (i+1)*0.5, 0.0, j*0.5 );
        
        glVertex3f( i*0.5, 0.0, (j+1)*0.5 );
        glVertex3f( (i+1)*0.5, 0.0, (j+1)*0.5 );
        glVertex3f( (i+1)*0.5, 0.0, j*0.5 );
      }
    }
  }
  glEnd();
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
*/
