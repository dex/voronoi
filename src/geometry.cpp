#include <qaction.h>
#include <qpopupmenu.h>
#include <qcanvas.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qpoint.h>
#include <qpair.h>

#include <algorithm>

#include "geometry.h"
#include "tooltip.h"
#include "algorithm.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// 	DiagramView
// {{{

DiagramView::DiagramView(QCanvas *canvas, QWidget *parent, const char *name)
    : QCanvasView(canvas, parent, name)
{ // {{{
    viewport()->setMouseTracking(true);
    dynTip = new DynamicTip(this);
    createActions();
} // }}}

bool DiagramView::hasPoint(const int x, const int y)
{ // {{{
    return isDuplicate(x, y);
} // }}}

void DiagramView::createActions()
{ // {{{
    newAct = new QAction(tr("&New"), tr("Ctrl+N"), this);
    newAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/filenew.png")));
    connect(newAct, SIGNAL(activated()), this, SLOT(newFile()));
    calAct = new QAction(tr("&Run"), tr("Ctrl+R"), this);
    calAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/run.png")));
    connect(calAct, SIGNAL(activated()), this, SLOT(calculate()));
} // }}}  

void DiagramView::newFile()
{ // {{{
    QValueVector<DiagramBisector*>::const_iterator it;
    for (it = edgeList.begin(); it!=edgeList.end(); it++)
	delete (*it);
    edgeList.clear();

    QValueVector<DiagramPoint*>::const_iterator it2;
    for (it2 = pointList.begin(); it2!=pointList.end(); it2++)
	delete (*it2);
    pointList.clear();


    canvas()->update();
    qDebug("-===========New=============-");
} // }}}

void DiagramView::calculate()
{ // {{{
    qDebug("-===========Calculate=============-");
    /* sort before starting algorithm */
    stable_sort(pointList.begin(), pointList.end(), PtrLess<DiagramPoint *>());

    /* For New Voronoi Diagram */
    if (edgeList.size() > 0) {
	/* Delete bisectors */
	QValueVector<DiagramBisector*>::const_iterator it;
	for (it = edgeList.begin(); it!=edgeList.end(); it++)
	    delete (*it);
	edgeList.clear();
	/* Clear bisector associate with DiagramPoint */
	QValueVector<DiagramPoint*>::const_iterator it2;
	for (it2 = pointList.begin(); it2!=pointList.end(); it2++)
	    (*it2)->getEdgeList().clear();
	canvas()->update();
    }

    /* start the divide-and-conquer algorithm */
    VoronoiAlgo algorithm(pointList, edgeList, this);
    algorithm.start();

    /* draw the canvas */
    for (QValueVector<DiagramBisector *>::iterator it = edgeList.begin();
	    it != edgeList.end(); it++) {
	(*it)->draw();
	qDebug("bisector of (%d,%d)-(%d,%d) has %d infinity end.", 
		(*it)->getLeftPoint()->getX(), 
		(*it)->getLeftPoint()->getY(), 
		(*it)->getRightPoint()->getX(), 
		(*it)->getRightPoint()->getY(), 
		(*it)->getLineType());
    }
} // }}}

void DiagramView::contentsContextMenuEvent(QContextMenuEvent *event)
{ // {{{
    QPopupMenu contextMenu(this);
    newAct->addTo(&contextMenu);
    calAct->addTo(&contextMenu);
    contextMenu.exec(event->globalPos());
} // }}}

bool DiagramView::isDuplicate(int x, int y)
{ // {{{
    QValueVector<DiagramPoint*>::const_iterator it;
    for (it = pointList.begin(); it!=pointList.end(); it++) {
	if ( (*it)->x() == x && (*it)->y() == y) {
	    return true;
	}
    }
    return false;
} // }}}

void DiagramView::contentsMousePressEvent(QMouseEvent *event)
{ // {{{
    if (event->button() == LeftButton)
	addPoint(event->pos().x(), event->pos().y());
} // }}}

void DiagramView::contentsMouseMoveEvent(QMouseEvent *event)
{ // {{{
    emit locationChanged(event->pos().x(), event->pos().y());
} // }}}

