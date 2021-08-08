#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setPage(0);
    // 设置禁止最大化
    Qt::WindowFlags flags;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    ui->xMinLineEdit->setValidator(new QDoubleValidator());
    ui->xMaxLineEdit->setValidator(new QDoubleValidator());
    ui->yMinLineEdit->setValidator(new QDoubleValidator());
    ui->yMaxLineEdit->setValidator(new QDoubleValidator());
    setInputPicWidgetsState(false);
    setExtractPicWidgetsState(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPage(int page)
{
    int width = 1060;
    int height = 512;
    switch (page) {
        case 0: width = 1060; break;
        case 1: width = 1280; break;
        default: qDebug()<<"错误的页码！"; return;
    }
    ui->stackedWidget->setCurrentIndex(page);
    setFixedSize(width,height); // 禁止改变窗口大小。
    this->resize(QSize(width,height));
}

void MainWindow::on_stackedWidget_currentChanged(int page)
{
    switch (page) {
    case 0:{
        break;
    }
    case 1:{
        pic.currentAdjustedPic = &pic.adjustedPic;

        ui->picLabel_2->resize(pic.originalPic.pixmapToShow.width(),pic.originalPic.pixmapToShow.height());
        int displayx = ( ui->picDisplayGBox_2->width() - ui->picLabel_2->width() ) / 2;
        int displayy = ( ui->picDisplayGBox_2->height() - ui->picLabel_2->height() ) / 2;
        ui->picLabel_2->move(displayx, displayy);
        ui->picLabel_2->setPixmap(pic.adjustedPic.pixmapToShow);

        ui->colorLabel->paintColor = pic.extractPic.curveColor;
        ui->colorLabel->update();

        initializeTable();
        on_resetButton_clicked();
        break;
    }
    default: return;
    }
}

void MainWindow::on_selectPicButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("选择图片"),
            pic.filePath,
            tr("图片文件(*.png *jpg *jpeg *bmp)"));
    QFileInfo fi = QFileInfo(fileName);
    pic.filePath = fi.absolutePath();
    ui->picPathText->setText(fileName);
    changePic(fileName);
}

void MainWindow::on_picPathText_returnPressed()
{
    QString fileName = ui->picPathText->text();
    changePic(fileName);
}

void MainWindow::changePic(QString &fileName){
    Mat img_input = cv::imread(cv::String(fileName.toLocal8Bit().toStdString()));
    if(img_input.empty()){
        QMessageBox::warning(this,"错误","图片打开失败!");
        if(!pic.originalPic.hasPic) setInputPicWidgetsState(false);
        return;
    }
    initInput();
    Mat temp;
    cv::cvtColor(img_input,temp,COLOR_BGR2RGB);
    pic.ConstructPicContent(pic.originalPic, temp, true);

    //ui->picDisplayGBox->resize(temp.cols,temp.rows);
    //ui->picLabel->resize(temp.cols,temp.rows);
    ui->picLabel->resize(pic.originalPic.pixmapToShow.width(),pic.originalPic.pixmapToShow.height());
    int displayx = ( ui->picDisplayGBox->width() - ui->picLabel->width() ) / 2;
    int displayy = ( ui->picDisplayGBox->height() - ui->picLabel->height() ) / 2;
    ui->picLabel->move(displayx, displayy);
    ui->picLabel->setPixmap(pic.originalPic.pixmapToShow);
}

void MainWindow::initInput()
{
    ui->picLabel->clearList();
    pic.inputPic.clearList();
    setInputPicWidgetsState(true);
}

void MainWindow::setInputPicWidgetsState(bool state)
{
    ui->autoAdjustRadioButton->setEnabled(state);
    ui->manualAdjustRadioButton->setEnabled(state);
    ui->xMinLineEdit->setEnabled(state);
    ui->xMaxLineEdit->setEnabled(state);
    ui->yMinLineEdit->setEnabled(state);
    ui->yMaxLineEdit->setEnabled(state);
    ui->nextButton->setEnabled(false);
}

