#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL>
#include <QOpenGLFunctions>
#include <QObject>
#include <QOpenGLWidget>

class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    MyOpenGLWidget(QWidget *parent = nullptr);
    ~MyOpenGLWidget();

protected:
    // OpenGLの初期化
    void initializeGL() override;

    // OpenGLの描画
    void paintGL() override;

    // ウィジェットのリサイズ時に呼ばれる
    void resizeGL(int width, int height) override;

    // その他のメソッドや変数をここに追加
};

#endif // GLWIDGET_H
