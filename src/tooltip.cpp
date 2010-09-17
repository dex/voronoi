#include <qrect.h>

#include "tooltip.h"
#include "geometry.h"

DynamicTip::DynamicTip(QWidget *parent)
    : QToolTip(parent)
{
    // nothing
}

void DynamicTip::maybeTip(const QPoint &pos)
{
    /* IMPORTANT : This point's position is 
     * mouse point's position plus (1, 1) */
    if ( !parentWidget()->inherits("DiagramView"))
	return;

    if ( ((DiagramView *)parentWidget())->hasPoint(pos.x()-1, pos.y()-1)) {
	int d = (DiagramPoint::DIAMETER / 2);
	int s = DiagramPoint::DIAMETER;
	QRect rect(pos.x()-d, pos.y()-d, s, s);
	tip(rect, QString("%1,%2").arg(pos.x()-1).arg(pos.y()-1));
    } else {
	return;
    }
}
