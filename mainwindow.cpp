#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdocumentdialog.h"
#include "bibdata.h"

#include <QCoreApplication>
#include <QStringListModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model_()
    , completer_(this)
{
	ui->setupUi(this);
	setWindowTitle(QString("%1 v%2").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()));
	ui->tableView->setModel(&model_);
	ui->tableView->setSortingEnabled(true);

	connect(ui->newBtn, &QPushButton::clicked, [this](bool /*checked*/)
	{
		NewDocumentDialog d(this);
		d.setWindowTitle("Nieuw document");
		auto ret = d.exec();
		if (ret == QDialog::Accepted)
		{
			qDebug() << "Add to DB";
		}
	});

	completer_.setModel(new QStringListModel(BibData::instance().getTags(), &completer_));
	completer_.setCaseSensitivity(Qt::CaseInsensitive);
	completer_.setFilterMode(Qt::MatchContains);
	ui->searchTxt->setCompleter(&completer_);
}

MainWindow::~MainWindow()
{
	delete ui;
}
