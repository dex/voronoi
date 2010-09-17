#ifndef CONVEX_H
#define CONVEX_H

class ConvexHull
{
    public:
	ConvexHull(const QValueVector<DiagramPoint*> &pointSet);
	DiagramPoint* current();
	DiagramPoint* backward();
	DiagramPoint* prev();
	DiagramPoint* forward();
	DiagramPoint* next();
	DiagramPoint* leftMost();
	DiagramPoint* rightMost();

    private:
	bool calConvexHull(const QValueVector<DiagramPoint*> &pointSet);

	QValueVector<DiagramPoint*> convexPointSet;
	int curPos;
};

#endif
