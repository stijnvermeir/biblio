#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("stijnvsoftware");
	QCoreApplication::setOrganizationDomain("stijn-v.be");
	QCoreApplication::setApplicationName("Biblio");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
