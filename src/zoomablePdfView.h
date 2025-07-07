#pragma once
#include <QPdfView>
#include <QWheelEvent>

class ZoomablePdfView : public QPdfView{
    Q_OBJECT
public:
    using QPdfView::QPdfView;
protected:
    void wheelEvent(QWheelEvent* event) override{
        if(event->modifiers() & Qt::ControlModifier){
            setZoomMode(QPdfView::ZoomMode::Custom);
            qreal factor = zoomFactor();
            if(event->angleDelta().y() > 0)
                setZoomFactor(factor * 1.2);
            else
                setZoomFactor(factor / 1.2);
            event->accept();
        }
        else{
            QPdfView::wheelEvent(event);
        }
    }
};