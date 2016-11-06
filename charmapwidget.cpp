#include "charmapwidget.h"
#include <QMouseEvent>
#include <QToolTip>
#include <QPainter>

#include <stdint.h>
#include <iostream>

charmapWidget::charmapWidget(QWidget *parent) : QWidget(parent)
{
    /*DPIaware scale factor */
    if (logicalDpiX()<100)
        m_scale = 2;
    else
        m_scale = 4;
    /* end of DPI */

    squareSize = 8*m_scale;
    columns = 16;
    lastKey = -1;
    setMouseTracking(true);
}

void charmapWidget::updateFont(unsigned char *fontdata, int len)
{
    for(int i=0; i<len; ++i)
        displayFont[i] = fontdata[i];
    //squareSize = qMax(24, QFontMetrics(displayFont).xHeight() * 3);
    adjustSize();
    update();
}

QSize charmapWidget::sizeHint() const
{
    return QSize(columns*squareSize, columns*squareSize);
}

void charmapWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint widgetPosition = mapFromGlobal(event->globalPos());
    uint key = (widgetPosition.y()/squareSize)*columns + widgetPosition.x()/squareSize;

//    QString text = QString::fromLatin1("<p>Character: <span style=\"font-size: 24pt; font-family: %1\">").arg("displayFont.family()")
//                  + QChar(key)
//                  + QString::fromLatin1("</span><p>Value: 0x")
//                  + QString::number(key, 16);
    QString text = QString::fromLatin1("Char: 0x")+ QString::number(key, 16);
    QToolTip::showText(event->globalPos(), text, this);
}

void charmapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastKey = (event->y()/squareSize)*columns + event->x()/squareSize;
        if (QChar(lastKey).category() != QChar::Other_NotAssigned)
            emit characterSelected(lastKey);
        update();
    }
    QWidget::mousePressEvent(event);
}


void charmapWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(Qt::white));

    QRect redrawRect = event->rect();
    int beginRow = redrawRect.top()/squareSize;
    int endRow = redrawRect.bottom()/squareSize;
    int beginColumn = redrawRect.left()/squareSize;
    int endColumn = redrawRect.right()/squareSize;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::black));
    for (int row = beginRow; row <= endRow; ++row) {

        for (int column = beginColumn; column <= endColumn; ++column) {

            int key = row*columns + column;
            painter.setClipRect(column*squareSize, row*squareSize, squareSize, squareSize);
            if (key == lastKey)
                painter.fillRect(column*squareSize + 1, row*squareSize + 1, squareSize, squareSize, QBrush(Qt::red));
            drawChar(painter,column*squareSize,row*squareSize,key);
        }
    }

    painter.setPen(QPen(Qt::gray));
    painter.setBrush(Qt::NoBrush);
    //painter.setBrush(QBrush(Qt::green));
    int w, h;
    for (int row = beginRow; row <= endRow; ++row) {
        for (int column = beginColumn; column <= endColumn; ++column) {
            painter.setClipRect(column*squareSize, row*squareSize, squareSize, squareSize);
            if (column!=endColumn)
                w = squareSize;
            else
                w = squareSize-1;
            if (row!=endRow)
                h = squareSize;
            else
                h = squareSize-1;
            painter.drawRect(column*squareSize, row*squareSize, w, h);
        }
    }
}

void charmapWidget::drawChar(QPainter &painter, int cx, int cy, int c)
{
    uint64_t cd = *((uint64_t *) (&displayFont[c*8]));
    int on = 0;
    for(int y=7; y>=0; --y)
        for (int x=0; x<8; ++x) {
            if (cd & ((uint64_t)1<<63)) {
                painter.setBrush(QBrush(Qt::black));
                //painter.setBrush(QBrush("#6c6cff"));
                //painter.setBrush(QBrush(m_fcolor));
                on = 1;
            } else {
                painter.setBrush(Qt::NoBrush);
                on = 0;
            }

            if (m_scale>8)
                painter.drawRect(x*m_scale,y*m_scale,m_scale-1,m_scale-1); //(10, 15, 90, 60);
            else
                if (m_scale ==1) {
                    if (on)
                        painter.drawPoint(cx+x*m_scale,cy+y*m_scale);
                } else {
                    if (on)
                        painter.drawRect(cx+x*m_scale,cy+y*m_scale,m_scale,m_scale);
                }

            cd <<=1;
        }
}
