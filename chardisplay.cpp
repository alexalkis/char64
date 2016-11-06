#include "chardisplay.h"
#include <qpainter.h>
#include <QMouseEvent>

#include <iostream>
#include <math.h>
#include <QDebug>

chardisplay::chardisplay(QWidget *parent) : QWidget(parent)
{
    //std::cout << this->width() << "x"<< this->height() << "\n";
    m_chardata = 0;
    m_charindex = 0;
    m_copyvalid = false;
    m_modified = false;
    for(int i=0; i<2048; ++i)
        m_charset[i]=0;

    setAutoFillBackground ( true );
}

void chardisplay::clear(void)
{
    m_chardata = 0;
    m_charindex = 0;
    m_copyvalid = false;
    m_modified = false;
    for(int i=0; i<2048; ++i)
        m_charset[i]=0;
    update();
}

void chardisplay::copy(void)
{
    m_copyvalid = true;
    m_copydata = m_chardata;
}

void chardisplay::cut(void)
{
    m_copyvalid = true;
    m_copydata = m_chardata;
    m_chardata = 0;
    update();
}

void chardisplay::paste(void)
{
    m_chardata = m_copydata;
    update();
}

void chardisplay::setFColor(const QColor c)
{
    m_fcolor = c;
    update();
}

void chardisplay::setBColor(const QColor c)
{
    m_bcolor = c;
    QPalette p( palette() );
    p.setColor( QPalette::Background, m_bcolor);
    setPalette( p );
    update();
}
void chardisplay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    //static int counter = 0;
    //++counter;
    //std::cout << this->width() << "x"<< this->height() << "\n";
    //std::cout << "paintEvent called #"<<counter << " times\n";
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.setBrush(QBrush("#c56c00"));

    //painter.setPen(QColor(Qt::black));// setPen(QBrush("#000000"));
    painter.setPen(m_fcolor);

    uint64_t cd = m_chardata;
    int on = 0;
    for(int y=0; y<8; ++y)
        for (int x=0; x<8; ++x) {
            if (cd & ((uint64_t)1<<63)) {
                //painter.setBrush(QBrush("#6c6cff"));
                painter.setBrush(QBrush(m_fcolor));
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
                        painter.drawPoint(x*m_scale,y*m_scale);
                } else {
                    if (on)
                        painter.drawRect(x*m_scale,y*m_scale,m_scale-1,m_scale-1);
                }

            cd <<=1;
        }


}

void chardisplay::charDrawPixel(int x, int y, bool on)
{
    if (x<0 || x>7 || y<0 || y>7)
        return;

    x = 7-x;
    y = 7-y;
    uint64_t cd = 1;
    while (y--)
        cd <<= 8;
    while (x--)
        cd <<=1;
    if (on) {
        m_chardata |= cd;
    } else {
        m_chardata &= ~cd;
    }
    m_modified = true;
}


void chardisplay::plotLine(int x0, int y0, int x1, int y1, bool on)
{
   int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
   int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
   int err = dx+dy, e2; /* error value e_xy */

   for(;;){  /* loop */
      charDrawPixel(x0,y0,on);
      if (x0==x1 && y0==y1) break;
      e2 = 2*err;
      if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
      if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
   }
}

void chardisplay::plotCircle(int xm, int ym, int r, bool on)
{
   int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
   do {
      charDrawPixel(xm-x, ym+y, on); /*   I. Quadrant */
      charDrawPixel(xm-y, ym-x, on); /*  II. Quadrant */
      charDrawPixel(xm+x, ym-y, on); /* III. Quadrant */
      charDrawPixel(xm+y, ym+x, on); /*  IV. Quadrant */
      r = err;
      if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
   } while (x < 0);
}

