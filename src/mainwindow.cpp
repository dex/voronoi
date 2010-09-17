#include <qaction.h>
#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcanvas.h>
#include <qdir.h>
#include <qlabel.h>
#include <qstatusbar.h>

#include "mainwindow.h"
#include "geometry.h"
#include "inputdialog.h"

MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name)
{ // {{{

    canvas = new QCanvas(this, "canvas");
    canvas->resize(640,480);
    canvasView = new DiagramView(canvas, this, "canvasView");
    canvasView->setResizePolicy(QScrollView::AutoOneFit);
    setCentralWidget(canvasView);
    inputDialog = NULL;

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
} // }}}

void MainWindow::createActions()
{ // {{{
    newAct = new QAction(tr("&New"), tr("Ctrl+N"), this);
    newAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/filenew.png")));
    connect(newAct, SIGNAL(activated()), this, SLOT(newFile()));
    exitAct = new QAction(tr("E&xit"), tr("Ctrl+Q"), this);
    exitAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/exit.png")));
    connect(exitAct, SIGNAL(activated()), this, SLOT(exit()));
    inputAct = new QAction(tr("&Input"), tr("Ctrl+I"), this);
    inputAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/input.png")));
    connect(inputAct, SIGNAL(activated()), this, SLOT(input()));
    calAct = new QAction(tr("&Run"), tr("Ctrl+R"), this);
    calAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/run.png")));
    connect(calAct, SIGNAL(activated()), this, SLOT(calculate()));
    licenseAct = new QAction(tr("&License"), 0, this);
    licenseAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/license.png")));
    connect(licenseAct, SIGNAL(activated()), this, SLOT(license()));
    aboutQtAct = new QAction(tr("About &Qt"), 0, this);
    aboutQtAct->setIconSet(QPixmap::fromMimeSource(
		QDir::convertSeparators("images/qt.png")));
    connect(aboutQtAct, SIGNAL(activated()), this, SLOT(aboutQt()));
} // }}}

void MainWindow::createMenus()
{ // {{{
    fileMenu = new QPopupMenu(this);
    newAct->addTo(fileMenu);
    fileMenu->insertSeparator();
    exitAct->addTo(fileMenu);
    actionMenu = new QPopupMenu(this);
    inputAct->addTo(actionMenu);
    calAct->addTo(actionMenu);
    helpMenu = new QPopupMenu(this);
    licenseAct->addTo(helpMenu);
    aboutQtAct->addTo(helpMenu);

    menuBar()->insertItem(tr("&File"), fileMenu);
    menuBar()->insertItem(tr("&Action"), actionMenu);
    menuBar()->insertItem(tr("&Help"), helpMenu);
} // }}}

void MainWindow::createToolBars()
{ // {{{
    mainToolBar = new QToolBar(tr("Main"), this);
    newAct->addTo(mainToolBar);
    exitAct->addTo(mainToolBar);
    mainToolBar->addSeparator();
    inputAct->addTo(mainToolBar);
    calAct->addTo(mainToolBar);
    mainToolBar->addSeparator();
    licenseAct->addTo(mainToolBar);
    aboutQtAct->addTo(mainToolBar);

} // }}}

void MainWindow::createStatusBar()
{ // {{{
    locationLabel = new QLabel("XXXX, XXXX", this);
    locationLabel->setAlignment(AlignCenter);
    locationLabel->setMinimumSize(locationLabel->sizeHint());
    locationLabel->clear();

    statusBar()->addWidget(locationLabel);

    connect(canvasView, SIGNAL(locationChanged(int, int)),
	    this, SLOT(updateStatusBar(int, int)));

} // }}}

void MainWindow::updateStatusBar(int x, int y)
{ // {{{
    locationLabel->setText(QString("%1,%2").arg(x).arg(y));
} // }}}

void MainWindow::newFile()
{ // {{{
    ((DiagramView *)centralWidget())->newFile();
} // }}}

void MainWindow::license()
{ // {{{
    QString text = 
	"<b> License </b><br/>"
	"Copyright (c) 2004<br/>"
	"Chien-Hua Chen (M933040015)<br/>"
	"All right reserved<br/>";
    QMessageBox::about(this, tr("License"), tr(text));
} // }}}

void MainWindow::aboutQt()
{ // {{{
    QMessageBox::aboutQt(this);
} // }}}

void MainWindow::exit()
{ // {{{
    ((QApplication *)parent())->closeAllWindows();
} // }}}

void MainWindow::calculate()
{ // {{{
    ((DiagramView *)centralWidget())->calculate();
} // }}}

void MainWindow::input()
{ // {{{
    if (!inputDialog) {
	inputDialog = new InputDialog(this);
	connect(inputDialog, SIGNAL(addPoint(int, int)), 
		canvasView, SLOT(addPoint(int, int)));
    }

    if (inputDialog->isHidden()) {
	inputDialog->show();
    } else {
	inputDialog->raise();
	inputDialog->setActiveWindow();
    }
} // }}}
