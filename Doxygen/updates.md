# First Level Heading

Paragraph.

## アプデ

- RayTracingInternalData.hで構造体RayTracingInternalDataを分離
    * レイトレの内部状態などを整理した構造体として汎用性があるため
    * ray構造体も同様に分離
    * cameraのインスタンスはg_Camera2

- rayTracing 関数は別ファイルに切り出して RayTracer.cpp/h に置いた
    * 再利用性
- void initAreaLights()をlight.cpp二隔離


| ファイル                       | 役割                                                                              |
| -------------------------- | ------------------------------------------------------------------------------- |
| `RayTracer.cpp` / `.h`     | **交差判定**（`rayTracing`, `rayTriangleIntersect` など）                               |
| `Shading.cpp` / `.h`       | **放射輝度の計算=レンダリング方程式**（`computeShading`, `computeDirectLighting`, `computeReflection`, など） |
| `random.c`   | `randomMT()` などランダムサンプル関連                                                       |


- bullet
+ other bullet
* another bullet
    * child bullet

1. ordered
2. next ordered

### Third Level Heading

Some *italic* and **bold** text and `inline code`.

An empty line starts a new paragraph.

Use two spaces at the end  
to force a line break.

A horizontal ruler follows:

---

Add links inline like [this link to the Qt homepage](https://www.qt.io),
or with a reference like [this other link to the Qt homepage][1].

    Add code blocks with
    four spaces at the front.

> A blockquote
> starts with >
>
> and has the same paragraph rules as normal text.

First Level Heading in Alternate Style
======================================

Paragraph.

Second Level Heading in Alternate Style
---------------------------------------

Paragraph.

[1]: https://www.qt.io
