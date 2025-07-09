#include "film_buffer.h"
#include <cstring> // for memset
/*
FilmBuffer::FilmBuffer()
    : g_width(0), g_height(0),
    g_filmBuffer(nullptr),
    g_accumulationBuffer(nullptr),
    g_countBuffer(nullptr)
{
}

FilmBuffer::~FilmBuffer()
{
    releaseMemory();
}

*/
/**
 * @brief フィルムバッファの初期化を行う関数
 * @details 復習 @ref about_buffer "バッファとは" を参照してください。この関数は、フィルムバッファ、アキュムレーションバッファ、カウントバッファのメモリを確保し、
 * バッファを初期化します。また、OpenGLのテクスチャを生成し、初期設定を行います。
 *
 * @param[out] g_FilmBuffer 平均化されたピクセルの色を格納するバッファを確保
 * @param[out] g_AccumulationBuffer 色の積算結果を保持するバッファを確保
 * @param[out] g_CountBuffer 各ピクセルのサンプル数を保持するバッファを確保
 * @param[out] g_FilmTexture OpenGLのテクスチャオブジェクトを生成し、初期設定を適用
 *
 * @details **処理の流れ**:
 *
 * #### 1. メモリの確保
 * - フィルムバッファ、アキュムレーションバッファ、カウントバッファのメモリを動的に確保します。
 *
 * #### 2. バッファのリセット
 * - リセット処理を呼び出して、全てのバッファを初期化します。
 *
 * #### 3. OpenGLテクスチャの設定
 * - OpenGLのテクスチャを生成し、2Dテクスチャとして初期化。
 * - テクスチャのフィルタリング設定を適用（線形補間）。
 *
 * @code
 * void initFilm() {
 *   g_FilmBuffer = (float*)malloc(sizeof(float) * g_FilmWidth * g_FilmHeight * 3);
 *   g_AccumulationBuffer = (float*)malloc(sizeof(float) * g_FilmWidth * g_FilmHeight * 3);
 *   g_CountBuffer = (int*)malloc(sizeof(int) * g_FilmWidth * g_FilmHeight);
 *   resetFilm();
 *
 *   glGenTextures(1, &g_FilmTexture);
 *   glBindTexture(GL_TEXTURE_2D, g_FilmTexture);
 *
 *   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_FilmWidth, g_FilmHeight, 0, GL_RGB, GL_FLOAT, g_FilmBuffer);
 *
 *   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 *   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 * }
 * @endcode
 *@see
 *
 *
 * @see resetFilm
 * @see updateFilm
 */
/*
void initFilm()
{
    g_FilmBuffer = (float*)malloc( sizeof(float) * g_FilmWidth * g_FilmHeight * 3 );
    g_AccumulationBuffer = (float*)malloc( sizeof(float) * g_FilmWidth * g_FilmHeight * 3 );
    g_CountBuffer = (int*)malloc( sizeof(int) * g_FilmWidth * g_FilmHeight );
    resetFilm();

    glGenTextures( 1, &g_FilmTexture );
    glBindTexture( GL_TEXTURE_2D, g_FilmTexture );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, g_FilmWidth, g_FilmHeight, 0, GL_RGB, GL_FLOAT, g_FilmBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void FilmBuffer::releaseMemory()
{
    delete[] g_filmBuffer;
    delete[] g_accumulationBuffer;
    delete[] g_countBuffer;
    g_filmBuffer = nullptr;
    g_accumulationBuffer = nullptr;
    g_countBuffer = nullptr;
}

void FilmBuffer::resize(int new_width, int new_height)
{
    // If size is the same, do nothing.
    if (g_width == new_width && g_height == new_height) {
        return;
    }

    releaseMemory();

    g_width = new_width;
    g_height = new_height;

    if (g_width > 0 && g_height > 0) {
        int num_pixels = g_width * g_height;
        g_filmBuffer = new float[num_pixels * 3];
        g_accumulationBuffer = new float[num_pixels * 3];
        g_countBuffer = new int[num_pixels];
        clear();
    }
}

void FilmBuffer::clear()
{
    if (g_width > 0 && g_height > 0) {
        int num_pixels = g_width * g_height;
        memset(g_accumulationBuffer, 0, sizeof(float) * num_pixels * 3);
        memset(g_countBuffer, 0, sizeof(int) * num_pixels);
        memset(g_filmBuffer, 0, sizeof(float) * num_pixels * 3);
    }
}

void FilmBuffer::addSample(int i, int j, const Eigen::Vector3d& color)
{
    if (i < 0 || i >= g_width || j < 0 || j >= g_height) {
        return;
    }

    int index = (j * g_width + i);
    int index3 = index * 3;

    // Add sample to accumulation buffer
    g_accumulationBuffer[index3 + 0] += color.x();
    g_accumulationBuffer[index3 + 1] += color.y();
    g_accumulationBuffer[index3 + 2] += color.z();
    g_countBuffer[index]++;

    // Update film buffer with the new average
    float inv_count = 1.0f / g_countBuffer[index];
    g_filmBuffer[index3 + 0] = g_accumulationBuffer[index3 + 0] * inv_count;
    g_filmBuffer[index3 + 1] = g_accumulationBuffer[index3 + 1] * inv_count;
    g_filmBuffer[index3 + 2] = g_accumulationBuffer[index3 + 2] * inv_count;
}

const float* FilmBuffer::getFilmData() const
{
    return g_filmBuffer;
}

int FilmBuffer::getWidth() const
{
    return g_width;
}

int FilmBuffer::getHeight() const
{
    return g_height;
}
*/
