#pragma once

#include <QWidget>
#include <QImage>
#include <QPointF>
#include <QPainter>

const int DISPLAY_WIDTH = 1280;
const int DISPLAY_HEIGHT = 800;

class TDisplay : public QWidget
{
    Q_OBJECT
public:
    TDisplay(QWidget *parent = 0);
    ~TDisplay();
private:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);
    void mouseMoveEvent(QMouseEvent* e);
private:
    void DrawLightning(QPainter& painter, const QPointF& from, const QPointF& to);
    void DrawBezier(QPainter& painter, const QPointF& p0, QPointF p1, const QPointF& p2);
private:
    QImage Frame;
    QPointF To;
};
