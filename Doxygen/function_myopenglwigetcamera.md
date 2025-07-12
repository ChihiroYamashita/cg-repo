\anchor updateRayTracing_doxygen
### void MyOpenGLWidget_camera::updateRayTracing()

  @brief 複数フレームに分割して全ピクセルのレイトレーシングを進める関数
  @details この関数は、1フレームあたり一定数のピクセル（例：2000個）に対してレイトレーシング計算を実行し、
  その結果を `m_film` の蓄積バッファに加算します。各ピクセルごとにカメラからレイを発射し、交差した位置の法線情報に基づいて色を決定します。
  すべてのピクセルが計算されると、`m_isDirty` フラグが false となり、以降のフレームでは再計算をスキップします。
 
  @see MyOpenGLWidget_camera::debug_computeNormalColor
  @see FilmBuffer::addSample
  @see FilmBuffer::updateFilmBuffer
  @see Camera::screenView
 
  ### 処理の流れ
  @startuml
loop フレーム毎
  MyOpenGLWidget_camera -> updateRayTracing : レンダリング進行
  alt m_isDirty == false
    updateRayTracing --> MyOpenGLWidget_camera : 何もしない
  else
    loop pixelsPerFrame回
      updateRayTracing -> "Camera::screenView" : ピクセルに対応するレイ生成
      updateRayTracing -> debug_computeNormalColor : 法線ベクトルから色を取得
      updateRayTracing -> "FilmBuffer::addSample" : 色を蓄積
      alt 最終ピクセル到達
        updateRayTracing -> m_isDirty : falseに設定
      end
    end
    updateRayTracing -> "FilmBuffer::updateFilmBuffer" : 平均色に変換
    updateRayTracing -> OpenGL : glTexSubImage2D によりテクスチャ転送
    updateRayTracing -> update : 再描画要求
  end
end
@enduml
 
  #### 1. 再レンダリングが必要か確認
  ```cpp
  if (!m_isDirty) return;
  ```
  すでに全ピクセルの計算が終わっていれば何もしません。
 
  #### 2. 一度に計算するピクセル数を設定
  ```cpp
  const int pixelsPerFrame = 2000;
  ```
  1フレームあたりの処理量を決めるパラメータで、描画のスムーズさと進行速度をトレードオフします。
 
  #### 3. 指定ピクセル数だけレイトレーシングを繰り返す
  ```cpp
  for (int k = 0; k < pixelsPerFrame; ++k) {
  ```
  ループ内で各ピクセルの処理を順番に進めます。
 
  #### 4. 正規化スクリーン座標に変換
  ```cpp
  double p_x = (double)m_progress_i / width;
  double p_y = (double)m_progress_j / height;
  ```
  スクリーン座標 (i, j) を 0.0～1.0 に正規化し、レイの発射に利用します。
 
  #### 5. カメラからレイを発射
  ```cpp
  g_Camera2.screenView(p_x, p_y, ray);
  ```
  カメラからスクリーン上の位置に向けてレイを生成します。
 
  #### 6. レイに対応する色（法線可視化）を取得
  ```cpp
  Eigen::Vector3d color = debug_computeNormalColor(ray);
  ```
  三角形とレイの交差を計算し、ヒットした箇所の法線ベクトルを RGB に変換した色を取得します。
 
  #### 7. `FilmBuffer` に色を蓄積
  ```cpp
  m_film.addSample(m_progress_i, m_progress_j, color);
  ```
  同一ピクセルに対して複数サンプルを加算することでアンチエイリアス等も可能になります。
 
  #### 8. 次のピクセルへ移動
  ```cpp
  m_progress_i++;
  if (m_progress_i >= width) {
      m_progress_i = 0;
      m_progress_j++;
  ```
  横方向に進み、右端に到達したら次の行へ進みます。
 
  #### 9. すべてのピクセルが終わったら終了
  ```cpp
  if (m_progress_j >= height) {
      m_isDirty = false;
      qDebug() << "Rendering finished.";
      break;
  }
  ```
  全ピクセルの処理が完了したら、dirty フラグを false にして再計算を止めます。
 
  #### 10. 蓄積された色から平均を計算
  ```cpp
  m_film.updateFilmBuffer();
  ```
  それぞれのピクセルで加算された色の平均を求めて、描画用バッファに展開します。
 
  #### 11. OpenGLテクスチャに転送
  ```cpp
  makeCurrent();
  glBindTexture(...);
  glTexSubImage2D(..., m_film.getFilmBufferPtr());
  glBindTexture(...);
  doneCurrent();
  ```
  GPUテクスチャに更新されたフィルムバッファを転送します。
 
  #### 12. `paintGL()` の呼び出しをスケジュール
  ```cpp
  update();
  ```
  Qt に対して再描画を要求し、画面が更新されます。
 
  ---
 
  @see MyOpenGLWidget_camera::debug_computeNormalColor
  @see FilmBuffer::addSample
  @see FilmBuffer::updateFilmBuffer
  @see Camera::screenView
 
 
  @note 本関数はプログレッシブ・レンダリングを前提としており、UIをブロックせず滑らかに描画が進む設計です。
 /