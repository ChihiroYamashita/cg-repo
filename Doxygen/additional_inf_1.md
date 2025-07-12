@page additional_inf_1 レイと三角形の交差計算の詳細

わかりやすく初学者向けに説明しますね！

---

### これは何？
`std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>` は、  
「`Eigen::Vector3d` という型を使った特別な配列（動的配列）」です。

ここで重要な部分を一つずつ簡単に説明します！

---

### 1. `std::vector<Eigen::Vector3d>` とは？
C++の `std::vector` は、**可変サイズの配列**です。

- **例**: `std::vector<int>` は整数のリスト。
- ここでは `std::vector<Eigen::Vector3d>` なので、**3次元のベクトル**のリストを作ります。

---

### 2. `Eigen::Vector3d` とは？
`Eigen` は数学計算用のライブラリです。その中の `Vector3d` は：

- **3次元のベクトル**を表します。
- 例: 「\[x, y, z\]」みたいな形のデータ。
- **用途**: 3Dグラフィックスや物理計算で座標や方向を表すのによく使います。

---

### 3. `Eigen::aligned_allocator<Eigen::Vector3d>` は何？
これは少しテクニカルですが、「データの置き方（アライメント）」を調整するためのものです。

#### どうして必要？
コンピュータが計算を速くするためには、データが特定の並び方でメモリに配置されている必要があります。  
例えば、16バイトごとにデータを揃えると、計算が速くなります。

普通の配列（`std::vector`）では、この配置がうまく揃わないことがあります。  
そのため、この **`Eigen::aligned_allocator`** を使って、正しく配置するようにしています。

---

### わかりやすい例

#### 普通の配列の場合：
あなたがノートを取るときに、どこに書いてもOKとする感じ。  
ただ、ランダムに書き始めると後で探しにくくなるし、整理も大変。

#### アライメント付きの場合：
ノートに「1ページ1テーマ」などのルールを決めて整理する感じ。  
これだと後で探しやすく、効率が良くなります！

---

### 全体の意味
`std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>` は、  
「3次元ベクトル（`Eigen::Vector3d`）を効率よく使うために、特別な整理整頓（アライメント）をした動的配列」です。

---

### 使い方の例
こんな感じで使えます：

```cpp
#include <Eigen/Core>
#include <vector>
#include <iostream>

int main() {
    // アライメント付きの動的配列を作成
    std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> vertices;

    // ベクトルを追加
    vertices.emplace_back(1.0, 0.0, 0.0); // 点 (1, 0, 0)
    vertices.emplace_back(0.0, 1.0, 0.0); // 点 (0, 1, 0)
    vertices.emplace_back(0.0, 0.0, 1.0); // 点 (0, 0, 1)

    // 配列の中身を表示
    for (const auto& v : vertices) {
        std::cout << "ベクトル: " << v.transpose() << std::endl;
    }

    return 0;
}
```

出力は：
```
ベクトル: 1 0 0
ベクトル: 0 1 0
ベクトル: 0 0 1
```

---

### まとめ
1. `std::vector` → **配列**。
2. `Eigen::Vector3d` → **3次元ベクトル**。
3. `Eigen::aligned_allocator` → **データを正しく整理するための道具**。

難しい部分（アライメント）は「効率よくするための仕組み」と覚えておけばOKです！