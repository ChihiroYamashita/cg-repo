#include "film_buffer.h"

FilmBuffer::FilmBuffer() : m_width(0), m_height(0) {}

FilmBuffer::~FilmBuffer() {
    // std::vectorを使っているので、デストラクタで明示的にdelete[]する必要はありません
}
/**
 * @brief フィルムバッファを指定サイズで初期化する関数
 * @details 復習 @ref about_buffer "バッファとは" を参照してください。
 * この関数は、フィルムバッファ（平均色）、アキュムレーションバッファ（積算値）、カウントバッファ（サンプル数）を
 * 与えられた解像度でリサイズし、すべての要素を初期値（0）でクリアします。
 *
 * @param[in] width  フィルムの横幅（ピクセル単位）
 * @param[in] height フィルムの縦幅（ピクセル単位）
 *
 * @details **処理の流れ**:
 *
 * #### 1. メンバ変数の設定
 * - 与えられた width, height をクラスのメンバに格納します。
 *
 * #### 2. バッファの初期化
 * - `std::vector::resize()` を使って、各バッファを動的にリサイズし、初期値で埋めます。
 *   - `m_filmBuffer`: 平均色を格納（float × 3 チャンネル）
 *   - `m_accumulationBuffer`: サンプルの積算値を格納（float × 3）
 *   - `m_countBuffer`: サンプル数（int）を格納
 *
 * @code
 * FilmBuffer buffer;
 * buffer.init(800, 600); // 800x600のバッファを準備
 * @endcode
 *
 * @see FilmBuffer::reset()
 * @see FilmBuffer::addSample()
 * @see FilmBuffer::updateFilmBuffer()
 *
 * @startuml film_buffer_init
 * class FilmBuffer {
 *   - int m_width
 *   - int m_height
 *   - std::vector<float> m_filmBuffer
 *   - std::vector<float> m_accumulationBuffer
 *   - std::vector<int> m_countBuffer
 *   --
 *   + void init(int width, int height)
 *   + void reset()
 *   + void addSample(int i, int j, const Eigen::Vector3d& color)
 *   + void updateFilmBuffer()
 * }
 * FilmBuffer -> m_filmBuffer : resize()
 * FilmBuffer -> m_accumulationBuffer : resize()
 * FilmBuffer -> m_countBuffer : resize()
 * @enduml
 */
void FilmBuffer::init(int width, int height) {
    m_width = width;
    m_height = height;

    // 指定されたサイズで各バッファをリサイズし、0で初期化
    m_filmBuffer.resize(width * height * 3, 0.0f);
    m_accumulationBuffer.resize(width * height * 3, 0.0f);
    m_countBuffer.resize(width * height, 0);
}

void FilmBuffer::reset() {
    // バッファの内容をすべて0にクリア
    std::fill(m_accumulationBuffer.begin(), m_accumulationBuffer.end(), 0.0f);
    std::fill(m_countBuffer.begin(), m_countBuffer.end(), 0);
}

// これが g_FilmBuffer[...] = ... の代わりになります
/**
 * @brief 指定された位置に1つの色サンプルを追加します。
 * @details ピクセルの位置 (i, j) に対して、渡された色をバッファに加えます。
 * 追加された色は、あとで平均化されて最終画像になります。
 * この関数は、**プログレッシブレンダリング**でよく使われる「色の積み重ね」を行う処理です。
 *
 * @param[in] i 横方向のピクセル位置（0 ～ 幅-1）
 * @param[in] j 縦方向のピクセル位置（0 ～ 高さ-1）
 * @param[in] color 追加する色（赤、緑、青の値を持つベクトル。0.0～1.0の範囲）
 *
 * @note 座標が画面の外（範囲外）の場合は何もしません。
 *
 * @details **処理のステップ**
 * 1. 座標 (i, j) が範囲内かチェックします。
 * 2. バッファ上の対応する位置を1次元インデックスに変換します。
 * 3. その位置に、色（R/G/B）をそれぞれ加算します。
 * 4. そのピクセルに加えたサンプル数（カウント）を1つ増やします。
 *
 * @code
 * Eigen::Vector3d color(0.3, 0.6, 0.9);  // 薄い青色
 * buffer.addSample(100, 50, color);      // 画面の(100, 50)番目のピクセルに色を追加
 * @endcode
 *
 * @see FilmBuffer::init()
 * @see FilmBuffer::updateFilmBuffer()
 * @see FilmBuffer::reset()
 *
 * @startuml add_sample_simple
 * class FilmBuffer {
 *   + void addSample(int i, int j, const Eigen::Vector3d& color)
 * }
 * FilmBuffer -> m_accumulationBuffer : 色を加算
 * FilmBuffer -> m_countBuffer : サンプル数を+1
 * @enduml
 */