void DiagramView::addPoint(int x, int y)
{ // {{{
    if (!isDuplicate(x, y) && 
	    x >= 0 && x <= canvas()->width() && 
	    y >= 0 && y <= canvas()->height()) {
	DiagramPoint *newPoint = new DiagramPoint(x, y, 
		DiagramPoint::DIAMETER, canvas());
	pointList.push_back(newPoint);
	qDebug("(%d,%d) is been added.", x, y);
    }
} // }}}

DiagramView::~DiagramView()
{ // {{{
    delete dynTip;
    dynTip = 0;
} // }}}
// }}}

///////////////////////////////////////////////////////////////////////////////
// 	DiagramPoint
// {{{

DiagramPoint::DiagramPoint(int x, int y, int diameter, QCanvas *canvas)
    : QCanvasEllipse(diameter,diameter,canvas), posX(x), posY(y)
{ // {{{
    setBrush(QColor(Qt::black));
    move(x, y);
    show();
    canvas->update();
} // }}}

void DiagramPoint::addEdge(DiagramBisector *edge)
{ // {{{
    edgeList.push_back(edge);
} // }}}

bool DiagramPoint::operator==(DiagramPoint &rhs) const
{ // {{{
    return ( posX == rhs.getX() && posY == rhs.getY());
} // }}}

bool DiagramPoint::operator<(const DiagramPoint &rhs) const
{ // {{{
    if (posX <= rhs.getX()) {
	if (posX == rhs.getX()) {
	    if (posY < rhs.getY())
		return true;
	    return false;
	}
	return true;
    }
    return false;
} // }}}

DiagramPoint::~DiagramPoint()
{ // {{{
    hide();
} // }}}
// }}}

///////////////////////////////////////////////////////////////////////////////
// 	DiagramLine
// {{{

DiagramLine::DiagramLine(DiagramPoint *leftPoint, DiagramPoint *rightPoint, 
	QCanvas *canvas) : QCanvasLine(canvas), leftPoint(leftPoint),
			   rightPoint(rightPoint)
{ // {{{
    int xa = leftPoint->getX();		int ya = leftPoint->getY();
    int xb = rightPoint->getX();	int yb = rightPoint->getY();

    setPoints(xa, ya, xb, yb);
    a = (double)ya-yb;
    b = (double)xb-xa;
    c = a * (double)xa + b * (double)ya;
    middleX = (double)( xa + xb ) / 2;
    middleY = (double)( ya + yb ) / 2;

} // }}}

void DiagramLine::draw()
{ // {{{
    setPen(QPen(QColor(Qt::red), 0, DotLine));
    show();
    canvas()->update();
} // }}}

bool DiagramLine::operator==(DiagramLine &rhs) const
{ // {{{
    if ( *(rhs.getLeftPoint()) == *leftPoint
	    && *(rhs.getRightPoint()) == *rightPoint ) {
	return true;
    } else if ( *(rhs.getRightPoint()) == *leftPoint
	    && *(rhs.getLeftPoint()) == *rightPoint ) {
	return true;
    }
    return false;
} // }}}

bool DiagramLine::isDiffArea(double xa, double ya, double xb, double yb)
{ // {{{
    if ( ((a*xa+b*ya)-c) * ((a*xb+b*yb)-c) <= 0)
	return true;
    return false;
} // }}}

DiagramLine::~DiagramLine()
{ // {{{
    hide();
} // }}}
// }}}

///////////////////////////////////////////////////////////////////////////////
// 	DiagramBisector
// {{{

