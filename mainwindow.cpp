#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chardialog.h"
#include "charmapwidget.h"

#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>

//#include <QtWidgets>
#include <iostream>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    createStatusBar();

    readSettings();

//    int dpiX = qApp->desktop()->logicalDpiX();
//    std::cout <<"DPI: "<< dpiX <<  " From QWidget: " << this->logicalDpiX() << std::endl;

    ui->displayChar->setScale(8);
    ui->spinBox->setValue(50);
    setWindowTitle(tr("Char64"));
    setWindowIcon(QIcon(":/images/app.png"));
    curFile="";
    ui->pushButton_2->setColorIndex(14);
    ui->pushButton_3->setColorIndex(6);

    // Ok, we want to add the "Recent files" section...we keep inserting before the 'Quit'
    // of the File menu
    separatorAct = ui->menuFile->insertSeparator(ui->menuFile->actions().at(ui->menuFile->actions().length()-1));
    for (int i = 0; i < MaxRecentFiles; ++i) {
            recentFileActs[i] = new QAction(this);
            recentFileActs[i]->setVisible(false);
            connect(recentFileActs[i], SIGNAL(triggered()),
                    this, SLOT(openRecentFile()));
            ui->menuFile->insertAction(
                        ui->menuFile->actions().at(ui->menuFile->actions().length()-1),
                        recentFileActs[i]
                        );
        }
    separatorAct2 = ui->menuFile->insertSeparator(ui->menuFile->actions().at(ui->menuFile->actions().length()-1));
    updateRecentFileActions();

    ui->pushButton->setVisible(false);
    ui->checkBox->setVisible(false);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::writeSettings()
{
    QSettings settings("Retrosoft", "Char64");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("splitterSizes",ui->splitter->saveState());
    /*
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    */
}

void MainWindow::readSettings()
{
    QSettings settings("Retrosoft", "Char64");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    ui->splitter->restoreState(settings.value("splitterSizes").toByteArray());
    resize(size);
    move(pos);
    /*
    restoreGeometry(settings.value("myWidget/geometry").toByteArray());
    restoreState(settings.value("myWidget/windowState").toByteArray());
    */
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;
    QString filename = action->data().toString();
    if (loadFile(filename)==false)
        removeFromRecentFilesActions(filename);
            //action->setVisible(false);
}
void MainWindow::on_pushButton_clicked()
{
    char foo[4096];
    std::cout << "Ok clicked!\n";
    statusBar()->showMessage(tr("Ok clicked"), 2000);

    QFile file("chargen");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Meh, couldn't found 'chargen' file...No C64 rom fonts for you!\n";
        return;
    }
    qint64 ret = file.read(foo,4096);
    file.close();

    std::cout << ret << "\n";
    ui->displayChar->setCharData((unsigned char *)foo, ret);

}

