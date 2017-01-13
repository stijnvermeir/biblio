#pragma once

#include <QDialog>
#include <QVariant>
#include <QCompleter>

namespace Ui {
class NewDocumentDialog;
}

class NewDocumentDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewDocumentDialog(const QVariant& documentId = QVariant(), QWidget *parent = 0);
	~NewDocumentDialog();

	virtual void accept();

private:
	Ui::NewDocumentDialog *ui;
	QCompleter completer_;
	QVariant documentId_;
};
