#include "colorpicker.h"

ColorPicker::ColorPicker(QWidget* parent, Qt::WindowFlags flag)
    : QLabel(parent) {
    paintColor = QColor(255, 255, 255);
}

void ColorPicker::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);//必须有，才能让背景图片显示出来
    QPainter painter(this);
    QBrush brush(paintColor); //画刷
    painter.setBrush(brush); //添加画刷
    painter.drawRect(0, 0, width(), height()); //绘制矩形
}

ColorPicker::~ColorPicker() {}

void ColorPicker::updatePainter()
{
    update();
}

void ColorPicker::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}