DiagramBisector::DiagramBisector(DiagramLine line, QCanvas *canvas, 
	DiagramPoint* refPoint, 
	double startX, double startY)
    : QCanvasLine(canvas), line(line),
    leftPoint(line.getLeftPoint()), rightPoint(line.getRightPoint()), 
    refPoint(refPoint)
{ // {{{
    QPair<double, double> middlePoint(line.getMiddlePoint());
    a = line.getB();
    b = line.getA() * (-1);
    c = a * middlePoint.first + b * middlePoint.second;

    enable = true;
    HP = false;

    boundStartPointX = -1;	boundStartPointY = -1;
    boundEndPointX = -1;	boundEndPointY = -1;
    intersectPointX = -1;	intersectPointY = -1;
    cutter = NULL;

    /* Add relationship between DiagramBisector and DiagramPoint */
    leftPoint->addEdge(this);	rightPoint->addEdge(this);

    findBoundIntersect(canvas);
    if (refPoint == NULL) { /* There is no reference point */
	if (boundStartPointY <= boundEndPointY) {
	    startPointX = boundStartPointX;	startPointY = boundStartPointY;
	    endPointX = boundEndPointX;		endPointY = boundEndPointY;
	    isStartINF = true; isEndINF = true;
	} else {
	    startPointX = boundEndPointX;	startPointY = boundEndPointY;
	    endPointX = boundStartPointX;	endPointY = boundStartPointY;
	    isStartINF = true; isEndINF = true;
	}
    } else {	/* There is a reference point */
	startPointX = startX;	startPointY = startY;
	isStartINF = false;
	if (boundStartPointY <= boundEndPointY) {
	    endPointX = boundEndPointX;		endPointY = boundEndPointY;
	    isEndINF = true;
	} else {
	    endPointX = boundStartPointX;	endPointY = boundStartPointY;
	    isEndINF = true;
	}

	if (startY > endPointY) {
	    endPointX = startPointX*2-endPointX;
	    endPointY = startPointY*2-endPointY;
	}
    }
} // }}}

DiagramPoint* DiagramBisector::getComPoint(DiagramBisector *line)
{ // {{{
    if (*leftPoint == *(line->getLeftPoint()) || 
	    *leftPoint == *(line->getRightPoint()))
	return leftPoint;
    if (*rightPoint == *(line->getLeftPoint()) || 
	    *rightPoint == *(line->getRightPoint()))
	return rightPoint;
    return NULL;
} // }}}

DiagramBisector::LineType DiagramBisector::getLineType() const
{ // {{{
    int type = 0;
    if (isStartINF) 
	type++;
    if (isEndINF) 
	type++;

    return (DiagramBisector::LineType)type;
} // }}}

void DiagramBisector::findBoundIntersect(QCanvas *canvas)
{ // {{{
    double width = (double)canvas->width();
    double height = (double)canvas->height();
    
    if (a == 0 && b != 0) {
	setBoundPoints(0, (c/b));
	setBoundPoints(width, (c/b));
    } else if (b == 0 && a != 0) {
	setBoundPoints((c/a), 0);
	setBoundPoints((c/a), height);
    } else {
	double tmp1, tmp2, tmp3, tmp4;
	tmp1 = (c/b); /* intersect with x = 0 */
	tmp2 = (c/a); /* intersect with y = 0 */
	tmp3 = (c - a*width)/b; 	/* intersect with x = width */
	tmp4 = (c - b*height)/a;	/* intersect with y = height */

	if (tmp1 >= 0 && tmp1 <= height)
	    setBoundPoints(0, tmp1);
	if (tmp2 >= 0 && tmp2 <= width)
	    setBoundPoints(tmp2, 0);
	if (tmp3 >=0 && tmp3 <=height)
	    setBoundPoints(width, tmp3);
	if (tmp4 >= 0 && tmp4 <= width)
	    setBoundPoints(tmp4, height);
    }
} // }}}

void DiagramBisector::setBoundPoints(double x, double y)
{ // {{{
    if (boundStartPointX == -1) {
	boundStartPointX = x;	boundStartPointY = y;
    } else if (boundEndPointX == -1) {
	boundEndPointX = x;	boundEndPointY = y;
    }
} // }}}

void DiagramBisector::draw()
{ // {{{
    if (enable) {
	line.draw(); /* Draw the line consist of two DigramPoint */
	qDebug("!!Bisector of (%d,%d)-(%d,%d) whose two point (%f,%f) (%f,%f) to draw",
		leftPoint->getX(), leftPoint->getY(), 
		rightPoint->getX(), rightPoint->getY(), 
		startPointX, startPointY, endPointX, endPointY); 
	setPoints((int)startPointX, (int)startPointY, 
		(int)endPointX, (int)endPointY);
	setPen(QColor(Qt::blue));
	show();
	canvas()->update();
    }
} // }}}

