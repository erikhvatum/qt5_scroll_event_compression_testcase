// By Erik Hvatum.  Copyright waived.  The contents of this file are public domain. 
 
#pragma once

#include <QtWidgets>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

class TestView
  : public QGraphicsView
{
    Q_OBJECT;

public:
    TestView(QGraphicsScene* scene, QWidget* parent=NULL);

signals:
    void mouse_moved(QPointF);

protected:
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
};
