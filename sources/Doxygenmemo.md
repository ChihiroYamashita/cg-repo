## Doxygenメモ

- リンク渡す方法
    1. mdファイルの一部分→コードのコメントの場合
    
    ```cpp
     * 復習 @ref barycentric_coordinates "バリュートリック座標" を参照してください。
    ```
  ---  
    ```md
     \anchor barycentric_coordinates
    ```
    ↑mdファイル内にこのタグの記述を入れておく「barycentric_coordinates」がタグ名
    
---


    2.md→mdの場合
    
    ```md
     openGLのmyopenglwiget.cppの座標変換に関しては [こちら](coordinate_transformation1.md) を参照してください。
    ```
    

---
    3. コードのコメントの変数→md
    
```
詳しくは @ref MyOpenGLWidget_camera::m_filmTexture "フィルムテクスチャ" を
ご覧ください。
```
    
    
注意クラス名があってる確認
```
class MyOpenGLWidget_camera {
    ...
    GLuint m_filmTexture;
};

```
---
    4. コードのコメントの変数→コード

```
/**
 * 詳しくは @ref MyOpenGLWidget_camera::m_filmTexture "フィルムテクスチャ" をご覧ください。
 */

```




---

```cpp
int main() {
    // test
}

```