bool DiagramBisector::isOnLine(double x, double y)
{ // {{{
    /* TODO: type ``double'' bug */
    if ((a*x+b*y-c) == 0)
	return true;
    return false;
} // }}}

bool DiagramBisector::isDiffArea(double xa, double ya, double xb, double yb)
{ // {{{
    if ( ((a*xa+b*ya)-c) * ((a*xb+b*yb)-c) <= 0)
	return true;
    return false;
} // }}}

bool DiagramBisector::isIntersect(DiagramBisector *line)
{ // {{{
    return (!((a*line->getB()) == (line->getA()*b)));
} // }}}

QPair<double, double> DiagramBisector::calIntersectPoint(DiagramBisector *line)
{ // {{{
    double d = (a * line->getB() - line->getA() * b);
    return qMakePair((c * line->getB() - line->getC() * b) / d, 
	    (a * line->getC() - line->getA() * c) / d);
} // }}}

void DiagramBisector::endWithIntersect(double intersectX, double intersectY)
{ // {{{
	endPointX = intersectX;
	endPointY = intersectY;
	isEndINF = false;
	if (isStartINF == true && endPointY < startPointY) {
	    startPointX = endPointX*2-startPointX;
	    startPointY = endPointY*2-startPointY;
	}
} // }}}

void DiagramBisector::addIntersectPoint(double intersectX, double intersectY, 
	DiagramBisector *cutter)
{ // {{{
    intersectPointX = intersectX;
    intersectPointY = intersectY;
    this->cutter = cutter;
} // }}}

