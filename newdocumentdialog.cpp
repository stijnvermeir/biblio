#include "newdocumentdialog.h"
#include "ui_newdocumentdialog.h"
#include "bibdata.h"

NewDocumentDialog::NewDocumentDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewDocumentDialog)
    , completer_(this)
    , documentId_()
{
	ui->setupUi(this);
	setWindowTitle("Nieuw document");
}

NewDocumentDialog::NewDocumentDialog(int documentId, QWidget *parent)
    : NewDocumentDialog(parent)
{
	documentId_ = documentId;
	setWindowTitle("Bewerk document");
	auto rowId = BibData::instance().findRow(documentId);
	if (rowId.isValid())
	{
		ui->nameTxt->setText(BibData::instance().get(rowId.toInt(), BibColumn::Name).toString());
		ui->boxNrSbx->setValue(BibData::instance().get(rowId.toInt(), BibColumn::BoxNr).toInt());
		ui->fileTxt->setText(BibData::instance().get(rowId.toInt(), BibColumn::File).toString());
		ui->tagsEdit->setPlainText(BibData::instance().get(rowId.toInt(), BibColumn::Tags).toStringList().join("\n"));
	}
}

NewDocumentDialog::~NewDocumentDialog()
{
	delete ui;
}

void NewDocumentDialog::accept()
{
	if (documentId_.isNull())
	{
		BibData::instance().insertDocument(ui->nameTxt->text(),
		                                   ui->boxNrSbx->value(),
		                                   ui->fileTxt->text(),
		                                   ui->tagsEdit->document()->toPlainText().split("\n", QString::SkipEmptyParts));
	}
	else
	{

	}

	QDialog::accept();
}