bool MainWindow::loadFile(const QString &fileName)
{
    //std::cout << "Will try to load: " << fileName.toAscii().data() << std::endl;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Char64"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }
    //QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->actionPaste->setEnabled(false);
    char data[2048];
    file.read(data,2048);
    ui->displayChar->setCharData((uchar*)data, 2048);
    file.close();
    //textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    setCurrentFile(fileName);
    ui->spinBox_2->setValue(0);
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->displayChar->setModified(false);

    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled";
    setWindowFilePath(shownName);
    setWindowTitle(shownName + " - Char64");

    QSettings settings("Retrosoft", "Char64");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::removeFromRecentFilesActions(QString &problem)
{
    QSettings settings("Retrosoft", "Char64");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(problem);
    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("Retrosoft", "Char64");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

bool MainWindow::maybeSave(void)
{
    //should request chardisplay if it has any changes
    if (!ui->displayChar->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    //QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    //out << textEdit->toPlainText();
    unsigned char data[2048];
    ui->displayChar->getCharData(data);
    file.write((char *)data,2048);
    file.close();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
void MainWindow::on_actionOpen_triggered()
{
    QFileDialog fileDlg;
    if (maybeSave()) {
        //QString fileName = QFileDialog::getOpenFileName(this);
        //fileDlg.setOption(QFileDialog::DontUseNativeDialog,true);
        //fileDlg.layout()->addWidget(new charmapWidget);
        QString fileName = fileDlg.getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{
    ui->displayChar->setScale(arg1.toInt());
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    ui->displayChar->setCharIndex(arg1);
}

void MainWindow::on_actionShift_Right_triggered()
{
    ui->displayChar->shiftRight();
}

void MainWindow::on_actionShift_Left_triggered()
{
    ui->displayChar->shiftLeft();
}

void MainWindow::on_actionShift_Up_triggered()
{
    ui->displayChar->shiftUp();
}

void MainWindow::on_actionShift_Down_triggered()
{
    ui->displayChar->shiftDown();
}

void MainWindow::on_actionMirror_Horizontaly_triggered()
{
    ui->displayChar->mirrorHorizontaly();
}

void MainWindow::on_actionMirror_Verticaly_triggered()
{
    ui->displayChar->mirrorVerticaly();
}

void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSave_As_triggered()
{
    saveAs();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->displayChar->copy();
}

void MainWindow::on_actionCut_triggered()
{
    ui->displayChar->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->displayChar->paste();
}

//void MainWindow::on_comboBox_currentIndexChanged(int index)
//{
//    ui->displayChar->setFColor(ui->comboBoxF->color());
//}

void MainWindow::on_actionRotate_CW_triggered()
{
    ui->displayChar->rotateCW();
}

void MainWindow::on_actionRotate_CCW_triggered()
{
    ui->displayChar->rotateCCW();
}

void MainWindow::on_spinBox_2_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    charDialog mainDialog(this);
    mainDialog.setWindowTitle(tr("Char selector"));
    mainDialog.move(QCursor::pos());

    unsigned char foo[2048];
    ui->displayChar->getCharData(foo);

    mainDialog.setFontData((unsigned char *)foo, 2048);
    mainDialog.adjustSize();
    QSize dsize = mainDialog.size();
    mainDialog.setMinimumSize(dsize);
    mainDialog.setMaximumSize(dsize);
    int ret = mainDialog.exec();
    if (ret) {
        ui->displayChar->setCharIndex(ret-1);
        ui->spinBox_2->setValue(ret-1);
    }
}

void MainWindow::on_pushButton_2_currentIndexChanged(int )
{
    ui->displayChar->setFColor(ui->pushButton_2->color());
}

void MainWindow::on_pushButton_3_currentIndexChanged(int )
{
    ui->displayChar->setBColor(ui->pushButton_3->color());
}

void MainWindow::on_actionDefault_triggered()
{
    char foo[4096];

    statusBar()->showMessage(tr("Loading default rom font"), 2000);

    QFile file(":/fonts/chargen");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Meh, couldn't found 'chargen' file...No C64 rom fonts for you!\n";
        return;
    }
    /*qint64 ret = */ (void) file.read(foo,4096);
    file.close();

    ui->displayChar->setCharData((unsigned char *)foo, 2048);
}

void MainWindow::on_actionUpper_Lower_case_triggered()
{
    char foo[4096];

    statusBar()->showMessage(tr("Loading default rom font"), 2000);

    QFile file(":/fonts/chargen");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Meh, couldn't found 'chargen' file...No C64 rom fonts for you!\n";
        return;
    }
    /*qint64 ret =*/(void) file.read(foo,4096);
    file.close();

    ui->displayChar->setCharData((unsigned char *)foo+2048, 2048);
}

void MainWindow::on_actionNew_triggered()
{
    if (maybeSave()) {
        ui->displayChar->clear();
        setCurrentFile("");
    }
}

void MainWindow::on_actionInvert_triggered()
{
    ui->displayChar->invert();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"Char64",QString("A simple character editor to edit fonts for the Commodore 64.\nWritten by Alex Argiropoulos.\n\n\nPre-alpha built. (Build date: " __DATE__ ")"));
}
