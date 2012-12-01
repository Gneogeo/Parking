#define WIN32_LEAN_AND_MEAN
#include <QtGui/QApplication>
#include "parking.h"



int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	parking w;
	w.show();
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}
