#include "popupwisget.h"
#include <QPainter>
#include <QMouseEvent>

//#include <iostream>
popupWisget::popupWisget(QWidget *parent) : QWidget(parent)
{

}

void popupWisget::loadPallete(std::vector<QColor> colors)
{
    for(uint i=0; i<colors.size(); ++i)
        m_colors.push_back(colors[i]);
}

void popupWisget::setTiles(int nx, int ny, int tileSize) {
    m_nx = nx;
    m_ny = ny;
    m_tileSize = tileSize;
    resize(nx*tileSize,ny*tileSize);
    update();
}

QSize popupWisget::sizeHint()
{
    return QSize(m_tileSize*m_nx,m_tileSize*m_ny);
}

void popupWisget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    int col = 0;
    for(int y=0; y<m_ny; ++y)
        for (int x=0; x<m_nx; ++x) {
            painter.setBrush(QBrush(m_colors[col++]));
            painter.drawRect(x*m_tileSize,y*m_tileSize,m_tileSize-1,m_tileSize-1);
        }
}

void popupWisget::mousePressEvent(QMouseEvent * event)
{
    int y = event->y() / m_tileSize;
    int x = event->x() / m_tileSize;
    //std::cout << "Color: " << y*m_nx+x << std::endl;
    emit popColor(y*m_nx+x);
    close();
}

void popupWisget::leaveEvent(QEvent * event)
{
    Q_UNUSED(event);
    //std::cout <<"leave event" << std::endl;
    emit popColor(-1);
    close();
}
