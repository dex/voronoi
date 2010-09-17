#include <qvaluevector.h>

#include "geometry.h"
#include "convex.h"

ConvexHull::ConvexHull(const QValueVector<DiagramPoint*> &pointSet)
{ // {{{
    if(!calConvexHull(pointSet))
	qFatal("construct Convex Hull failed.");
} // }}}

bool ConvexHull::calConvexHull(const QValueVector<DiagramPoint*> &pointSet)
{
    qDebug("there are %d points in this convex hull",pointSet.size());
    DiagramPoint* first = NULL;
    curPos = 0;

    /* if this point set has 1~3 point(s) */
    if (pointSet.size() <= 3) { 
	for (unsigned int i=0; i<pointSet.size(); i++) {
	    convexPointSet.push_back(pointSet[i]);
	    qDebug("(%d,%d) has been added into convexPointSet", 
		    pointSet[i]->getX(), pointSet[i]->getY());
	}
	return true;
    }

    /* if all points in a line */
    //QValueVector<DiagramBisector*> &degeList =

    /* find the start point */
    for (unsigned int i=0; i<pointSet.size(); i++) {
	bool hasInfLine = false;
	QValueVector<DiagramBisector*> &edgeList = 
	    pointSet[i]->getEdgeList();

	for (unsigned int j=0; j<edgeList.size(); j++) {
	    if (edgeList[j]->getLineType() == DiagramBisector::ONE_INF &&
		    edgeList[j]->isEnabled()) {
		hasInfLine = true;
		break;
	    }
	}

	if (hasInfLine) {
	    first = pointSet[i];
	    break;
	}
    }

    if (first == NULL)
	return false;

    /* find out the rest points which construct the convex hull */
    /* TODO: debug */
    DiagramPoint* prevPoint = NULL;
    DiagramPoint* curPoint = first;
    unsigned int pointCount = 0;

    do {
	QValueVector<DiagramBisector*> &edgeList = curPoint->getEdgeList();
	bool hasCandidate = false;
	pointCount++;

	for (unsigned int i=0; i<edgeList.size(); i++) {
	    if (edgeList[i]->getLineType() == DiagramBisector::ONE_INF && 
		    edgeList[i]->isEnabled()) {
		DiagramPoint *leftPoint = edgeList[i]->getLeftPoint();
		DiagramPoint *rightPoint = edgeList[i]->getRightPoint();

		if (*leftPoint == *curPoint) { /* rightPoint is candidate */
		    if (prevPoint == NULL || !(*rightPoint == *prevPoint)) {
			convexPointSet.push_back(rightPoint);
			prevPoint = curPoint;
			curPoint = rightPoint;
			qDebug("(%d,%d) has been added into convexPointSet", 
				rightPoint->getX(), rightPoint->getY());
			hasCandidate = true;
			break;
		    }
		} else { /* leftPoint is candidate */
		    if (prevPoint == NULL || !(*leftPoint == *prevPoint)) {
			convexPointSet.push_back(leftPoint);
			prevPoint = curPoint;
			curPoint = leftPoint;
			qDebug("(%d,%d) has been added into convexPointSet", 
				leftPoint->getX(), leftPoint->getY());
			hasCandidate = true;
			break;
		    }
		}
	    }
	}

	if (!hasCandidate)
	    qFatal("(%d,%d) has %d edges, but can't be a convex point.",
		    curPoint->getX(), curPoint->getY(), edgeList.size());
	if (pointCount > pointSet.size())
	    qFatal("(%d,%d) has %d edges, but can't be a convex point.",
		    curPoint->getX(), curPoint->getY(), edgeList.size());
	
    } while (!(*curPoint == *first));

    return true;
}

DiagramPoint* ConvexHull::current()
{ // {{{
    return convexPointSet[curPos];
} // }}}

DiagramPoint* ConvexHull::backward()
{ // {{{
    int size = convexPointSet.size();
    curPos = (curPos+size-1)%size;
    return current();
} // }}}

DiagramPoint* ConvexHull::prev()
{ // {{{
    int size = convexPointSet.size();
    return convexPointSet[(curPos+size-1)%size];
} // }}}

DiagramPoint* ConvexHull::forward()
{ // {{{
    int size = convexPointSet.size();
    curPos = (curPos+1)%size;
    return current();
} // }}}

DiagramPoint* ConvexHull::next()
{ // {{{
    int size = convexPointSet.size();
    return convexPointSet[(curPos+1)%size];
} // }}}

DiagramPoint* ConvexHull::leftMost()
{ // {{{
    unsigned int minPos = 0;
    for (unsigned int i = 1; i<convexPointSet.size(); i++) {
	if (convexPointSet[i]->getX() < convexPointSet[minPos]->getX())
	    minPos = i;
    }
    curPos = minPos;
    return current();
} // }}}

DiagramPoint* ConvexHull::rightMost()
{ // {{{
    unsigned int maxPos = 0;
    for (unsigned int i = 1; i<convexPointSet.size(); i++) {
	if (convexPointSet[i]->getX() > convexPointSet[maxPos]->getX())
	    maxPos = i;
    }
    curPos = maxPos;
    return current();
} // }}}
