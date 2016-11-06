#include "palettebutton.h"
#include "popupwisget.h"
#include <QMouseEvent>

#include <QThread>

#include <iostream>

palettebutton::palettebutton(QWidget *parent)
    :QPushButton(parent)
{
    QPalette pal = palette();


    colors.push_back(QColor(  0,  0,  0));
    colors.push_back(QColor(255,255,255));
    colors.push_back(QColor(136,  0,  0));
    colors.push_back(QColor(170,255,238));
    colors.push_back(QColor(204, 68,204));
    colors.push_back(QColor(  0,204, 85));
    colors.push_back(QColor(  0,  0,170));
    colors.push_back(QColor(238,238,119));
    colors.push_back(QColor(221,136, 85));
    colors.push_back(QColor(102, 68,  0));
    colors.push_back(QColor(255,119,119));
    colors.push_back(QColor( 51, 51, 51));
    colors.push_back(QColor(119,119,119));
    colors.push_back(QColor(170,255,102));
    colors.push_back(QColor(  0,136,255));
    colors.push_back(QColor(187,187,187));
    m_index = 0;

    pal.setColor(QPalette::Button, colors[m_index]);
    setAutoFillBackground(true);
    setPalette(pal);
    //setMouseTracking(true);
    update();
    m_popup = NULL;
}

void palettebutton::wheelEvent(QWheelEvent * event)
{
    int old = m_index;
    QPalette pal = palette();

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (event->delta()>0 && m_index<15) {
        ++m_index;
    }
    if (event->delta()<0 && m_index>0) {
        --m_index;
    }
#else
    if (event->angleDelta().y()>0 && m_index<15) {
        ++m_index;
    }
    if (event->angleDelta().y()<0 && m_index>0) {
        --m_index;
    }
#endif

    if (m_index!=old) {
        pal.setColor(QPalette::Button, colors[m_index]);
        setPalette(pal);
        emit currentIndexChanged(m_index);
    }
    //std::cout << "Got mouse-event: "<< event->angleDelta().y() << std::endl;
    QPushButton::wheelEvent(event);
}

void palettebutton::mousePressEvent(QMouseEvent * event)
{

    if (event->button() == Qt::LeftButton) {
        click();
        update();
    }
    else
        QPushButton::mousePressEvent(event);

}

void palettebutton::click(void)
{
    if (!m_popup) {
        m_popup = new popupWisget(this);

        m_popup->loadPallete(colors);
        m_popup->setTiles(8,2,32);
        m_popup->setWindowFlags( Qt::Tool| Qt::CustomizeWindowHint| Qt::FramelessWindowHint);
        m_popup->move(QCursor::pos().x()-16,QCursor::pos().y()-16);
        //boo.setGeometry(QCursor::pos().x(),QCursor::pos().y(),8*32,2*32);
        m_popup->setVisible(true);
        //QThread::sleep(2);
        connect(m_popup,SIGNAL(popColor(const int)),this,SLOT(popColor(const int)));
    }
}

void palettebutton::popColor(const int c)
{
    m_popup->deleteLater();
    m_popup = NULL;
    if (c!=-1 && c!=m_index) {
        QPalette pal = palette();
        pal.setColor(QPalette::Button, colors[c]);
        setPalette(pal);
        m_index = c;
        //std::cout << "Popup new color index: " << c << std::endl;
        emit currentIndexChanged(m_index);
    }
}

QColor palettebutton::color(void)
{
    return colors[m_index];
}

void palettebutton::setColorIndex(int i)
{
    m_index = i;
    QPalette pal = palette();
    pal.setColor(QPalette::Button, colors[i]);
    setPalette(pal);
    emit currentIndexChanged(m_index);
}
