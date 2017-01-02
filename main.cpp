#include "hl7reviewer.h"
#include <QtGui/QApplication>
#include "config/ConfigManager.h"

static const QString ORG_NAME      =  "Mindray";
static const QString APP_NAME      =  "HL7Viewer";



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HL7Reviewer w;
	w.resize(1024, 750);
	w.show();

	ConfigManager::ReleaseInstance();
	return a.exec();
}
