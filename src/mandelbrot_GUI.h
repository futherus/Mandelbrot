#ifndef MANDELBROT_GUI_H
#define MANDELBROT_GUI_H

#include <QMainWindow>
#include <QtWidgets>

class MandelbrotWidget : public QWidget
{
    Q_OBJECT

public:
    MandelbrotWidget(QWidget *parent = nullptr);
    ~MandelbrotWidget();

protected:
    QTimer  updateTimer;
    QPointF fractal_center;
    QSizeF  fractal_size;
    QByteArray raw_img;
    QLabel* fps_lbl;
    bool    enableAVX;

    void paintEvent (QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent *event);

};

#endif // MANDELBROT_GUI_H
