#ifndef CHARDISPLAY_H
#define CHARDISPLAY_H

#include <QWidget>
#include <stdint.h>
class chardisplay : public QWidget
{
    Q_OBJECT
    enum state {DRAW,ERASE};
    int m_scale;
    int m_charindex;
    uint64_t m_chardata;
    uint64_t m_copydata;
    bool m_copyvalid;
    state m_state;
    bool m_modified;
    unsigned char m_charset[2048];
    QColor m_fcolor;
    QColor m_bcolor;

    void plotLine(int x0, int y0, int x1, int y1, bool on);
    void plotCircle(int xm, int ym, int r, bool on);
    void charDrawPixel(int x, int y, bool on);
    void syncUIWithCharset(void);
    void flipDiagA1H8(void);

public:
    explicit chardisplay(QWidget *parent = 0);
    void clear(void);
    void setScale(int scale);
    void setCharIndex(int i);
    void setCharData(uchar *data, int len);
    void getCharData(uchar *data);

    void copy(void);
    void cut(void);
    void paste(void);

    void shiftRight(void);
    void shiftLeft(void);
    void shiftUp(void);
    void shiftDown(void);

    void rotateCCW(void);
    void rotateCW(void);

    void mirrorHorizontaly(void);
    void mirrorVerticaly(void);

    void invert(void);

    bool isModified() { return m_modified;}
    void setModified(bool val) { m_modified = val; }
    void setFColor(const QColor c);
    void setBColor(const QColor c);
signals:

public slots:

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *);
    QSize sizeHint();
};

#endif // CHARDISPLAY_H
