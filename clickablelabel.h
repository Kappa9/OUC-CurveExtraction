#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QPainter>
#include <QPoint>
#include <QList>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    virtual void paintEvent(QPaintEvent *event) override;
    ~ClickableLabel();

    void addPoint(QPoint p);
    void removePoint();
    void clearList();
    bool canDraw = false;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    QList<QPoint> pointList;
    int penSize;
    int crossSize;
};
#endif // CLICKABLELABEL_H