void chardisplay::mousePressEvent(QMouseEvent * event)
{
    static int oldx=-1;
    static int oldy=-1;
    //std::cout << "X: " << event->x() << " Y: " << event->y() << " Button:" << event->button() << " Buttons:" << event->buttons() << "\n";
    int y = event->y() / m_scale;
    int x = event->x() / m_scale;


    if (event->modifiers() &Qt::ShiftModifier) {
        //std::cout << "Hey mouse with shift pressed!!!\n";
        if (oldx!=-1) {
            if (event->button()==Qt::LeftButton)
                plotLine(x,y,oldx,oldy, true);
            else
                plotLine(x,y,oldx,oldy, false);
        }
    }

    if (event->modifiers() &Qt::ControlModifier) {
        //std::cout << "Hey mouse with shift pressed!!!\n";
        if (oldx!=-1) {
            int r = sqrt((x-oldx)*(x-oldx)+(y-oldy)*(y-oldy));
            if (event->button()==Qt::LeftButton)
                plotCircle(oldx,oldy, r, true);
            else
                plotCircle(oldx,oldy, r, false);
        }
    }


    if (event->button()==Qt::LeftButton) {
        charDrawPixel(x,y,true);
        m_state = DRAW;
    } else {
        charDrawPixel(x,y,false);
        m_state = ERASE;
    }

    update();

    syncUIWithCharset();

    oldx = x;
    oldy = y;
}


void chardisplay::syncUIWithCharset(void)
{
    static uint64_t mask = 255;

    /* Mirror vertically */
    uint64_t t = 0;
    for (int i=0; i<8; ++i) {
        t<<=8;
        t |= m_chardata & mask;
        m_chardata >>=8;
    }
    m_chardata = t;

    unsigned char *cur = &m_charset[m_charindex*8];
    *((uint64_t *)cur) = m_chardata;

    /* Mirror vertically */
    t = 0;
    for (int i=0; i<8; ++i) {
        t<<=8;
        t |= m_chardata & mask;
        m_chardata >>=8;
    }
    m_chardata = t;
}

void chardisplay::mouseMoveEvent(QMouseEvent *event)
{
    //if (event->x()>=width())
    //  std::cout << "-X: " << event->x() << " Y: " << event->y()<< " Button:" << event->button() << " Buttons:" << event->buttons() << "\n";
    int y = 7-event->y() / m_scale;
    int x = 7-event->x() / m_scale;



    uint64_t cd = 1;
    while (y--)
        cd <<= 8;
    while (x--)
        cd <<=1;
    if (m_state == DRAW)
        m_chardata |= cd;
    else
        m_chardata &= ~cd;

    syncUIWithCharset();
    update();
}

void chardisplay::shiftRight(void)
{
   static uint64_t mask = ((uint64_t)1<<(7*8))|((uint64_t)1<<(6*8))|((uint64_t)1<<(5*8))|((uint64_t)1<<(4*8))|
            ((uint64_t)1<<(3*8))|((uint64_t)1<<(2*8))|((uint64_t)1<<(1*8))|(1);
   uint64_t t = m_chardata & mask;  // we save the rightmost bits
   m_chardata&=~t;                  // we erase them
   m_chardata>>=1;                  // we shift the whole thing right
   t <<=7;                          // shift the saved bits 7 positions left
   m_chardata|=t;                   // 'or' them with the shifted thing
   update();
}

void chardisplay::shiftLeft(void)
{
    static uint64_t mask = ((uint64_t)1<<(7*8+7))|((uint64_t)1<<(6*8+7))|((uint64_t)1<<(5*8+7))|((uint64_t)1<<(4*8+7))|
             ((uint64_t)1<<(3*8+7))|((uint64_t)1<<(2*8+7))|((uint64_t)1<<(1*8+7))|(1<<7);
    uint64_t t = m_chardata & mask;  // we save the leftmost bits
    m_chardata&=~t;                  // we erase them
    m_chardata<<=1;                  // we shift the whole thing left
    t >>=7;                          // shift the saved bits 7 positions right
    m_chardata|=t;                   // 'or' them with the shifted thing
    update();
}

