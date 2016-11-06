#ifndef PALETTEBUTTON_H
#define PALETTEBUTTON_H

#include <QObject>
#include <QPushButton>
#include <vector>

class QMouseEvent;
class popupWisget;
class palettebutton : public QPushButton
{
    Q_OBJECT
public:
    explicit palettebutton(QWidget *parent=0);
    QColor color(void);
    void setColorIndex(int i);
protected:
    void mousePressEvent(QMouseEvent *event);// Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event);// Q_DECL_OVERRIDE;
public slots:
    void popColor(const int c);

signals:
    void currentIndexChanged(const int index);
private:
    void click(void);
    std::vector<QColor> colors;
    int m_index;
    popupWisget *m_popup;
};

#endif // PALETTEBUTTON_H
