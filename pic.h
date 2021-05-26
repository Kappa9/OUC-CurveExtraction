#ifndef PIC_H
#define PIC_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QStack>
#include <QUndoCommand>
#include <QString>
#include <iomanip>
#include <sstream>
#include <string>
#include "clickablelabel.h"

using namespace cv;

class AxisList{
public:
    AxisList();
    virtual bool updateAxis(QPoint p);
    virtual void removeAxis();
    virtual void clearList();
    virtual int getLength();
    QList<QPoint> getList();
    void printList();
protected:
    QList<QPoint> axisList;
};

class InputAxis : public AxisList{
public:
    bool autoAdjust = false;
    bool manualAdjust = false;
    bool updateAxis(QPoint p);
};

class ExtractCurveAxis : public AxisList{
public:
    bool coloredCurve = false;
    bool autoSelect = false;
    bool manualSelect = false;
    bool pickColor = false;
    QColor curveColor = QColor(0, 0, 0);
    QPoint pickPoint;
    bool updateAxisF(QPointF p);
    void removeAxis();
    void clearList();
    int getLength();
    QList<QPointF> getListF();
protected:
    QList<QPointF> axisListF;
};

class InsertPoint : public QUndoCommand{
public:
    QPoint viewPoint;
    QPointF physicalPoint;
    ExtractCurveAxis* pointList;
    ClickableLabel* paintLabel;
    InsertPoint(ExtractCurveAxis* list, ClickableLabel* label, QPoint viewp, QPointF phyp);
    virtual void redo();
    virtual void undo();
};

class Pic
{
public:
    Pic();
    QString filePath = "";
    int displayWidth;
    int displayHeight;
    class Border{
    public:
        float xmin;
        float xmax;
        float ymin;
        float ymax;
    };
    Border border;
    class PicContent{
    public:
        Mat mat;
        QPixmap pixmap;
        QPixmap pixmapToShow;
        int width;
        int height;
        bool hasPic = false;
    };
    PicContent originalPic;
    PicContent autoAdjustPic;
    PicContent adjustedPic;
    PicContent singleColorPic;
    PicContent* currentOriginalPic;
    PicContent* currentAdjustedPic;
    void ConstructPicContent(PicContent &content, Mat input, bool colored);
    InputAxis inputPic;
    ExtractCurveAxis extractPic;
    QList<QPoint> resizeAxis(PicContent &content, QList<QPoint> list, bool revert);
    QList<QPointF> resizeAxis(PicContent &content, QList<QPointF> list, bool revert);
    QPointF convertViewToPhysicalAxis(QPoint p, PicContent &content);
    QPointF convertOriginalToPhysicalAxis(QPointF p, PicContent &content);
    QPoint convertPhysicalToViewAxis(QPointF p, PicContent &content);
};

#endif // PIC_H
