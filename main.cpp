// By Erik Hvatum.  Copyright waived.  The contents of this file are public domain.

#include <chrono>
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QKeySequence>
#include <QLinearGradient>
#include <QMainWindow>
#include <QObject>
#include <QOpenGLWidget>
#include <QPair>
#include <QPen>
#include <QString>
#include <QSurfaceFormat>
#include <QTimer>
#include <QToolBar>
#include <QVector>
#include <thread>

#include "TestView.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QMainWindow* mw{new QMainWindow()};
    mw->setWindowTitle("scroll event compression test case");
    QToolBar* toolbar{mw->addToolBar("toolbar")};
    QAction* sleep_a_lot_action{new QAction("Sleep A Lot", mw)};
    sleep_a_lot_action->setCheckable(true);
    sleep_a_lot_action->setChecked(false);
    sleep_a_lot_action->setToolTip("When activated, the main thread will attempt to sleep for 999ms intervals with 1ms gaps between naps.\n"
                                   "Shortcut: spacebar");
    sleep_a_lot_action->setShortcut(QKeySequence(Qt::Key_Space));
    sleep_a_lot_action->setShortcutContext(Qt::WindowShortcut);
    QTimer* sleep_a_lot_timer{new QTimer(mw)};
    QObject::connect(sleep_a_lot_action, &QAction::toggled, [&](const bool& is_on){
        if(is_on) sleep_a_lot_timer->start();
        else sleep_a_lot_timer->stop();});
    sleep_a_lot_timer->setSingleShot(false);
    sleep_a_lot_timer->setInterval(1);
    QObject::connect(sleep_a_lot_timer, &QTimer::timeout, [&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(999));
        if(sleep_a_lot_action->isChecked()) sleep_a_lot_timer->start();
    });
    toolbar->addAction(sleep_a_lot_action);
    toolbar->addSeparator();
    QGraphicsScene* gs{new QGraphicsScene(mw)};
    QGraphicsSimpleTextItem* sti{gs->addSimpleText("Move the mouse wheel a few clicks up or down with various combinations of:\n"
                                                   "* \"Sleep A Lot\" enabled/disabled (shortcut to toggle: spacebar).\n"
                                                   "* Mouse held still/mouse moved about.\n\n"
                                                   "Questions whose answers differ for the same version of Qt on various\n"
                                                   "platforms (eg, Win32, X11, Aqua):\n"
                                                   "* With \"Sleep A Lot\" active, are mouse wheel events compressed?\n"
                                                   "* With \"Sleep A Lot\" active, does moving the mouse and the mouse wheel\n"
                                                   "  cause scroll wheel events to be thrown away by compression that are not\n"
                                                   "  thrown away when just the mouse wheel is manipulated?\n"
                                                   "* With \"Sleep A Lot\" active, is it necessary to move the mouse in order\n"
                                                   "  to provoke processing of buffered mouse wheel events?")};
    sti->setBrush(QBrush(Qt::black));
    sti->setPen(QPen(Qt::white));
    {
        QFont f(sti->font());
        f.setBold(true);
        f.setPointSize(36);
        sti->setFont(f);
    }
    gs->setSceneRect(sti->boundingRect());
    QGraphicsEllipseItem* mc{gs->addEllipse(0, 0, 25, 25, QPen(QColor(0, 255, 0, 200)), QBrush(QColor(0, 0, 0, 100)))};
    mc->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    QGraphicsSimpleTextItem* mcsti{gs->addSimpleText("0\n0")};
    mcsti->setBrush(QBrush(Qt::white));
    mcsti->setParentItem(mc);
    mcsti->moveBy(23, 22);
    QGraphicsDropShadowEffect* mcstige{new QGraphicsDropShadowEffect(gs)};
    mcstige->setBlurRadius(10);
    mcstige->setOffset(0, 0);
    mcstige->setColor(Qt::black);
    mcsti->setGraphicsEffect(mcstige);
    TestView* gv{new TestView(gs, mw)};
    QOpenGLWidget* gl_widget = new QOpenGLWidget;
    {
        QSurfaceFormat fmt;
        fmt.setRenderableType(QSurfaceFormat::OpenGL);
        fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
        fmt.setVersion(2, 1);
        fmt.setSamples(4);
        gl_widget->setFormat(fmt);
    }
    gv->setViewport(gl_widget);
    QObject::connect(gv, &TestView::mouse_moved, [&](const QPointF& scene_pos){
        mcsti->setText(QString("%1\n%2").arg(scene_pos.x()).arg(scene_pos.y()));
        qreal zoom{gv->transform().m22()};
        qreal delta{12.5 / zoom};
        mc->setPos(scene_pos.x() - delta, scene_pos.y() - delta);
    });
    gv->setDragMode(QGraphicsView::ScrollHandDrag);
    {
        QLinearGradient g;
        g.setSpread(QGradient::ReflectSpread);
        QGradientStops grad_stops;
        grad_stops.push_back(QGradientStop(0, QColor(Qt::red)));
        grad_stops.push_back(QGradientStop(0.5, QColor(Qt::green)));
        grad_stops.push_back(QGradientStop(1, QColor(Qt::blue)));
        g.setStops(grad_stops);
        QBrush b(g);
        gs->setBackgroundBrush(b);
    }
    mw->setCentralWidget(gv);
    mw->resize(2045, 838);
    mw->show();
    return app.exec();
}