void MainWindow::setExtractPicWidgetsState(bool state)
{
    ui->colorLabel->setEnabled(state);
    ui->chooseColorLabel->setEnabled(state);
}

void MainWindow::on_autoAdjustRadioButton_toggled(bool checked)
{
    pic.inputPic.autoAdjust = checked;
    if(checked){
        //跟后端对接
        pic.adjustedPic.mat = getCurve.AutoFix(pic.originalPic.mat);
        pic.ConstructPicContent(pic.autoAdjustPic, pic.adjustedPic.mat, true);
        pic.currentOriginalPic = &pic.autoAdjustPic;
        ui->nextButton->setEnabled(true);
    }
    else {
        pic.currentOriginalPic = &pic.originalPic;
    }
    ui->picLabel->setPixmap(pic.currentOriginalPic->pixmapToShow);
}

void MainWindow::on_manualAdjustRadioButton_toggled(bool checked)
{
    pic.inputPic.manualAdjust = checked;
    ui->picLabel->canDraw = checked;
    ui->picLabel->update();
    if(pic.inputPic.manualAdjust){
        initInput();
        QMessageBox::information(this,"提示","手动校正已启动!");
    }
}

void MainWindow::on_picLabel_clicked()
{
    if(!pic.inputPic.manualAdjust) return;
    QPoint relativeAxis = ui->picLabel->mapFromGlobal(QCursor::pos());
    if(!pic.inputPic.updateAxis(relativeAxis)) {
        ui->nextButton->setEnabled(true);
        return;
    }
    if(pic.inputPic.getLength() >= 3) ui->nextButton->setEnabled(true);
    ui->picLabel->addPoint(relativeAxis);
    qDebug() << relativeAxis;
}

void MainWindow::on_nextButton_clicked()
{
    pic.border.xmin = ui->xMinLineEdit->text().toFloat();
    pic.border.ymin = ui->yMinLineEdit->text().toFloat();
    if(ui->xMaxLineEdit->text().isEmpty()) pic.border.xmax = pic.currentOriginalPic->width;
    else pic.border.xmax = ui->xMaxLineEdit->text().toFloat();
    if(ui->yMaxLineEdit->text().isEmpty()) pic.border.ymax = pic.currentOriginalPic->height;
    else pic.border.ymax = ui->yMaxLineEdit->text().toFloat();
    if(pic.inputPic.autoAdjust){
        pic.adjustedPic.mat = pic.currentOriginalPic->mat;
        pic.ConstructPicContent(pic.adjustedPic, pic.adjustedPic.mat, true);
    }
    else{
        sendInputAxis(pic.inputPic.getList());
    }
    setPage(1);
}

void MainWindow::sendInputAxis(QList<QPoint> list)
{
    QList<QPoint> sendList = pic.resizeAxis(*pic.currentOriginalPic, list, false);
    Point2i lu(sendList[0].x(),sendList[0].y());
    Point2i lb(sendList[1].x(),sendList[1].y());
    Point2i rb(sendList[2].x(),sendList[2].y());
    Mat fixMat = getCurve.ManualFix(pic.originalPic.mat, lu, lb, rb);
    pic.ConstructPicContent(pic.adjustedPic, fixMat, true);
}

void MainWindow::initializeTable()
{
    ui->axisTableWidget->setColumnCount(1);
    ui->axisTableWidget->setRowCount(0);
    ui->axisTableWidget->setHorizontalHeaderLabels(QStringList()<<"坐标");
    ui->axisTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->axisTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    int colWidth = ui->axisTableWidget->width();
    ui->axisTableWidget->setColumnWidth(0,colWidth);
}

void MainWindow::addTableItem(QPoint p)
{
    int newRows = ui->axisTableWidget->rowCount() + 1;
    ui->axisTableWidget->setRowCount(newRows);
    QString axis = QString("(%1,%2)").arg(p.x()).arg(p.y());
    ui->axisTableWidget->setItem(newRows - 1,0,new QTableWidgetItem(axis));
}

