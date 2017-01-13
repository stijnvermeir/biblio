#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdocumentdialog.h"
#include "bibdata.h"

#include <QCoreApplication>
#include <QStringListModel>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model_()
    , completer_(this)
    , completerModel_(new QStringListModel(BibData::instance().getTags(), &completer_))
{
	ui->setupUi(this);
	setWindowTitle(QString("%1 v%2").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()));
	ui->tableView->setModel(&model_);
	ui->tableView->setSortingEnabled(true);
	ui->tableView->resizeColumnsToContents();

	connect(ui->newBtn, &QPushButton::clicked, [this](bool /*checked*/)
	{
		NewDocumentDialog d(QVariant(), this);
		auto ret = d.exec();
		if (ret == QDialog::Accepted)
		{
			model_.invalidate();
			completerModel_->setStringList(BibData::instance().getTags());
		}
	});

	connect(ui->editBtn, &QPushButton::clicked, [this](bool /*checked*/)
	{
		QVariant documentId = model_.mapToSource(ui->tableView->currentIndex()).data(Qt::UserRole);
		if (!documentId.isNull())
		{
			NewDocumentDialog d(documentId, this);
			auto ret = d.exec();
			if (ret == QDialog::Accepted)
			{
				model_.invalidate();
				completerModel_->setStringList(BibData::instance().getTags());
			}
		}
	});

	connect(ui->deleteBtn, &QPushButton::clicked, [this](bool /*checked*/)
	{
		QVariant documentId = model_.mapToSource(ui->tableView->currentIndex()).data(Qt::UserRole);
		if (!documentId.isNull())
		{
			auto ret = QMessageBox::question(this, "Opgelet!", "Ben je zeker dat je dit document wil verwijderen?");
			if (ret == QMessageBox::Yes)
			{
				BibData::instance().deleteDocument(documentId.toInt());
				model_.invalidate();
			}
		}
	});

	connect(ui->searchTxt, &QLineEdit::textChanged, [this](const QString& text)
	{
		model_.setSearchTerm(text);
	});

	connect(ui->tableView, &QTableView::doubleClicked, [this](const QModelIndex& index)
	{
		QVariant documentId = index.data(Qt::UserRole);
		if (!documentId.isNull())
		{
			QVariant row = BibData::instance().findRow(documentId.toInt());
			if (!row.isNull())
			{
				auto file = BibData::instance().get(row.toInt(), BibColumn::File).toString();
				if (!file.isEmpty())
				{
					QDesktopServices::openUrl(QUrl::fromLocalFile(file));
				}
			}
		}
	});

	completer_.setModel(completerModel_);
	completer_.setCaseSensitivity(Qt::CaseInsensitive);
	completer_.setFilterMode(Qt::MatchContains);
	ui->searchTxt->setCompleter(&completer_);
}

MainWindow::~MainWindow()
{
	delete ui;
}
