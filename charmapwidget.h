#ifndef CHARMAPWIDGET_H
#define CHARMAPWIDGET_H
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;

class charmapWidget : public QWidget
{
    Q_OBJECT

public:
    charmapWidget(QWidget *parent = 0);
    QSize sizeHint() const; //Q_DECL_OVERRIDE;

public slots:
    void updateFont(unsigned char *fontdata, int len);
    //void updateSize(const QString &fontSize);

signals:
    void characterSelected(const int index);

protected:
    void mouseMoveEvent(QMouseEvent *event);// Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event);// Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event);// Q_DECL_OVERRIDE;

private:
    void drawChar(QPainter &painter, int x, int y, int c);

    unsigned char displayFont[4096];
    int columns;
    int lastKey;
    int m_scale;
    int squareSize;
};
#endif // CHARMAPWIDGET_H
