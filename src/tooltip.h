#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <qtooltip.h>

class DynamicTip : public QToolTip
{
    public:
	DynamicTip(QWidget *parent);
	
    protected:
	void maybeTip(const QPoint &);
};

#endif
