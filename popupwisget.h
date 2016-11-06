#ifndef POPUPWISGET_H
#define POPUPWISGET_H

#include <QWidget>

class popupWisget : public QWidget
{
    Q_OBJECT
public:
    explicit popupWisget(QWidget *parent = 0);
    void loadPallete(std::vector<QColor> colors);
    void setTiles(int nx, int ny, int tileSize);
signals:

public slots:

signals:
    void popColor(const int index);

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint();
    void mousePressEvent(QMouseEvent * event);
    void leaveEvent(QEvent * event);
private:
    int m_nx;
    int m_ny;
    int m_tileSize;

    std::vector<QColor> m_colors;
};

#endif // POPUPWISGET_H
