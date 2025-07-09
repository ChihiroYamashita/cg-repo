#include "film_buffer.h"

FilmBuffer::FilmBuffer() : m_width(0), m_height(0) {}

FilmBuffer::~FilmBuffer() {
    // std::vectorを使っているので、デストラクタで明示的にdelete[]する必要はありません
}

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
