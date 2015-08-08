// By Erik Hvatum.  Copyright waived.  The contents of this file are public domain.

#include <cmath>
#include <QDebug>
#include <QTransform>
#include "TestView.h"

#ifdef pow
 #undef pow
#endif
#ifdef abs
 #undef abs
#endif

TestView::TestView(QGraphicsScene* scene, QWidget* parent)
  : QGraphicsView(scene, parent)
{
    setMouseTracking(true);
}

void TestView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    mouse_moved(mapToScene(event->x(), event->y()));
}
void TestView::wheelEvent(QWheelEvent* event)
{
    qreal zoom{transform().m22()};
    const qreal original_zoom{zoom};
    qreal increments{event->angleDelta().y() / 120.0};
    if(increments > 0) zoom *= pow(1.25, increments);
    else if(increments < 0) zoom *= pow(0.8, -increments);
    else return;
    if(zoom < 0.167772) zoom = 0.167772;
    else if(zoom > 18.1899) zoom = 18.1899;
    if(std::abs(std::abs(zoom) - 1) < 0.1) zoom = 1;
    qreal scale_zoom{zoom / original_zoom};
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    scale(scale_zoom, scale_zoom);
    mouse_moved(mapToScene(event->x(), event->y()));
    qDebug() << "zoom:" << zoom << "angleDelta:" << event->angleDelta() << "zoom increments" << increments;
}