void DiagramBisector::cutByIntersect(CutDirection dir)
{ // TODO: debug {{{
    if (cutter == NULL)
	return;

    double a, b, c;
    a = cutter->getA();
    b = cutter->getB();
    c = cutter->getC();

    if ( a < 0) {
	a *= (-1);	b *= (-1);	c *= (-1);
    }

    if (getLineType() == NO_INF) {
	if (dir == CUT_LEFT) {
	    if (startPointX * a + startPointY * b < c) {
		startPointX = intersectPointX;
		startPointY = intersectPointY;
		isStartINF = false;
	    } else if (endPointX * a + endPointY * b < c) {
		endPointX = intersectPointX;
		endPointY = intersectPointY;
		isEndINF = false;
	    }
	} else if (dir == CUT_RIGHT) {
	    if (startPointX * a + startPointY * b > c) {
		startPointX = intersectPointX;
		startPointY = intersectPointY;
		isStartINF = false;
	    } else if (endPointX * a + endPointY * b > c) {
		endPointX = intersectPointX;
		endPointY = intersectPointY;
		isEndINF = false;
	    }
	}
    } else if (getLineType() == ONE_INF) {
	if (dir == CUT_LEFT) {
	    if (isStartINF == false) {
		if (startPointX * a + startPointY * b > c) {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		} else if (startPointX * a + startPointY * b < c) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		} else {
		    if (endPointX * a + endPointY * b < c) {
			endPointX = intersectPointX;
			endPointY = intersectPointY;
			isEndINF = false;
		    }
		}
	    } else {
		if (endPointX * a + endPointY * b > c) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		} else if (endPointX * a + endPointY * b < c) {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		} else {
		    if (startPointX * a + startPointY * b < c) {
			startPointX = intersectPointX;
			startPointY = intersectPointY;
			isStartINF = false;
		    }
		}
	    }
	} else if (dir == CUT_RIGHT) {
	    if (isStartINF == false) {
		if (startPointX * a + startPointY * b < c) {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		} else if (startPointX * a + startPointY * b > c) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		} else {
		    if (endPointX * a + endPointY * b > c) {
			endPointX = intersectPointX;
			endPointY = intersectPointY;
			isEndINF = false;
		    }
		}
	    } else {
		if (endPointX * a + endPointY * b < c) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		} else if (endPointX * a + endPointY * b > c) {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		} else {
		    if (startPointX * a + startPointY * b > c) {
			startPointX = intersectPointX;
			startPointY = intersectPointY;
			isStartINF = false;
		    }
		}
	    }
	}
    } else if (getLineType() == TWO_INF) {
	if (dir == CUT_LEFT) {
	    if (startPointX * a + startPointY * b < c && 
		    endPointX * a + endPointY * b > c) {
		startPointX = intersectPointX;
		startPointY = intersectPointY;
		isStartINF = false;
	    } else if (startPointX * a + startPointY * b > c && 
		    endPointX * a + endPointY * b < c) {
		endPointX = intersectPointX;
		endPointY = intersectPointY;
		isEndINF = false;
	    } else if (startPointX * a + startPointY * b >= c && 
		    endPointX * a + endPointY * b >= c) {
		if (distance(startPointX, startPointY, 
			    intersectPointX, intersectPointY) < 
			distance(endPointX, endPointY, 
			    intersectPointX, intersectPointY)) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		} else {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		}
	    } else if (startPointX * a + startPointY * b <= c && 
		    endPointX * a + endPointY * b <= c) {
		if (distance(startPointX, startPointY, 
			    intersectPointX, intersectPointY) > 
			distance(endPointX, endPointY, 
			    intersectPointX, intersectPointY)) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		    endPointX = startPointX*2-endPointX;
		    endPointY = startPointY*2-endPointY;
		} else {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		    startPointX = endPointX*2-startPointX;
		    startPointY = endPointY*2-startPointY;
		}
	    }
	} else if (dir == CUT_RIGHT) {
	    if (startPointX * a + startPointY * b > c && 
		    endPointX * a + endPointY * b < c) {
		startPointX = intersectPointX;
		startPointY = intersectPointY;
		isStartINF = false;
	    } else if (startPointX * a + startPointY * b < c && 
		    endPointX * a + endPointY * b > c) {
		endPointX = intersectPointX;
		endPointY = intersectPointY;
		isEndINF = false;
	    } else if (startPointX * a + startPointY * b >= c && 
		    endPointX * a + endPointY * b >= c) {
		if (distance(startPointX, startPointY, 
			    intersectPointX, intersectPointY) > 
			distance(endPointX, endPointY, 
			    intersectPointX, intersectPointY)) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		    endPointX = startPointX*2-endPointX;
		    endPointY = startPointY*2-endPointY;
		} else {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		    startPointX = endPointX*2-startPointX;
		    startPointY = endPointY*2-startPointY;
		}
	    } else if (startPointX * a + startPointY * b <= c && 
		    endPointX * a + endPointY * b <= c) {
		if (distance(startPointX, startPointY, 
			    intersectPointX, intersectPointY) < 
			distance(endPointX, endPointY, 
			    intersectPointX, intersectPointY)) {
		    startPointX = intersectPointX;
		    startPointY = intersectPointY;
		    isStartINF = false;
		} else {
		    endPointX = intersectPointX;
		    endPointY = intersectPointY;
		    isEndINF = false;
		}
	    }
	}
    }

    qDebug("!!Bisector of (%d,%d)-(%d,%d) whose two point (%f,%f) (%f,%f) is cut by (%f,%f).", 
	    leftPoint->getX(), leftPoint->getY(), 
	    rightPoint->getX(), rightPoint->getY(), 
	    startPointX, startPointY, endPointX, endPointY, 
	    intersectPointX, intersectPointY);

    /* reset intersect point */
    intersectPointX = -1;	intersectPointY = -1;
    //cutter = NULL;
} // }}}

bool DiagramBisector::operator==(DiagramBisector &rhs) const
{ // {{{
    if ( *(rhs.getLeftPoint()) == *leftPoint
	    && *(rhs.getRightPoint()) == *rightPoint ) {
	return true;
    } else if ( *(rhs.getRightPoint()) == *leftPoint
	    && *(rhs.getLeftPoint()) == *rightPoint ) {
	return true;
    }
    return false;
} // }}}

DiagramBisector::~DiagramBisector()
{ // {{{
    hide();
} // }}}

// }}}
