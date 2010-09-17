#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>

#include "mainwindow.h"
#include "geometry.h"

void myMessageOutput( QtMsgType type, const char *msg )
{
    switch ( type ) {
	case QtDebugMsg:
	    fprintf( stderr, "Debug: %s\n", msg );
	    break;
	case QtWarningMsg:
	    fprintf( stderr, "Warning: %s\n", msg );
	    break;
	case QtFatalMsg:
	    fprintf( stderr, "Fatal: %s\n", msg );
	    abort();                    // deliberately core dump
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler( myMessageOutput );
    QApplication app(argc, argv);
    MainWindow mainWin;
    app.setMainWidget(&mainWin);
    mainWin.show();
    return app.exec();
}
