#ifndef COLORPICKER_H
#define COLORPICKER_H
#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QPainter>
#include <QPoint>
#include <QList>
#include <QColor>

class ColorPicker : public QLabel {
    Q_OBJECT

public:
    explicit ColorPicker(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    virtual void paintEvent(QPaintEvent *event) override;
    ~ColorPicker();
    void updatePainter();
    QColor paintColor;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    QList<QPoint> pointList;

};

#endif // COLORPICKER_H
