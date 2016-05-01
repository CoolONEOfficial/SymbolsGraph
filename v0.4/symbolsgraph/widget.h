#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <fstream>
#include <QPropertyAnimation>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

    char mas[256];
    int currentMas[256];
    QPoint masFill[4];
    float max;
    int x1,y1,x2,y2;
    QFont backgroundFont;
    int changeTimerId;
    int x,y;
    int scale;

    QPropertyAnimation *scaleAnimation;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void paintEvent(QPaintEvent *);
    void drawGraph(QPainter &p);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    void setScale(int newVal)
    {
        scale = newVal;
        update();
    }

    int getScale()
    {
        return scale;
    }

private slots:

private:
    Ui::Widget *ui;

    Q_PROPERTY(int scale READ getScale WRITE setScale);
};

#endif // WIDGET_H
