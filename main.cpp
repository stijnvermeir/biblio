#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("stijnvsoftware");
	QCoreApplication::setOrganizationDomain("stijn-v.be");
	QCoreApplication::setApplicationName("Biblio");
	QCoreApplication::setApplicationVersion(BIBLIO_VERSION);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
