#ifndef CHARDIALOG_H
#define CHARDIALOG_H

#include <QDialog>

namespace Ui {
class charDialog;
}

class charDialog : public QDialog
{
    Q_OBJECT

public:
    explicit charDialog(QWidget *parent = 0);
    ~charDialog();

    void setFontData(unsigned char *data, int len);
public slots:
    void mydone(int n);
private:
    Ui::charDialog *ui;
};

#endif // CHARDIALOG_H
