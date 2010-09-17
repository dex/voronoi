#include <qcanvas.h>
#include <qvaluevector.h>
#include <qmap.h>

#include "geometry.h"
#include "algorithm.h"
#include "convex.h"

void VoronoiAlgo::start()
{ // {{{
    calculate(pointList);
} // }}}

void VoronoiAlgo::calculate(QValueVector<DiagramPoint*> &pointSet)
{ // {{{
    QValueVector<DiagramPoint*> leftPointSet, rightPointSet;
    int num;

    /* Check for empty set */
    if (pointSet.isEmpty())
	return;

    qDebug("-= enter calculate recursive function =-");
    if ( (num = mapXAxis(pointSet)) == 1) {
	if (pointSet.size() > 1)
	    calHBisector(pointSet);
	return;
    } else {
	split(pointSet, leftPointSet, rightPointSet);
	calculate(leftPointSet);
	calculate(rightPointSet);
	qDebug("start merging ...");
	merge(leftPointSet, rightPointSet, pointSet);
	qDebug("end merging");
	return;
    }
} // }}}

int VoronoiAlgo::mapXAxis(const QValueVector<DiagramPoint*> &pointSet)
{ // {{{
    QValueVector<DiagramPoint*>::const_iterator it;
    QMap<int, int> myMap;
    for (it = pointSet.begin(); it!= pointSet.end(); it++)
	myMap.insert((*it)->getX(), (*it)->getY());
    qDebug("there are %d points map to X Axis.", myMap.size());
    return myMap.size();
} // }}}

void VoronoiAlgo::calHBisector(QValueVector<DiagramPoint*> &pointSet)
{ // {{{
    for( unsigned int i=1; i<pointSet.size(); i++) {
	DiagramBisector *bisector = new DiagramBisector(
		DiagramLine(pointSet[i-1], pointSet[i], 
		    canvasView->canvas()), 
		canvasView->canvas());
	edgeList.push_back(bisector);
	qDebug("bisector between (%d,%d) - (%d,%d) is been added.", 
		bisector->getLeftPoint()->getX(), 
		bisector->getLeftPoint()->getY(), 
		bisector->getRightPoint()->getX(), 
		bisector->getRightPoint()->getY());
    }
    return;
} // }}}

void VoronoiAlgo::split(const QValueVector<DiagramPoint*> &pointSet, 
	QValueVector<DiagramPoint*> &leftPointSet, 
	QValueVector<DiagramPoint*> &rightPointSet)
{ // {{{
    unsigned int sum = 0; 
    double center = 0;

    for (unsigned int i=0; i<pointSet.size(); i++)
	sum += pointSet[i]->getX();
    center = sum/(double)pointSet.size();
    qDebug("the center line is x= %f.",center);

    for (unsigned int j=0; j<pointSet.size(); j++) {
	if ((double)(pointSet[j]->getX()) <= center) {
	    leftPointSet.push_back(pointSet[j]);
	    qDebug("(%d,%d) split to leftPointSet.", 
		    pointSet[j]->getX(), 
		    pointSet[j]->getY());
	} else {
	    rightPointSet.push_back(pointSet[j]);
	    qDebug("(%d,%d) split to rightPointSet.", 
		    pointSet[j]->getX(), 
		    pointSet[j]->getY());
	}
    }
    return;
} // }}}

