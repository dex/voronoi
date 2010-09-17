/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <qvalidator.h>

void InputDialog::init()
{
    QRegExp regExp("0|([1-9][0-9]*)");
    xValueEdit->setValidator(new QRegExpValidator(regExp, this));
    yValueEdit->setValidator(new QRegExpValidator(regExp, this));
}

void InputDialog::addClicked()
{
    if (xValueEdit->hasAcceptableInput() && 
	    yValueEdit->hasAcceptableInput()) {
	qDebug("input (%d, %d)", 
		xValueEdit->text().toInt(), 
		yValueEdit->text().toInt());
	emit addPoint(xValueEdit->text().toInt(), 
		yValueEdit->text().toInt());
    }
}
