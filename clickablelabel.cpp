#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags flag)
    : QLabel(parent) {
    canDraw = false;
    penSize = 1;
    crossSize = 6;
}

void ClickableLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);//必须有，才能让背景图片显示出来
    if(!canDraw) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(QPen(Qt::red, penSize, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
    for(QPoint i : pointList){
        painter.drawLine(i.x() - crossSize, i.y(), i.x() + crossSize, i.y());
        painter.drawLine(i.x(), i.y() - crossSize, i.x(), i.y() + crossSize);
    }
}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::addPoint(QPoint p)
{
    pointList.push_back(p);
}

void ClickableLabel::removePoint()
{
    if(!pointList.isEmpty()) pointList.pop_back();
}

void ClickableLabel::clearList()
{
    pointList.clear();
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    update();
    emit clicked();
}