void FilmBuffer::addSample(int i, int j, const Eigen::Vector3d& color) {
    if (i < 0 || i >= m_width || j < 0 || j >= m_height) return;

    // 1次元配列のインデックスを計算
    int index = (j * m_width + i);
    int bufferIndex = index * 3;

    // 蓄積バッファに色を加算
    m_accumulationBuffer[bufferIndex + 0] += static_cast<float>(color.x());
    m_accumulationBuffer[bufferIndex + 1] += static_cast<float>(color.y());
    m_accumulationBuffer[bufferIndex + 2] += static_cast<float>(color.z());

    // カウントをインクリメント（今回は1サンプルずつなので+1）
    m_countBuffer[index]++;
}
/**
 * @brief フィルムバッファに最終的な色を計算して格納する関数
 * @details この関数は、ピクセルごとにサンプルされた色の合計（積算バッファ）を、
 * サンプル数（カウントバッファ）で割ることで平均色を求め、最終的な画像（フィルムバッファ）に格納します。
 *
 * この処理は、**プログレッシブレンダリング**において、各ピクセルに対して複数サンプルを取った後に
 * 色の平均を出すために必要です。
 *
 * @note OpenGLへの転送処理は行いません。バッファ更新のみを行います。
 *
 * @details **処理の流れ**:
 *
 * #### 1. 各ピクセルに対して繰り返す
 * - 蓄積されたRGB値（`m_accumulationBuffer`）をサンプル数（`m_countBuffer`）で割り、
 *   フィルムバッファ（`m_filmBuffer`）に平均色を保存します。
 * - サンプル数が0の場合は、そのピクセルは黒 (0,0,0) に初期化されます。
 *
 * @code
 * // サンプル追加後に平均色を計算
 * buffer.updateFilmBuffer();
 * @endcode
 *
 * #### 2. シーケンス図（処理の全体の流れ）
 * @startuml update_film
 * participant App as A
 * participant FilmBuffer as FB
 *
 * A -> FB: addSample(i, j, color)（複数回）
 * A -> FB: updateFilmBuffer()
 * loop すべてのピクセル
 *     FB -> FB: accumulationBuffer ÷ countBuffer → filmBuffer に保存
 * end
 * @enduml
 *
 * @see FilmBuffer::addSample()
 * @see FilmBuffer::init()
 * @see FilmBuffer::getFilmBufferPtr()
 */
void FilmBuffer::updateFilmBuffer() {
    // main.cppのupdateFilm()のロジックとほぼ同じ
    for (int i = 0; i < m_width * m_height; ++i) {
        if (m_countBuffer[i] > 0) {
            m_filmBuffer[i * 3 + 0] = m_accumulationBuffer[i * 3 + 0] / m_countBuffer[i];
            m_filmBuffer[i * 3 + 1] = m_accumulationBuffer[i * 3 + 1] / m_countBuffer[i];
            m_filmBuffer[i * 3 + 2] = m_accumulationBuffer[i * 3 + 2] / m_countBuffer[i];
        } else {
            m_filmBuffer[i * 3 + 0] = 0.0f;
            m_filmBuffer[i * 3 + 1] = 0.0f;
            m_filmBuffer[i * 3 + 2] = 0.0f;
        }
    }
}

const float* FilmBuffer::getFilmBufferPtr() const {
    // テクスチャに転送するために、バッファの先頭アドレスを返す
    return m_filmBuffer.data();
}

int FilmBuffer::getWidth() const { return m_width; }
int FilmBuffer::getHeight() const { return m_height; }
