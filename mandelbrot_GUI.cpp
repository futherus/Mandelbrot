#include <chrono>
#include <QObject>

#include "mandelbrot_GUI.h"
#include "ui_mainwindow.h"
#include "compute.h"
#include "get_fps.h"

static const double SCREEN_ZOOM   = 1.5;
static const double SCREEN_SHIFT  = 0.1;
static const double SCREEN_SCALER = 5.0;

MandelbrotWidget::MandelbrotWidget(QWidget *parent)
    : QWidget(parent)
{
    enableAVX = true;

    raw_img.resize(4 * width() * height());

    fractal_center = QPointF(0, 0);
    fractal_size   = QSizeF (3, 2);

    connect(&updateTimer, SIGNAL(timeout()),
            this,         SLOT(update()));

    updateTimer.start(0);

    qDebug() << "Constructor. W" << width() << "H" << height();
}

void MandelbrotWidget::paintEvent(QPaintEvent*)
{
    qDebug() << "Painting. size(raw_img) = " << raw_img.size()
             << "fractal center" << fractal_center << "fractal size" << fractal_size;

    qDebug() << "AVX" << enableAVX << "FPS:" <<  get_fps();
    mandelbrot((uint32_t*) raw_img.data(), enableAVX,
                     fractal_center.x() - fractal_size.width()  / 2,
                     fractal_center.y() - fractal_size.height() / 2,
                     fractal_center.x() + fractal_size.width()  / 2,
                     fractal_center.y() + fractal_size.height() / 2,
                     width(), height()
                    );

    QImage img((uchar*) raw_img.data(), width(), height(), QImage::Format_RGB32);

    QPainter painter(this);
    painter.drawImage(QPoint(0,0), img);
}

void MandelbrotWidget::resizeEvent(QResizeEvent* event)
{
    raw_img.resize(4 * event->size().width() * event->size().height());
    qDebug() << "Resize event. W" << width() << "H" << height();
}

void MandelbrotWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Key pressed";
    bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
    switch(event->key())
    {
        case Qt::Key_Plus:
        {
            qDebug() << "'+'";
            fractal_size /= (shiftPressed ? SCREEN_ZOOM * SCREEN_SCALER : SCREEN_ZOOM);
            qDebug() << fractal_size;
            break;
        }
        case Qt::Key_Minus:
        {
            qDebug() << "'-'";
            fractal_size *= (shiftPressed ? SCREEN_ZOOM * SCREEN_SCALER : SCREEN_ZOOM);
            qDebug() << fractal_size;
            break;
        }
        case Qt::Key_Left:
        {
            qDebug() << "Left";
            double dx = fabs(fractal_size.width() * (shiftPressed ? SCREEN_SHIFT * SCREEN_SCALER : SCREEN_SHIFT));
            fractal_center.setX(fractal_center.x() - dx);
            qDebug() << fractal_center;
            break;

        }
        case Qt::Key_Right:
        {
            qDebug() << "Right";
            double dx = fabs(fractal_size.width() * (shiftPressed ? SCREEN_SHIFT * SCREEN_SCALER : SCREEN_SHIFT));
            fractal_center.setX(fractal_center.x() + dx);
            qDebug() << fractal_center;
            break;
        }
        case Qt::Key_Up:
        {
            qDebug() << "Up";
            double dy = fabs(fractal_size.height() * (shiftPressed ? SCREEN_SHIFT * SCREEN_SCALER : SCREEN_SHIFT));
            fractal_center.setY(fractal_center.y() - dy);
            qDebug() << fractal_center;
            break;

        }
        case Qt::Key_Down:
        {
            qDebug() << "Down";
            double dy = fabs(fractal_size.height() * (shiftPressed ? SCREEN_SHIFT * SCREEN_SCALER : SCREEN_SHIFT));
            fractal_center.setY(fractal_center.y() + dy);
            qDebug() << fractal_center;
            break;
        }
        case Qt::Key_A:
        {
            qDebug() << "A";
            enableAVX = !enableAVX;
        }
    }

    update();
}

MandelbrotWidget::~MandelbrotWidget()
{
}

