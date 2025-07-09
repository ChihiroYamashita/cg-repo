//
//  GLPreview.cpp
//  gl3d_raytracing_v2
//
//  Created by Yonghao Yue on 2019/11/19.
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

//OpenGLによるリアルタイムプレビュー用描画・ライティング処理群のファイル

#include "GLPreview.h"
/*
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
*/
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
/**
 * @brief オブジェクト全体に対して簡易な OpenGL 向け直接照明カラーを計算する
 * @details 各メッシュごとに頂点のワールド座標と法線方向を使い、エリアライトからの直接光の影響を評価し、
 * 頂点カラー（vertex_colors）として格納します。このカラーは OpenGL によるリアルタイム描画時に使用され、
 * レイトレーシングとは異なる即時レンダリング用の擬似ライティング結果です。
 *
 * ライトとの距離と法線の角度に基づいた Lambertian 反射モデルで強度を計算します。
 * テクスチャがないメッシュや、確認用のリアルタイム描画に便利です。
 *
 * @param[out] io_Object 対象となる 3D オブジェクト。各メッシュに対して頂点カラーが上書きされます。
 * @param[in] lights 照明として使われるエリアライトのリスト。
 *
 * @see computeGLShading(TriMesh&, const std::vector<AreaLight>&)
 * @see computeGLDirectShading
 *
 * @startuml
 * loop 各メッシュに対して
 *     computeGLShading(mesh, lights)
 * end
 * @enduml
 *
 * ### 処理の流れ（簡易版）
 *
 * 1. 各メッシュに対してループ
 * 2. メッシュ内の各頂点の位置と法線をもとに光強度を計算
 * 3. 結果を `vertex_colors` に書き込む
 *
 * @note この関数は OpenGL 用の描画結果にしか影響を与えません。レイトレーシングとは無関係です。
 */
void computeGLShading( Object& io_Object, const std::vector<AreaLight>& lights )
{
  for( int i=0; i<io_Object.meshes.size(); i++ )
  {
    computeGLShading( io_Object.meshes[i], lights );
  }
}

/**
 * @brief 各メッシュのマテリアル色(kd)を、そのメッシュの全頂点カラーに適用する
 * @param[out] io_Object 対象となるObject
 */
void applyMaterialColorToVertices(Object& io_Object)
{
  for (auto& mesh : io_Object.meshes)
  {
    const Eigen::Vector3d material_color = mesh.material.kd;

    // assignの代わりに、手動のforループで各要素に代入する
    for (size_t i = 0; i < mesh.vertex_colors.size(); ++i)
    {
      mesh.vertex_colors[i] = material_color;
    }
  }
}


