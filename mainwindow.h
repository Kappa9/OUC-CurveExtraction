#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QColor>
#include <QPainter>
#include <QList>
#include <QPoint>
#include <QUndoStack>
#include <QStackedWidget>
#include <vector>
#include <fstream>
#include <pic.h>
#include <getcolor.h>
#include <getcurve.h>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectPicButton_clicked();

    void on_nextButton_clicked();

    void on_autoAdjustRadioButton_toggled(bool checked);

    void on_manualAdjustRadioButton_toggled(bool checked);

    void on_picLabel_clicked();

    void on_picPathText_returnPressed();

    void on_stackedWidget_currentChanged(int page);

    void on_previousPageButton_clicked();

    void on_outputButton_clicked();

    void on_coloredCurveCheckBox_stateChanged(int check);

    void on_autoChooseRadioButton_toggled(bool checked);

    void on_manualChooseRadioButton_toggled(bool checked);

    void on_colorLabel_clicked();

    void on_chooseColorLabel_clicked();

    void on_picLabel_2_clicked();

    void on_undoButton_clicked();

    void on_redoButton_clicked();

    void on_resetButton_clicked();
private:
    Ui::MainWindow *ui;
    Pic pic;
    Getcolor getColor;
    GetCurve getCurve;
    QUndoStack extractUndoStack;
    void setPage(int page);
    void setInputPicWidgetsState(bool state);
    void setExtractPicWidgetsState(bool state);
    void changePic(QString &fileName);
    void initInput();
    void sendInputAxis(QList<QPoint> list);
    void initializeTable();
    void addTableItem(QPoint p);
    void removeTableItem();
    void updateTable();
    void updateUndoButtons();
    void initColorPicker();
};
#endif // MAINWINDOW_H
