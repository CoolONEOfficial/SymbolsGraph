#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <fstream>
#include <QFileDialog>
#include <math.h>
#include <QFontDatabase>
#include <QTimer>
#include <QMouseEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    // Clear
    for(int f = 0; f<256; f++)
    {
        mas[f] = 0;
        currentMas[f] = 0;
    }

    max = 0;

    ui->setupUi(this);

    setCursor(Qt::BlankCursor);

    setMouseTracking(true);

    scaleAnimation = new QPropertyAnimation(this, "scale");
    scaleAnimation->setEasingCurve(QEasingCurve::OutBack);
    scaleAnimation->setDuration(1000);
    scaleAnimation->setStartValue(0);
    scaleAnimation->setEndValue(1000);

    backgroundFont.setFamily(QFontDatabase::applicationFontFamilies(
                             QFontDatabase::addApplicationFont(
                             QApplication::applicationDirPath()+"//fonts//HACKED.ttf")).first());

    changeTimerId = startTimer(20);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::drawGraph(QPainter &p)
{
    p.setBrush(QBrush(QColor(100,100,100)));
    p.setPen(QPen(QColor(255,255,255)));

    if(max > 0)
    {
        float scaleX = width()/255.0;
        float scaleY = height() / max;

        for(int f = 0; f<255; f++)
        {
            float alfa = scale / 1000.0;

            float masVal = (1-alfa) * currentMas[f] + alfa * mas[f];
            float masValNext = (1-alfa) * currentMas[f+1] + alfa * mas[f+1];

            x1 = f*scaleX;
            y1 = height() - masVal * scaleY;
            x2 = (f+1.0) * scaleX;
            y2 = height() - masValNext * scaleY;

            masFill[0].setX(x1);
            masFill[0].setY(y1);

            masFill[1].setX(x2);
            masFill[1].setY(y2);

            masFill[2].setX(x2);
            masFill[2].setY(height());

            masFill[3].setX(x1);
            masFill[3].setY(height());

            if(y1 != height() && y2 != height())
                p.drawPolygon(masFill,4);

//            if(currentMas[f] != 0)
//            {
//                backgroundFont.setPixelSize((width()+height())/200);
//                p.setFont(backgroundFont);
//                p.drawText(QRect(x2-p.font().pixelSize(),y2-p.font().pixelSize()*2,p.font().pixelSize()*2,p.font().pixelSize()),Qt::AlignCenter,QString::number(currentMas[f]));
//            }
        }
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    // Open File
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("All files (*.*)"));

    if(!fileName.isNull())
    {
        setWindowTitle("SymbolsGraph - "+fileName);

        std::ifstream file;
        file.open(fileName.toStdString().c_str());

        // Clear
        for(int f = 0; f<256; f++)
        {
            currentMas[f] = mas[f];
            mas[f] = 0;
        }

        max = 0;

        do
        {
            unsigned char symbol;
            file>>symbol;
            mas[symbol]++;

            if (file.fail())
            {
                break;
            }
        }while(true);
        file.close();

        for(int f = 0; f<256; f++)
        {
            if (mas[f]>max)
                max = mas[f];
        }

        scale = 0;

        scaleAnimation->setStartValue(0);
        scaleAnimation->setEndValue(1000);
        scaleAnimation->stop();
        scaleAnimation->start();
    }

    update();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    x = event->x();
    y = event->y();
    update();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // Background
    p.setBrush(QColor(100,100,100));
    p.drawRect(-1,-1,width()+2,height()+2);
    // Title
    backgroundFont.setPixelSize((height()+width())/2/6);
    p.setPen(QColor(70,70,70));
    p.setFont(backgroundFont);
    p.drawText(QRect(0,height()/2-backgroundFont.pixelSize()/2,width(),backgroundFont.pixelSize()),
               Qt::AlignCenter,"SymbolsGraph");
    backgroundFont.setPixelSize((height()+width())/2/10);
    p.setFont(backgroundFont);
    p.drawText(QRect(0,height()/10*7-backgroundFont.pixelSize()/2,width(),backgroundFont.pixelSize()),
               Qt::AlignCenter,"Click to open file");
    p.setBrush(QColor(0,0,0));
    p.setRenderHints(QPainter::Antialiasing,true);

    drawGraph(p);

    // Draw ui
    p.setBrush(QColor(0,0,0,0));
    p.drawLine(0,y,width(),y);
    p.drawLine(x,0,x,height());
    backgroundFont.setPixelSize(15);
    p.setFont(backgroundFont);
    p.drawText(QRect(x-25-2,y-p.font().pixelSize(),25,25),
               Qt::AlignRight, QString::number(x/(width()/255)+1));
    p.drawText(QRect(x+2,y-p.font().pixelSize(),25,25),
               Qt::AlignLeft, QString::number(round( (height()-y)/(height()/max) )));

    if(max>0)
    {
        // Vertical
        for(int f = max; f>0; f--)
        {
            int y = height()-((height()/max)*f);

            if(f % 5 != 0)
            {
                p.drawLine(0,y,width()/200.0,y);
            }
            else
            {
                p.drawLine(0,y,width()/100.0,y);
                backgroundFont.setPixelSize(width()/100.0);
                p.setFont(backgroundFont);
                p.drawText(width()/100.0+p.font().pixelSize(),y+p.font().pixelSize()/3.0,QString::number(f));
            }
        }

        // Horizontal
        for(int f = 1; f<255; f++)
        {
            int x = (width()/255.0)*f;
            if(currentMas[f] == 0)
                p.drawLine(x,height(),x,height()-height()/150.0);
        }
    }
}
