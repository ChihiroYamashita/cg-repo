@page additional_info_coding モンテカルロ法をコーディングに落とし込む


コード内で積分の処理を実際に行っている箇所は、「ランダムサンプリングとその結果の累積」によって、積分を近似している部分です。積分自体は、解析的に直接計算されるのではなく、モンテカルロ積分によって数値的に近似されています。

以下にコード内で積分がどのように近似されているかを解説します。

---

### **数式における積分**
面光源からの光の寄与を計算する積分は次のような形式です：



\f[
L_d(\mathbf{x}, \mathbf{\omega}_o) = \int_A \frac{\mathbf{L}(p, -\mathbf{\omega}_i) \cdot \mathbf{f}_r(\mathbf{\omega}_i, \mathbf{\omega}_o, \mathbf{x}) \cdot \cos \theta_i \cdot \cos \theta_l}{\|\mathbf{x} - p\|^2} \, \mathrm{d}A(p)
\f]

ここでは、エリアライトの面積  \f$A \f$ 上で積分を行っています。


---

### **コード内で積分を近似している部分**
コード全体で積分を近似しているのは、以下の部分です：

#### **1. サンプリングによる積分の分解**
```cpp
const Eigen::Vector3d p_light = sampleRandomPoint( in_AreaLights[i] );
```

- **`sampleRandomPoint`**:
  - 面光源（エリアライト）からランダムな点 \f$ p \f$ をサンプリングしています。
  - これにより、積分の微小面積要素 \f$mathrm{d}A \f$ をランダムな点に置き換えます。

---

#### **2. サンプリング点ごとの寄与計算**
サンプリングした点 \f$ p \f$ に基づいて、寄与を計算する部分です。

```cpp
const double cos_theta = std::max<double>( 0.0, w_L.dot( in_n ) );
direct_light_contribution += area * in_AreaLights[i].color.cwiseProduct( in_Material.kd ) * 
                             in_AreaLights[i].intensity * cos_theta * cosT_l / ( M_PI * dist * dist );
```

- **寄与の計算**:
  - 面光源上のランダムな点 \f$p \f$ における寄与を、面積\f$mathrm{d}A\f$ に基づいて計算。
  - サンプリング点 \f$ p \f$ の寄与を式としてモデル化。

---

#### **3. サンプリング点の寄与を累積**
以下のコード部分で、すべてのサンプリング点からの寄与を合計しています：

```cpp
for( int i=0; i<in_AreaLights.size(); i++ )
```

- 各サンプリング点で計算した光の寄与を `direct_light_contribution` に足し合わせています。
- これにより、面光源全体での寄与が近似的に求められます。
- in_AreaLights.size()はエリアライトの数。この場合は二個しか無いので二回しかランダムサンプリングさしない。

---

### **積分の近似方法：モンテカルロ法**
モンテカルロ積分をコードで近似的に計算している流れは次のようになります：

1. **サンプリング**:
   - `sampleRandomPoint` 関数でエリアライト \f$ A \f$ 上の点をランダムに選択。
2. **寄与の計算**:
   - 各サンプリング点で光の寄与を計算。
3. **累積和**:
   - サンプリング点の寄与を全て合計。
   - サンプリング点数が多いほど積分の精度が向上。

---

### **積分の結果としての近似式**
積分の計算をコードに基づいて近似的に表現すると、次のようになります：

\f[
L_d(\mathbf{x}, \mathbf{\omega}_o) \approx \sum_{i=1}^N \frac{\mathbf{L}(p_i, -\mathbf{\omega}_i) \cdot \mathbf{f}_r(\mathbf{\omega}_i, \mathbf{\omega}_o, \mathbf{x}) \cdot \cos \theta_i \cdot \cos \theta_l}{\|\mathbf{x} - p_i\|^2} \cdot \frac{\mathrm{Area}}{N}
\f]

ここで：
- \f$ N \f$: サンプリング数（エリアライト内の点のサンプリング回数）。
- \f$ p_i \f$: サンプリングされたエリアライトの点。
- \f$ \mathrm{Area} \f$: エリアライト全体の面積。

---

### **コード内での積分の具体例**
例えば、エリアライトが 10 回サンプリングされる場合：
1. **10 個のサンプリング点 \f$ p \f$** を `sampleRandomPoint` で生成。
2. 各点 \f$ p \f$ で光の寄与を計算し、`direct_light_contribution` に加算。
3. 全体の累積結果がエリアライト全体の寄与の近似値になります。

---

### **まとめ**
コード内で積分が直接的に計算されているわけではなく、ランダムサンプリングと寄与の累積を用いてモンテカルロ積分として近似的に計算しています。

- **積分処理の要点**:
  1. ランダムにサンプリングすることで積分領域を近似。
  2. 各サンプリング点で寄与を計算。
  3. 全体の寄与を合計して積分値を近似的に得る。