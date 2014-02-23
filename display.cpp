#include <vector>
#include <cmath>

#include <QDesktopWidget>
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QLinearGradient>

#include "display.h"

TDisplay::TDisplay(QWidget *parent)
    : QWidget(parent)
    , Frame(QSize(DISPLAY_WIDTH, DISPLAY_HEIGHT), QImage::Format_ARGB32)
    , To(600, 400)
{
    QDesktopWidget desktop;
    setGeometry(desktop.width() / 2 - DISPLAY_WIDTH / 2,
                desktop.height() / 2 - DISPLAY_HEIGHT / 2,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT);
    Frame.fill(Qt::black);
    startTimer(50);
    setMouseTracking(true);
    update();
}

TDisplay::~TDisplay() {
}

void TDisplay::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.drawImage(0, 0, Frame);
}

struct TSegment {
public:
    TSegment(QPointF start, QPointF end, int width = 2, QColor color = Qt::white)
        : Start(start)
        , End(end)
        , Width(width)
        , Color(color)
    {
    }
    TSegment(const TSegment& segment)
        : Start(segment.Start)
        , End(segment.End)
        , Width(segment.Width)
        , Color(segment.Color)
    {
    }
    QPointF Start;
    QPointF End;
    int Width;
    QColor Color;
};

typedef std::vector<TSegment> TSegmentList;

void DrawSegmentList(QPainter& painter, const TSegmentList& segments) {
    for (TSegmentList::const_iterator it = segments.begin(); it != segments.end(); ++it) {
        QPen pen;
        pen.setWidth(it->Width);
        pen.setColor(it->Color);
        painter.setPen(pen);
        painter.drawLine(it->Start, it->End);
    }
}

inline QPointF Average(const QPointF& a, const QPointF& b) {
    return (a + b) / 2;
}

inline QPointF Normalize(const QPointF& point) {
    float abs = sqrt(point.x() * point.x() + point.y() * point.y());
    return point / abs;
}

inline float RandomFloat(float from, float to) {
    float num = random() % 1000;
    return from + (to - from) * num / 1000.0;
}

inline QPointF Perpendicular(const QPointF& point) {
    return QPointF(-point.y(), point.x());
}

inline QPointF Rotate(const QPointF& point, float a) {
    QPointF result;
    result.setX(point.x() * cos(a) - point.y() * sin(a));
    result.setY(point.x() * sin(a) + point.y() * cos(a));
    return result;
}

void TDisplay::timerEvent(QTimerEvent*) {
    Frame.fill(Qt::black);
    QPainter painter(&Frame);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::white);
    pen.setWidth(3);
    painter.setPen(pen);

    QPointF from = QPointF(0, 500);
    QPointF mid = QPointF(0.8 * To.x(), 500);

    DrawBezier(painter, from, mid, To);

    DrawLightning(painter, To, QPointF(100, 100));

    update();
}

void TDisplay::mouseMoveEvent(QMouseEvent* e) {
    To = e->pos();
}

void TDisplay::DrawLightning(QPainter& painter, const QPointF& from, const QPointF& to) {
    TSegmentList segments;
    segments.push_back(TSegment(from, to));

    float offset = 50.0;
    float lengthScale = 0.7;

    for (size_t i = 0; i < 4; ++i) {
        TSegmentList newSegments;
        for (size_t j = 0; j < segments.size(); ++j) {
            const TSegment& segment = segments[j];
            QPointF mid = Average(segment.Start, segment.End);

            QColor color = Qt::white;

            mid += Perpendicular(Normalize(segment.End - segment.Start)) * RandomFloat(-offset, offset);
            newSegments.push_back(TSegment(segment.Start, mid, 2, color));
            newSegments.push_back(TSegment(mid, segment.End, 2, color));

            QPointF direction = mid - segment.Start;
            QPointF splitEnd = Rotate(direction, RandomFloat(-0.8, 0.8)) * lengthScale + mid;

            color.setRed(0.8 * color.red());
            color.setGreen(0.8 * color.green());
            color.setBlue(0.8 * color.blue());

            newSegments.push_back(TSegment(mid, splitEnd, 1, color));
        }
        segments.swap(newSegments);
        offset /= 2;
    }

    DrawSegmentList(painter, segments);
}

inline float sqr(float expr) {
    return expr * expr;
}

void TDisplay::DrawBezier(QPainter& painter, const QPointF& p0, QPointF p1, const QPointF& p2) {
    QPointF prevPos = p0;
    p1 *= 2;
    for (float t = 0; t < 1; t += 0.01) {
        QPointF pos = p0 * sqr(1.0f - t) + p1 * t * (1.0f - t) + p2 * sqr(t);
        painter.drawLine(prevPos, pos);
        prevPos = pos;
    }
}
