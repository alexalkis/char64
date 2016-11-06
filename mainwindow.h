#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    bool maybeSave(void);
    bool loadFile(const QString &fileName);
    void removeFromRecentFilesActions(QString &problem);
    void setCurrentFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void openRecentFile();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void on_pushButton_clicked();
    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void on_spinBox_valueChanged(const QString &arg1);
    void on_spinBox_2_valueChanged(int arg1);
    void on_actionShift_Right_triggered();
    void on_actionShift_Left_triggered();
    void on_actionShift_Up_triggered();
    void on_actionShift_Down_triggered();
    void on_actionMirror_Horizontaly_triggered();
    void on_actionMirror_Verticaly_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();

    //void on_comboBox_currentIndexChanged(int index);

    void on_actionRotate_CW_triggered();
    void on_actionRotate_CCW_triggered();
    void on_spinBox_2_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_2_currentIndexChanged(int );

    void on_pushButton_3_currentIndexChanged(int );

    void on_actionDefault_triggered();

    void on_actionUpper_Lower_case_triggered();

    void on_actionNew_triggered();

    void on_actionInvert_triggered();

    void on_actionAbout_triggered();

private:

    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    Ui::MainWindow *ui;
    QString curFile;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorAct,*separatorAct2;
};

#endif // MAINWINDOW_H
