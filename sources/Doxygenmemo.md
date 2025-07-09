## Doxygenメモ

- リンク渡す方法
    1. コード→mdファイルの場合
    
    ```cpp
     * 復習 @ref barycentric_coordinates "バリュートリック座標" を参照してください。
    ```
    
    ```md
     \anchor barycentric_coordinates
    ```
    ↑mdファイル内にこのタグの記述を入れておく「barycentric_coordinates」がタグ名
    

    2.md→mdの場合
    
    ```md
     openGLのmyopenglwiget.cppの座標変換に関しては [こちら](coordinate_transformation1.md) を参照してください。
    ```


```


```




---