QPair<DiagramLine, DiagramLine> VoronoiAlgo::findBeginEndLine(
	const QValueVector<DiagramPoint*> &leftPointSet, 
	const QValueVector<DiagramPoint*> &rightPointSet)
{ // {{{
    DiagramPoint *point1, *point2, *point3, *point4;
    ConvexHull leftConvex(leftPointSet);
    ConvexHull rightConvex(rightPointSet);

    bool isChanged = false;

    leftConvex.rightMost();
    rightConvex.leftMost();
    do {
	isChanged = false;
	DiagramLine line(leftConvex.current(), rightConvex.current(), 
		canvasView->canvas());
	double a, b, c;
	a = line.getA();	b = line.getB();	c = line.getC();

	if (b < 0) {
	    a *= (-1);	b *= (-1);	c *= (-1);
	}

	if (a*leftConvex.next()->getX() + b*leftConvex.next()->getY() < c) {
	    leftConvex.forward();
	    isChanged = true;
	} else if (a*leftConvex.prev()->getX() + b*leftConvex.prev()->getY() < c) {
	    leftConvex.backward();
	    isChanged = true;
	}
	if (a*rightConvex.next()->getX() + b*rightConvex.next()->getY() < c) {
	    rightConvex.forward();
	    isChanged = true;
	} else if (a*rightConvex.prev()->getX() + b*rightConvex.prev()->getY() < c) {
	    rightConvex.backward();
	    isChanged = true;
	}
    } while (isChanged);
    point1 = leftConvex.current();	point2 = rightConvex.current();
    
    leftConvex.rightMost();
    rightConvex.leftMost();
    do {
	isChanged = false;
	DiagramLine line(leftConvex.current(), rightConvex.current(), 
		canvasView->canvas());
	double a, b, c;
	a = line.getA();	b = line.getB();	c = line.getC();

	if (b < 0) {
	    a *= (-1);	b *= (-1);	c *= (-1);
	}

	if (a*leftConvex.next()->getX() + b*leftConvex.next()->getY() > c) {
	    leftConvex.forward();
	    isChanged = true;
	} else if (a*leftConvex.prev()->getX() + b*leftConvex.prev()->getY() > c) {
	    leftConvex.backward();
	    isChanged = true;
	}
	if (a*rightConvex.next()->getX() + b*rightConvex.next()->getY() > c) {
	    rightConvex.forward();
	    isChanged = true;
	} else if (a*rightConvex.prev()->getX() + b*rightConvex.prev()->getY() > c) {
	    rightConvex.backward();
	    isChanged = true;
	}
    } while (isChanged);
    point3 = leftConvex.current();	point4 = rightConvex.current();


    /* *BE CAREFUL* Easy to return a invalid memery address */
    qDebug("BeginLine:(%d,%d)-(%d,%d), EndLine:(%d,%d)-(%d,%d).", 
	    point1->getX(), point1->getY(), 
	    point2->getX(), point2->getY(), 
	    point3->getX(), point3->getY(), 
	    point4->getX(), point4->getY()); 
    return qMakePair(
	    DiagramLine(point1, point2, 
		canvasView->canvas()), 
	    DiagramLine(point3, point4, 
		canvasView->canvas()));
} // }}}

