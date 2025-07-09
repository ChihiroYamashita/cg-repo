#ifndef FILM_BUFFER_H
#define FILM_BUFFER_H

#include <Eigen/Dense>

class FilmBuffer {
public:
    // Constructor and Destructor
    FilmBuffer();
    ~FilmBuffer();

    // Public Methods
    void resize(int new_width, int new_height);
    void clear();
    void addSample(int i, int j, const Eigen::Vector3d& color);

    // Getters
    const float* getFilmData() const;
    int getWidth() const;
    int getHeight() const;

private:
    // Private Methods
    void releaseMemory();

    // Member Variables
    int g_width;
    int g_height;

    float* g_filmBuffer;         // 表示用バッファ (平均化された色)
    float* g_accumulationBuffer; // 色の合計値を蓄積
    int* g_countBuffer;        // サンプル数をカウント
};

#endif // FILM_BUFFER_H
