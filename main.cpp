#include "hl7reviewer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HL7Reviewer w;
	w.show();
	return a.exec();
}