void VoronoiAlgo::merge(const QValueVector<DiagramPoint*> &leftPointSet, 
	const QValueVector<DiagramPoint*> &rightPointSet, 
	QValueVector<DiagramPoint*> &pointSet)
{ // {{{
    debug("merge size is %d:%d", leftPointSet.size(), rightPointSet.size());
    DiagramBisector *curBisector = NULL;
    QValueVector<DiagramBisector*> leftSetNeedCut, rightSetNeedCut;
    QValueVector<DiagramBisector*> HPSet;
    leftSetNeedCut.clear();	rightSetNeedCut.clear();
    unsigned int bisectorCount = 0;

    /* find the Begin and End lines which prepared to find HP */
    QPair<DiagramLine, DiagramLine> foundLines(
	    findBeginEndLine(leftPointSet, rightPointSet));

    /* start find the HP from the upper common line */
    curBisector = new DiagramBisector(
	    DiagramLine(foundLines.first.getLeftPoint(), 
		foundLines.first.getRightPoint(), 
		canvasView->canvas()), 
	    canvasView->canvas());
    edgeList.push_back(curBisector);
    curBisector->setHP(true);
    HPSet.push_back(curBisector);
    qDebug("first bisector between (%d,%d) - (%d,%d) is been added.", 
	    curBisector->getLeftPoint()->getX(), 
	    curBisector->getLeftPoint()->getY(), 
	    curBisector->getRightPoint()->getX(), 
	    curBisector->getRightPoint()->getY());

    /* find out the others lien to construct HP */
    while ( ! (curBisector->getLine() == foundLines.second)) {
	int leftIntersectNum = 0, rightIntersectNum = 0;
	double leftCandidatePointX = -1, leftCandidatePointY = -1;
	double rightCandidatePointX = -1, rightCandidatePointY = -1;
	DiagramBisector *leftCandidateBisector = NULL;
	DiagramBisector *rightCandidateBisector = NULL;
	DiagramPoint *leftPoint = NULL;
	DiagramPoint *rightPoint = NULL;

	if (curBisector->getLeftPoint()->getX() < 
		curBisector->getRightPoint()->getX()) {
	    leftPoint = curBisector->getLeftPoint();
	    rightPoint = curBisector->getRightPoint();
	} else {
	    leftPoint = curBisector->getRightPoint();
	    rightPoint = curBisector->getLeftPoint();
	}


	/* find out the left set intersect point with smallest y-value */
	QValueVector<DiagramBisector*> &leftEdgeList = 
	    leftPoint->getEdgeList();
	if (!leftEdgeList.isEmpty()) {
	    for (unsigned int i=0; i<leftEdgeList.size(); i++) {
		if (curBisector->isIntersect(leftEdgeList[i]) && 
			//!leftEdgeList[i]->hasCutter() && 
			(leftEdgeList[i]->getCutter() == NULL || 
			 !(leftEdgeList[i]->getComPoint(leftEdgeList[i]->getCutter()) 
			 == curBisector->getRefPoint())) && 
			!leftEdgeList[i]->isHP()) {  
		    QPair<double, double> intersectPoint(
			    curBisector->calIntersectPoint(
				leftEdgeList[i]));
		    if (leftIntersectNum == 0) {
			if (*curBisector == *(HPSet[0]) || 
				intersectPoint.second >= 
				curBisector->getStartPoint().second) {
			    leftIntersectNum++;
			    leftCandidatePointX = intersectPoint.first;
			    leftCandidatePointY = intersectPoint.second;
			    leftCandidateBisector = leftEdgeList[i];
			}
		    } else {
			leftIntersectNum++;
			if ((*curBisector == *(HPSet[0]) || 
				intersectPoint.second >= 
				curBisector->getStartPoint().second) && 
				intersectPoint.second < leftCandidatePointY) {
			    leftCandidatePointX = intersectPoint.first;
			    leftCandidatePointY = intersectPoint.second;
			    leftCandidateBisector = leftEdgeList[i];
			}
		    }
		}
	    }
	}

	/* find out the right set intersect point with smallest y-value */
	QValueVector<DiagramBisector*> &rightEdgeList = 
	    rightPoint->getEdgeList();
	if (!rightEdgeList.isEmpty()) {
	    for (unsigned int i=0; i<rightEdgeList.size(); i++) {
		if (curBisector->isIntersect(rightEdgeList[i]) && 
			//!rightEdgeList[i]->hasCutter() && 
			(rightEdgeList[i]->getCutter() == NULL || 
			 !(rightEdgeList[i]->getComPoint(rightEdgeList[i]->getCutter()) 
			 == curBisector->getRefPoint())) && 
			!rightEdgeList[i]->isHP()) {  
		    QPair<double, double> intersectPoint(
			    curBisector->calIntersectPoint(
				rightEdgeList[i]));
		    if (rightIntersectNum == 0) {
			if (*curBisector == *(HPSet[0]) || 
				intersectPoint.second >= 
				curBisector->getStartPoint().second) {
			    rightIntersectNum++;
			    rightCandidatePointX = intersectPoint.first;
			    rightCandidatePointY = intersectPoint.second;
			    rightCandidateBisector = rightEdgeList[i];
			}
		    } else {
			rightIntersectNum++;
			if ((*curBisector == *(HPSet[0]) || 
				intersectPoint.second >= 
				curBisector->getStartPoint().second) && 
				intersectPoint.second < rightCandidatePointY) {
			    rightCandidatePointX = intersectPoint.first;
			    rightCandidatePointY = intersectPoint.second;
			    rightCandidateBisector = rightEdgeList[i];
			}
		    }
		}
	    }
	}

	/* decide which point is the best intersect */
	double candidatePointX = -1, candidatePointY = -1;
	DiagramBisector* candidateBisector = NULL;
	enum { LEFT = 0, RIGHT = 1 } dir = LEFT;  

	if (leftIntersectNum > 0 && rightIntersectNum == 0) {
	    candidatePointX = leftCandidatePointX;
	    candidatePointY = leftCandidatePointY;
	    candidateBisector = leftCandidateBisector;
	    dir = LEFT;
	} else if (leftIntersectNum == 0 && rightIntersectNum > 0) {
	    candidatePointX = rightCandidatePointX;
	    candidatePointY = rightCandidatePointY;
	    candidateBisector = rightCandidateBisector;
	    dir = RIGHT;
	} else if (leftIntersectNum > 0 && rightIntersectNum > 0) {
	    if (leftCandidatePointY < rightCandidatePointY) {
		candidatePointX = leftCandidatePointX;
		candidatePointY = leftCandidatePointY;
		candidateBisector = leftCandidateBisector;
		dir = LEFT;
	    } else {
		candidatePointX = rightCandidatePointX;
		candidatePointY = rightCandidatePointY;
		candidateBisector = rightCandidateBisector;
		dir = RIGHT;
	    }
	} else {
	    qFatal("Find the intersect failed when finding HP.");
	}
	qDebug("intersect with bisector of (%d,%d) - (%d,%d).", 
		candidateBisector->getLeftPoint()->getX(), 
		candidateBisector->getLeftPoint()->getY(), 
		candidateBisector->getRightPoint()->getX(), 
		candidateBisector->getRightPoint()->getY());

	/* cut line with intersect point */
	curBisector->endWithIntersect(candidatePointX, 
		candidatePointY);
	candidateBisector->addIntersectPoint(candidatePointX, 
		candidatePointY, curBisector);
	if ( dir == LEFT) {
	    leftSetNeedCut.push_back(candidateBisector);
	    candidateBisector->cutByIntersect(
		    DiagramBisector::CUT_RIGHT);
	} else {
	    rightSetNeedCut.push_back(candidateBisector);
	    candidateBisector->cutByIntersect(
		    DiagramBisector::CUT_LEFT);
	}

	/* find the reference point */
	DiagramPoint *refPoint = 
	    curBisector->getComPoint(candidateBisector);
	qDebug("refPoint(%d,%d)", 
		refPoint->getX(), refPoint->getY());

	/* find out two points to consturct the new bisector */
	DiagramPoint *newLeftPoint = NULL;
	DiagramPoint *newRightPoint = NULL;

	if ( *(candidateBisector->getLeftPoint()) == *refPoint)
	    newLeftPoint = candidateBisector->getRightPoint();
	else
	    newLeftPoint = candidateBisector->getLeftPoint();

	if ( *(curBisector->getLeftPoint()) == *refPoint)
	    newRightPoint = curBisector->getRightPoint();
	else
	    newRightPoint = curBisector->getLeftPoint();

	/* add the new bisector */
	bisectorCount++;
	DiagramBisector *newBisector = new DiagramBisector(
		DiagramLine(newLeftPoint, newRightPoint, 
		    canvasView->canvas()), 
		canvasView->canvas(), 
		refPoint, 
		candidatePointX, candidatePointY);
	edgeList.push_back(newBisector);
	newBisector->setHP(true);
	HPSet.push_back(newBisector);
	qDebug("New bisector between (%d,%d) - (%d,%d) is been added.", 
		newBisector->getLeftPoint()->getX(), 
		newBisector->getLeftPoint()->getY(), 
		newBisector->getRightPoint()->getX(), 
		newBisector->getRightPoint()->getY());

	curBisector = newBisector;

	if (bisectorCount > pointSet.size())
	    qFatal("infinity loop in merge...");
    }

    /* clear the line existed at the right side */
    for (unsigned int i=0; i<leftPointSet.size(); i++) {
	QValueVector<DiagramBisector*> &edgeList = 
	    leftPointSet[i]->getEdgeList();
	for (unsigned int j=0; j<edgeList.size(); j++) {
	    if (edgeList[j]->getLineType() != DiagramBisector::TWO_INF && 
		    edgeList[j]->getCutter() == NULL && 
		    edgeList[j]->isEnabled() && 
		    !edgeList[j]->isHP()) {
		double xValue = -1, yValue = -1;
		double xAltValue = -1, yAltValue = -1;
		if (edgeList[j]->getStartPoint().first < 
			edgeList[j]->getEndPoint().first ||
			!edgeList[j]->isStartPointINF()) {
		    xValue = edgeList[j]->getStartPoint().first;
		    yValue = edgeList[j]->getStartPoint().second;
		    xAltValue = edgeList[j]->getEndPoint().first;
		    yAltValue = edgeList[j]->getEndPoint().second;
		} else {
		    xValue = edgeList[j]->getEndPoint().first;
		    yValue = edgeList[j]->getEndPoint().second;
		    xAltValue = edgeList[j]->getStartPoint().first;
		    yAltValue = edgeList[j]->getStartPoint().second;
		}
		for (unsigned int k=0; k<HPSet.size(); k++) {
		    DiagramBisector *line = HPSet[k];
		    if ((yValue <= line->getStartPoint().second && 
				yValue >= line->getEndPoint().second) || 
			    (yValue >= line->getStartPoint().second && 
			     yValue <= line->getEndPoint().second)) {
			double a = line->getA(),
			       b = line->getB(),
			       c = line->getC();
			if (a < 0) {
			    a*=(-1);	b*=(-1);	c*=(-1);
			}
			if (a*xValue + b*yValue > c || 
				(a*xValue + b*yValue == c && 
				 a*xAltValue + b*yAltValue > c)) {
			    edgeList[j]->disable();
			    qDebug("Disabled the bisector of (%d,%d) - (%d,%d).", 
				    edgeList[j]->getLeftPoint()->getX(), 
				    edgeList[j]->getLeftPoint()->getY(), 
				    edgeList[j]->getRightPoint()->getX(), 
				    edgeList[j]->getRightPoint()->getY());
			    break;
			}
		    }
		}
	    }
	}
    }

    /* clear the line existed at the left side */
    for (unsigned int p=0; p<rightPointSet.size(); p++) {
	QValueVector<DiagramBisector*> &edgeList = 
	    rightPointSet[p]->getEdgeList();
	for (unsigned int j=0; j<edgeList.size(); j++) {
	    if (edgeList[j]->getLineType() != DiagramBisector::TWO_INF && 
		    edgeList[j]->getCutter() == NULL && 
		    edgeList[j]->isEnabled() && 
		    !edgeList[j]->isHP()) {
		double xValue = -1, yValue = -1;
		double xAltValue = -1, yAltValue = -1;
		if (edgeList[j]->getStartPoint().first > 
			edgeList[j]->getEndPoint().first || 
			!edgeList[j]->isStartPointINF()) {
		    xValue = edgeList[j]->getStartPoint().first;
		    yValue = edgeList[j]->getStartPoint().second;
		    xAltValue = edgeList[j]->getEndPoint().first;
		    yAltValue = edgeList[j]->getEndPoint().second;
		} else {
		    xValue = edgeList[j]->getEndPoint().first;
		    yValue = edgeList[j]->getEndPoint().second;
		    xAltValue = edgeList[j]->getStartPoint().first;
		    yAltValue = edgeList[j]->getStartPoint().second;
		}
		for (unsigned int k=0; k<HPSet.size(); k++) {
		    DiagramBisector *line = HPSet[k];
		    if ((yValue <= line->getStartPoint().second && 
				yValue >= line->getEndPoint().second) || 
			    (yValue >= line->getStartPoint().second && 
			     yValue <= line->getEndPoint().second)) {
			double a = line->getA(),
			       b = line->getB(),
			       c = line->getC();
			if (a < 0) {
			    a*=(-1);	b*=(-1);	c*=(-1);
			}
			if (a*xValue + b*yValue < c || 
				(a*xValue + b*yValue == c && 
				 a*xAltValue + b*yAltValue < c)) {
			    edgeList[j]->disable();
			    qDebug("Disabled the bisector of (%d,%d) - (%d,%d).", 
				    edgeList[j]->getLeftPoint()->getX(), 
				    edgeList[j]->getLeftPoint()->getY(), 
				    edgeList[j]->getRightPoint()->getX(), 
				    edgeList[j]->getRightPoint()->getY());
			    break;
			}
		    }
		}
	    }
	}
    }

    /* clear the cutter */
    for (unsigned int m=0; m<leftSetNeedCut.size(); m++)
	leftSetNeedCut[m]->clearCutter();
    for (unsigned int n=0; n<rightSetNeedCut.size(); n++)
	rightSetNeedCut[n]->clearCutter();
    /* clean HP flag */
    for (unsigned int k=0; k<HPSet.size(); k++)
	HPSet[k]->setHP(false);
    return; 
} // }}}