void MainWindow::removeTableItem()
{
    if(ui->axisTableWidget->rowCount() > 0)
        ui->axisTableWidget->removeRow(ui->axisTableWidget->rowCount() - 1);
}

void MainWindow::updateTable()
{
    ui->axisTableWidget->setRowCount(pic.extractPic.getLength());
    QString axis;
    int i = 0;
    for(QPointF p : pic.extractPic.getListF()){
        axis = QString("(%1,%2)").arg(p.x()).arg(p.y());
        ui->axisTableWidget->setItem(i,0,new QTableWidgetItem(axis));
        i++;
    }
}

void MainWindow::updateUndoButtons()
{
    ui->undoButton->setEnabled(extractUndoStack.canUndo());
    ui->redoButton->setEnabled(extractUndoStack.canRedo());
    ui->resetButton->setEnabled(!extractUndoStack.isClean());
}

void MainWindow::on_previousPageButton_clicked()
{
    ui->axisTableWidget->clear();
    setPage(0);
}

void MainWindow::on_coloredCurveCheckBox_stateChanged(int checked)
{
    pic.extractPic.coloredCurve = checked;
    setExtractPicWidgetsState(checked);
    //ui->colorLabel->setEnabled(checked);
    //ui->chooseColorLabel->setEnabled(checked);
    //ui->autoChooseCheckBox->setEnabled(checked);
    if(!checked){
        ui->autoChooseRadioButton->setEnabled(true);
        ui->manualChooseRadioButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);
        pic.extractPic.pickColor = false;
        pic.currentAdjustedPic = &pic.adjustedPic;
        ui->picLabel_2->setPixmap(pic.currentAdjustedPic->pixmapToShow);
    }/*else{
        if(pic.singleColorPic.hasPic){
            pic.currentAdjustedPic = &pic.singleColorPic;
            ui->picLabel_2->setPixmap(pic.currentAdjustedPic->pixmapToShow);
        }
    }*/
}

void MainWindow::on_lineptmCheckBox_stateChanged(int checked)
{
    pic.extractPic.lineptm = checked;
    if(pic.extractPic.autoSelect){
        on_autoChooseRadioButton_toggled(true);
    }
}

void MainWindow::on_autoChooseRadioButton_toggled(bool checked)
{
    pic.extractPic.autoSelect = checked;
    ui->picLabel_2->canDraw = checked;
    on_resetButton_clicked();
    Mat temp;
    Mat threshold;
    if(pic.currentAdjustedPic->mat.channels() == 3){
        cv::cvtColor(pic.currentAdjustedPic->mat,temp,COLOR_RGB2GRAY);
        cv::threshold(temp, threshold, 50, 255, THRESH_BINARY_INV);
    }
    else {
        threshold = pic.currentAdjustedPic->mat;
    }
    vector<Point2f> viewpoints = getCurve.AutoGetCurve(threshold, pic.extractPic.lineptm);
    QList<QPointF> originalAxisList;
    QPointF relativeAxis;
    QPointF physicalAxis;
    for (Point2f p : viewpoints) {
        originalAxisList.push_back(QPointF(p.x, p.y));
        relativeAxis = pic.resizeAxis(*pic.currentAdjustedPic, originalAxisList, true)[0];
        physicalAxis = pic.convertOriginalToPhysicalAxis(originalAxisList[0], *pic.currentAdjustedPic);
        extractUndoStack.push(new InsertPoint(&pic.extractPic, ui->picLabel_2, QPoint(relativeAxis.x(), relativeAxis.y()), physicalAxis));
        originalAxisList.clear();
    }
    updateTable();
    updateUndoButtons();
}

void MainWindow::on_manualChooseRadioButton_toggled(bool checked)
{
    pic.extractPic.manualSelect = checked;
    ui->picLabel_2->canDraw = checked;
    on_resetButton_clicked();
    //ui->picLabel_2->update();
    if(checked){
        QMessageBox::information(this,"提示","手动选取已启动!");
    }
}

void MainWindow::on_colorLabel_clicked()
{
    initColorPicker();
}

void MainWindow::on_chooseColorLabel_clicked()
{
    initColorPicker();
}

