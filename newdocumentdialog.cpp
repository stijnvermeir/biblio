#include "newdocumentdialog.h"
#include "ui_newdocumentdialog.h"
#include "bibdata.h"

#include <QStyledItemDelegate>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

namespace {

class TagsItemDelegate : public QStyledItemDelegate
{
public:
	TagsItemDelegate(QObject* parent = Q_NULLPTR)
	    : QStyledItemDelegate(parent)
	{
	}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
	{
		QLineEdit* widget = new QLineEdit(parent);
		QCompleter* completer = new QCompleter(BibData::instance().getTags(), widget);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		completer->setFilterMode(Qt::MatchContains);
		widget->setCompleter(completer);
		widget->setPlaceholderText("Typ hier om een nieuw kernwoord toe te voegen ...");
		return widget;
	}


	void setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		QLineEdit* widget = static_cast<QLineEdit*>(editor);
		if (widget)
		{
			widget->setText(index.data().toString());
		}
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		QLineEdit* widget = static_cast<QLineEdit*>(editor);
		if (widget)
		{
			widget->setText(widget->text().trimmed());
			if (!widget->text().isEmpty())
			{
				// qDebug() << "Checking if " << widget->text() << " exists or not ...";
				if (!BibData::instance().doesTagExist(widget->text()))
				{
					auto result = QMessageBox::question(widget, "Ongekend kernwoord", QString("Het kernwoord '%1' is nog niet bekend. Wil je het toevoegen?").arg(widget->text()));
					if (result == QMessageBox::Yes)
					{
						BibData::instance().insertTag(widget->text());
					}
					else
					{
						widget->setText("");
					}
				}
			}
		}
		QStyledItemDelegate::setModelData(editor, model, index);
	}
};

void addListItem(QListWidget* listWidget, const QString& text)
{
	auto item = new QListWidgetItem(listWidget);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	item->setText(text);
	listWidget->addItem(item);
	listWidget->setItemDelegate(new TagsItemDelegate(listWidget));
}

} // namespace

NewDocumentDialog::NewDocumentDialog(const QVariant& documentId, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewDocumentDialog)
    , completer_(this)
    , documentId_(documentId)
{
	ui->setupUi(this);

	if (documentId_.isNull())
	{
		setWindowTitle("Nieuw document");
	}
	else
	{
		setWindowTitle("Bewerk document");
		auto rowId = BibData::instance().findRow(documentId.toInt());
		if (documentId_.isValid())
		{
			ui->nameTxt->setText(BibData::instance().get(rowId.toInt(), BibColumn::Name).toString());
			ui->boxNrSbx->setValue(BibData::instance().get(rowId.toInt(), BibColumn::BoxNr).toInt());
			ui->fileTxt->setText(BibData::instance().get(rowId.toInt(), BibColumn::File).toString());
			for (const QString& tag : BibData::instance().get(rowId.toInt(), BibColumn::Tags).toStringList())
			{
				addListItem(ui->listWidget, tag);
			}
		}
	}

	for (int i = ui->listWidget->count(); i < 8; ++i)
	{
		addListItem(ui->listWidget, "");
	}

	connect(ui->listWidget, &QListWidget::itemChanged, [this](QListWidgetItem*)
	{
		for (int i = 0; i < ui->listWidget->count(); ++i)
		{
			if (ui->listWidget->item(i)->text().isEmpty())
			{
				return;
			}
		}
		addListItem(ui->listWidget, "");
	});

	connect(ui->fileBrowseBtn, &QPushButton::clicked, [this](bool /*checked*/)
	{
		auto file = QFileDialog::getOpenFileName(this, "Bestand selecteren");
		if (!file.isEmpty())
		{
			ui->fileTxt->setText(file);
		}
	});
}

NewDocumentDialog::~NewDocumentDialog()
{
	delete ui;
}

void NewDocumentDialog::accept()
{
	QStringList tags;
	for (int i = 0; i < ui->listWidget->count(); ++i)
	{
		auto tag = ui->listWidget->item(i)->text();
		if (!tag.isEmpty())
		{
			tags += tag;
		}
	}
	if (documentId_.isNull())
	{	
		BibData::instance().insertDocument(ui->nameTxt->text(),
		                                   ui->boxNrSbx->value(),
		                                   ui->fileTxt->text(),
		                                   tags);
	}
	else
	{
		BibData::instance().updateDocument(documentId_.toInt(),
		                                   ui->nameTxt->text(),
		                                   ui->boxNrSbx->value(),
		                                   ui->fileTxt->text(),
		                                   tags);
	}

	QDialog::accept();
}
