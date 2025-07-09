#ifndef FILM_BUFFER_H
#define FILM_BUFFER_H


#include <Eigen/Dense>
#include <vector> // ポインタの代わりにvectorを使うとメモリ管理が楽になります

class FilmBuffer {
public:
    // コンストラクタ・デストラクタ
    FilmBuffer();
    ~FilmBuffer();

    // バッファの初期化（リサイズ）
    void init(int width, int height);

    // バッファのリセット
    void reset();

    // 【重要】ピクセルに計算結果（サンプル）を追加するメソッド
    void addSample(int i, int j, const Eigen::Vector3d& color);

    // 【重要】蓄積バッファから平均を計算し、フィルムバッファを更新するメソッド
    void updateFilmBuffer();

    // 描画用のフィルムバッファへのポインタを返すGetter
    const float* getFilmBufferPtr() const;

    // 現在のバッファの幅と高さを取得する
    int getWidth() const;
    int getHeight() const;

private:
    int m_width;
    int m_height;

    // 生ポインタの代わりにstd::vectorを使うと自動でメモリ解放してくれて安全です
    std::vector<float> m_filmBuffer;         // 表示用バッファ (平均化された色)
    std::vector<float> m_accumulationBuffer; // 色の合計値を蓄積
    std::vector<int> m_countBuffer;          // サンプル数をカウント
};

#endif // FILM_BUFFER_H