void MainWindow::initColorPicker()
{
    pic.extractPic.pickColor = true;
    pic.currentAdjustedPic = &pic.adjustedPic;
    ui->picLabel_2->setPixmap(pic.currentAdjustedPic->pixmapToShow);
    on_resetButton_clicked();
    setCursor(QCursor(QPixmap(":/icon/src/colorpicker.png"),0,21));
    ui->autoChooseRadioButton->setEnabled(false);
    ui->manualChooseRadioButton->setEnabled(false);
}

void MainWindow::on_picLabel_2_clicked()
{
    if(pic.extractPic.pickColor){
        //取色
        QImage image = pic.adjustedPic.pixmapToShow.toImage();
        QPoint viewPickPoint = ui->picLabel_2->mapFromGlobal(QCursor::pos());
        QList<QPoint> list;
        list.push_back(viewPickPoint);
        pic.extractPic.pickPoint = pic.resizeAxis(pic.adjustedPic, list, false)[0];

        pic.extractPic.curveColor = image.pixelColor(viewPickPoint);
        ui->colorLabel->paintColor = pic.extractPic.curveColor;
        ui->colorLabel->update();

        Point2i point(pic.extractPic.pickPoint.x(), pic.extractPic.pickPoint.y());
        pic.singleColorPic.mat = getColor.GetPointColor(pic.adjustedPic.mat, point);
        pic.ConstructPicContent(pic.singleColorPic, pic.singleColorPic.mat, false);
        pic.currentAdjustedPic = &pic.singleColorPic;
        ui->picLabel_2->setPixmap(pic.currentAdjustedPic->pixmapToShow);

        ui->autoChooseRadioButton->setEnabled(true);
        ui->manualChooseRadioButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);
        pic.extractPic.pickColor = false;
        return;
    }
    if(pic.extractPic.manualSelect){
        //打点
        if(!pic.extractPic.manualSelect) return;
        QPoint relativeAxis = ui->picLabel_2->mapFromGlobal(QCursor::pos());
        QPointF physicalAxis = pic.convertViewToPhysicalAxis(relativeAxis, *pic.currentAdjustedPic);
        //转换原图的坐标系坐标
        extractUndoStack.push(new InsertPoint(&pic.extractPic, ui->picLabel_2, relativeAxis, physicalAxis));
        updateTable();
        updateUndoButtons();
        qDebug() << relativeAxis;
        return;
    }
}

void MainWindow::on_undoButton_clicked()
{
    extractUndoStack.undo();
    updateTable();
    updateUndoButtons();
}

void MainWindow::on_redoButton_clicked()
{
    extractUndoStack.redo();
    updateTable();
    updateUndoButtons();
}

void MainWindow::on_resetButton_clicked()
{
    extractUndoStack.clear();
    pic.extractPic.clearList();
    ui->picLabel_2->clearList();
    ui->picLabel_2->update();
    updateTable();
    updateUndoButtons();
}


void MainWindow::on_outputButton_clicked()
{
    //保存路径选择
    QString filePath;
    QString fileName = QDateTime().currentDateTime().toString("yyyyMMdd-hhmmss")+".csv";
    filePath = QFileDialog::getSaveFileName(this,tr("导出数据"),pic.filePath+"\\"+fileName,".csv");
    qDebug()<<filePath;
    if(filePath.isEmpty()) {
        return;
    }
    outputData(filePath);
}


void MainWindow::outputData(QString fileName)
{
    //导出数据
    ofstream outp;
    outp.open(fileName.toStdString(), ios::out | ios::trunc);
    //文件打开失败
    if (outp.fail()){
        QMessageBox::critical(this,"错误","数据导出失败!\n无写入权限，请更换路径到非系统分区!");
        return;
    }
    outp << "横坐标" << "," << "纵坐标" << "," << endl;
    for (QPointF p : pic.extractPic.getListF())
    {
        outp << p.x() << "," << p.y() << "," << endl;
    }
    outp.close();

    QMessageBox::information(this,"提示","数据导出成功!");
}
