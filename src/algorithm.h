#ifndef VORONOI_ALOG_H
#define VORONOI_ALOG_H

#include <qcanvas.h>
#include "geometry.h"

class VoronoiAlgo 
{
    public:
	VoronoiAlgo(QValueVector<DiagramPoint*> &pointList, 
		QValueVector<DiagramBisector*> &edgeList, 
		DiagramView *canvasView)
	    : pointList(pointList), edgeList(edgeList), canvasView(canvasView)
	    {} ;
	void start();

    protected:
	void calculate(QValueVector<DiagramPoint*> &pointSet);
	int mapXAxis(const QValueVector<DiagramPoint*> &pointSet);
	void calHBisector(QValueVector<DiagramPoint*> &pointSet);
	void split(const QValueVector<DiagramPoint*> &pointSet, 
		QValueVector<DiagramPoint*> &leftPointSet, 
		QValueVector<DiagramPoint*> &rightPointSet);
	QPair<DiagramLine, DiagramLine> findBeginEndLine(
		const QValueVector<DiagramPoint*> &leftPointSet, 
		const QValueVector<DiagramPoint*> &rightPointSet);
	void merge(const QValueVector<DiagramPoint*> &leftPointSet, 
		const QValueVector<DiagramPoint*> &rightPointSet, 
		QValueVector<DiagramPoint*> &pointSet);

    private:
	QValueVector<DiagramPoint*> &pointList;
	QValueVector<DiagramBisector*> &edgeList;
	DiagramView  *canvasView;
};

#endif
