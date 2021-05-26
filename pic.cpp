#include "pic.h"

Pic::Pic()
{
    this->filePath = "C:/";
    this->displayWidth = 640;
    this->displayHeight = 480;
    this->currentOriginalPic = &originalPic;
    this->currentAdjustedPic = &adjustedPic;
}

void Pic::ConstructPicContent(PicContent& content, Mat input, bool colored)
{
    content.mat = input;
    QImage::Format format = QImage::Format_RGB888;
    if(!colored) format = QImage::Format_Grayscale8;
    QImage qImg = QImage((const unsigned char*)(content.mat.data), content.mat.cols, content.mat.rows, content.mat.step, format);
    content.width = content.mat.cols;
    content.height = content.mat.rows;
    content.pixmap = QPixmap::fromImage(qImg);
    content.pixmapToShow = content.pixmap.scaled(displayWidth, displayHeight, Qt::KeepAspectRatio,Qt::SmoothTransformation);
    content.hasPic = true;
}

QList<QPoint> Pic::resizeAxis(PicContent& content, QList<QPoint> list, bool revert)
{
    if(content.width == displayWidth) return list;
    QList<QPoint> sendList;
    float expand = content.width;
    float shrink = displayWidth;
    if(revert){
        shrink = content.width;
        expand = displayWidth;
    }
    float x,y;
    for(QPoint p : list){
        x = expand / shrink * p.x();
        y = expand / shrink * p.y();
        QPoint newP(x,y);
        sendList.push_back(newP);
    }
    return sendList;
}

QList<QPointF> Pic::resizeAxis(Pic::PicContent &content, QList<QPointF> list, bool revert)
{
    if(content.width == displayWidth) return list;
    QList<QPointF> sendList;
    float expand = content.width;
    float shrink = displayWidth;
    if(revert){
        shrink = content.width;
        expand = displayWidth;
    }
    float x,y;
    for(QPointF p : list){
        x = expand / shrink * p.x();
        y = expand / shrink * p.y();
        QPointF newP(x,y);
        sendList.push_back(newP);
    }
    return sendList;
}

QPointF Pic::convertViewToPhysicalAxis(QPoint p, PicContent &content)
{
    float x = border.xmin + (border.xmax - border.xmin) * float(p.x()) / float(content.pixmapToShow.width());
    float y = border.ymin + (border.ymax - border.ymin) * (content.pixmapToShow.height() - float(p.y())) / float(content.pixmapToShow.height());
    std::ostringstream convertx, converty;
    convertx << std::setprecision(4) << x;
    converty << std::setprecision(4) << y;
    x = std::stof(convertx.str());
    y = std::stof(converty.str());
    QPointF newp(x, y);
    return newp;
}

QPointF Pic::convertOriginalToPhysicalAxis(QPointF p, Pic::PicContent &content)
{
    float x = border.xmin + (border.xmax - border.xmin) * p.x() / float(content.mat.cols);
    float y = border.ymin + (border.ymax - border.ymin) * (content.mat.rows - p.y()) / float(content.mat.rows);
    std::ostringstream convertx, converty;
    convertx << std::setprecision(4) << x;
    converty << std::setprecision(4) << y;
    x = std::stof(convertx.str());
    y = std::stof(converty.str());
    QPointF newp(x, y);
    return newp;
}

QPoint Pic::convertPhysicalToViewAxis(QPointF p, Pic::PicContent &content)
{
    float x = (p.x() - border.xmin) * float(content.pixmapToShow.width()) / (border.xmax - border.xmin);
    float y = (p.y() - border.ymin) * float(content.pixmapToShow.height()) / (border.ymax - border.ymin);
    QPoint newp(x, y);
    return newp;
}

AxisList::AxisList()
{

}

bool AxisList::updateAxis(QPoint p)
{
    axisList.push_back(p);
    return true;
}

void AxisList::removeAxis()
{
    if(!axisList.isEmpty()) axisList.pop_back();
}

void AxisList::clearList()
{
    axisList.clear();
}

QList<QPoint> AxisList::getList()
{
    return axisList;
}

void AxisList::printList()
{
    for (QPoint i : axisList){
        qDebug()<<"存在的点："<<i;
    }
}

int AxisList::getLength()
{
    return axisList.length();
}

bool InputAxis::updateAxis(QPoint p)
{
    if(axisList.length() >= 3) return false;
    axisList.push_back(p);
    return true;
}

bool ExtractCurveAxis::updateAxisF(QPointF p)
{
    axisListF.push_back(p);
    return true;
}

void ExtractCurveAxis::removeAxis()
{
    if(!axisListF.isEmpty()) axisListF.pop_back();
}

void ExtractCurveAxis::clearList()
{
    axisListF.clear();
}

int ExtractCurveAxis::getLength()
{
    return axisListF.length();
}

QList<QPointF> ExtractCurveAxis::getListF()
{
    return axisListF;
}

InsertPoint::InsertPoint(ExtractCurveAxis *list, ClickableLabel* label, QPoint viewp, QPointF phyp) : QUndoCommand("Insert Point")
{
    this->pointList = list;
    this->paintLabel = label;
    this->viewPoint = viewp;
    this->physicalPoint = phyp;
}

void InsertPoint::redo()
{
    pointList->updateAxisF(physicalPoint);
    paintLabel->addPoint(viewPoint);
    paintLabel->update();
}

void InsertPoint::undo()
{
    pointList->removeAxis();
    paintLabel->removePoint();
    paintLabel->update();
}