void chardisplay::flipDiagA1H8(void) {
   uint64_t x = m_chardata;
   uint64_t t;
   const uint64_t k1 = 0x5500550055005500;
   const uint64_t k2 = 0x3333000033330000;
   const uint64_t k4 = 0x0f0f0f0f00000000;
   t  = k4 & (x ^ (x << 28));
   x ^=       t ^ (t >> 28) ;
   t  = k2 & (x ^ (x << 14));
   x ^=       t ^ (t >> 14) ;
   t  = k1 & (x ^ (x <<  7));
   x ^=       t ^ (t >>  7) ;
   m_chardata = x;
}

void chardisplay::rotateCCW(void)
{
    flipDiagA1H8();
    mirrorVerticaly();
    //update();
}

void chardisplay::rotateCW(void)
{
    rotateCCW();
    rotateCCW();
    rotateCCW();
}

void chardisplay::shiftUp(void)
{
    static uint64_t mask = ((uint64_t) 255) << (7*8);

    uint64_t t = m_chardata & mask;  // we save the uppermost bits

    m_chardata<<=8;                  // we shift the whole thing 'up'
    t >>=(7*8);                      // shift the saved bits 7 positions right
    m_chardata|=t;                   // 'or' them with the shifted thing
    update();
}

void chardisplay::shiftDown(void)
{
    static uint64_t mask = 255;

    uint64_t t = m_chardata & mask;  // we save the uppermost bits

    m_chardata>>=8;                  // we shift the whole thing 'down'
    t <<=(7*8);                      // shift the saved bits 7 positions left
    m_chardata|=t;                   // 'or' them with the shifted thing
    update();
}

void chardisplay::mirrorHorizontaly(void)
{
    static uint64_t mask = ((uint64_t)1<<(7*8))|((uint64_t)1<<(6*8))|((uint64_t)1<<(5*8))|((uint64_t)1<<(4*8))|
             ((uint64_t)1<<(3*8))|((uint64_t)1<<(2*8))|((uint64_t)1<<(1*8))|(1);
    uint64_t t=0;
    for (int i=0; i<8; ++i) {
        t<<=1;
        t |= m_chardata & mask;
        m_chardata &=~mask;
        m_chardata >>=1;

    }
    m_chardata = t;
    update();
}

void chardisplay::mirrorVerticaly(void)
{
    static uint64_t mask = 255;

    uint64_t t = 0;
    for (int i=0; i<8; ++i) {
        t<<=8;
        t |= m_chardata & mask;
        m_chardata >>=8;
    }
    m_chardata = t;
    update();
}

void chardisplay::invert(void)
{
    m_chardata ^= 0xffffffffffffffff;
    update();
}

void chardisplay::setScale(int scale)
{
    m_scale = scale;
    resize(sizeHint());
    update();
}

void chardisplay::setCharIndex(int i)
{
    unsigned char *cur = &m_charset[m_charindex*8];
    mirrorVerticaly();
    *((uint64_t *)cur) = m_chardata;

    m_charindex = i;
    m_chardata = 0;
    for (int j=0; j < 8; ++j) {
        m_chardata<<=8;
        m_chardata |= m_charset[i*8+j];
        //if (j<7) m_chardata <<=8;
    }
    update();
}

void chardisplay::setCharData(uchar *data, int len)
{
    Q_ASSERT(len<=2048);
    m_charindex = 0;
    m_chardata = 0;
    for (int i=0; i < 8; ++i) {
        m_chardata |= data[i];
        if (i<7) m_chardata <<=8;
    }
    update();
    for(int i=0; i<len; ++i)
        m_charset[i]=data[i];
}

void chardisplay::getCharData(uchar *data)
{
    for(int i=0; i<2048; ++i)
        data[i] = m_charset[i];
}

QSize chardisplay::sizeHint()
{
    return QSize(8*m_scale,8*m_scale);
}
