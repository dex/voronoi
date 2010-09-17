#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <qcanvas.h>
#include <qvaluevector.h>
#include <math.h>
#include <stdlib.h>

class QAction;
class QPainter;
class DynamicTip;
class DiagramBisector;

class DiagramPoint : public QCanvasEllipse
{
    public:
	enum { DIAMETER = 6 };
	DiagramPoint(int x, int y, int diameter, QCanvas *canvas);
	~DiagramPoint();
	int getX() const { return posX; };
	int getY() const { return posY; };
	void addEdge(DiagramBisector *edge);
	bool operator==(DiagramPoint &rhs) const;
	bool operator<(const DiagramPoint &rhs) const;
	QValueVector<DiagramBisector*> & getEdgeList() { return edgeList; };

    private:
	int posX, posY;
	QValueVector<DiagramBisector*> edgeList;
};

class DiagramLine : public QCanvasLine
{
    public:
	DiagramLine(DiagramPoint *leftPoint, DiagramPoint *rightPoint,
		QCanvas *canvas);
	~DiagramLine();
	void draw();
	QPair<double, double> getMiddlePoint() const 
	{ return qMakePair(middleX, middleY); };
	double getA() const { return a; };
	double getB() const { return b; };
	double getC() const { return c; };
	DiagramPoint* getLeftPoint() { return leftPoint; };
	DiagramPoint* getRightPoint() { return rightPoint; };
	bool operator==(DiagramLine &rhs) const;
	bool isDiffArea(double xa, double ya, double xb, double yb);

    private:
	double a, b, c;	 /* For ax+by=c */
	double middleX, middleY;
	DiagramPoint *leftPoint, *rightPoint;
};

class DiagramBisector : public QCanvasLine
{
    public:
	enum LineType { NO_INF = 0, ONE_INF = 1, TWO_INF = 2 };
	enum CutDirection { NO_CUT = 0, CUT_LEFT = 1, CUT_RIGHT = 2 };
	DiagramBisector(DiagramLine line, QCanvas *canvas, 
		DiagramPoint* refPoint = NULL, 
		double startX = -1, double startY = -1);
	~DiagramBisector();
	void draw();
	const DiagramLine & getLine() const { return line; };
	DiagramPoint * getRefPoint() const { return refPoint; };
	DiagramPoint * getLeftPoint() { return leftPoint; };
	DiagramPoint * getRightPoint() { return rightPoint; };
	DiagramPoint * getComPoint(DiagramBisector *line);
	double getA() const { return a; };
	double getB() const { return b; };
	double getC() const { return c; };
	LineType getLineType() const;
	bool isStartPointINF() const { return isStartINF; };
	bool isEndPointINF() const { return isEndINF; };
	QPair<double, double> getBoundStartPoint() const
	{ return qMakePair(boundStartPointX, boundStartPointY); };
	QPair<double, double> getBoundEndPoint() const
	{ return qMakePair(boundEndPointX, boundEndPointY); };
	QPair<double, double> getStartPoint() const
	{ return qMakePair(startPointX, startPointY); };
	QPair<double, double> getEndPoint() const
	{ return qMakePair(endPointX, endPointY); };
	bool isOnLine(double x, double y);
	bool isDiffArea(double xa, double ya, double xb, double yb);
	bool isIntersect(DiagramBisector *line);
	QPair<double, double> calIntersectPoint(DiagramBisector *line);
	void endWithIntersect(double intersectX, double intersectY);
	void addIntersectPoint(double intersectX, double intersectY, 
		DiagramBisector *cutter);
	void cutByIntersect(CutDirection dir);
	//bool hasCutter() const { return (!(cutter == NULL)); };
	DiagramBisector* getCutter() { return cutter; };
	bool operator== (DiagramBisector &rhs) const;
	bool isHP() const { return HP; };
	void setHP(bool flag) { HP = flag; };
	void clearCutter() { cutter = NULL; };
	void disable() { enable = false; };
	bool isEnabled() const { return enable; };

    protected:
	void findBoundIntersect(QCanvas *canvas);
	void setBoundPoints(double x, double y);
	double distance(double xa, double ya, double xb, double yb)
	const { return (abs((int)(xb-xa))+abs((int)(yb-ya))); };

    private:
	bool enable;

	double a, b, c;	 /* For ax+by=c */
	DiagramLine line;
	DiagramPoint *leftPoint, *rightPoint;

	DiagramPoint *refPoint;
	double startPointX, startPointY;
	bool isStartINF, isEndINF;
	double endPointX, endPointY;

	double boundStartPointX, boundStartPointY;
	double boundEndPointX, boundEndPointY;

	double intersectPointX, intersectPointY;
	DiagramBisector *cutter;

	bool HP;
};

class DiagramView : public QCanvasView
{
    Q_OBJECT
    public:
	DiagramView(QCanvas *canvas, QWidget *parent = 0,
		const char *name = 0);
	~DiagramView();
	bool hasPoint(const int x, const int y);

    public slots:
	void addPoint(int x, int y);

    protected:
	void contentsContextMenuEvent(QContextMenuEvent *event);
	void contentsMousePressEvent(QMouseEvent *event);
	void contentsMouseMoveEvent(QMouseEvent *event);

    signals:
	void locationChanged(int x, int y);

    public slots:
	void newFile();
	void calculate();

    private:
	void createActions();
	bool isDuplicate(int x, int y);

	QAction *newAct;
	QAction *calAct;
	QValueVector<DiagramPoint*> pointList;
	QValueVector<DiagramBisector*> edgeList;
	DynamicTip *dynTip;
};

template <class T>
class PtrLess {
    public:
	bool operator() (const T &lhs, const T &rhs) const
	{ return ((*lhs) < (*rhs)); };
};

#endif
