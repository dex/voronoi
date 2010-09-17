#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class QAction;
class QLabel;
class QCanvas;
class QCanvasView;
class InputDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
	MainWindow(QWidget *parent = 0, const char *name = 0);

    public slots:
	void updateStatusBar(int x, int y);

    private slots:
	void newFile();
    	void calculate();
    	void license();
	void aboutQt();
	void exit();
	void input();

    private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();

	QCanvasView *canvasView;
	QCanvas *canvas;

	QPopupMenu *fileMenu;
	QPopupMenu *actionMenu;
	QPopupMenu *helpMenu;
	QAction *newAct;
	QAction *exitAct;
	QAction *calAct;
	QAction *inputAct;
	QAction *licenseAct;
	QAction *aboutQtAct;
	QToolBar *mainToolBar;
	QLabel *locationLabel;
	InputDialog * inputDialog;
};
#endif
