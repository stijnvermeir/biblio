#include "bibdata.h"

#include <QtSql>
#include <QStandardPaths>
#include <QDir>
#include <QVector>
#include <QHash>
#include <QDebug>

using namespace std;

namespace {

QSqlDatabase conn()
{
	return QSqlDatabase::database("biblio");
}

QSqlError initDb()
{
	const int DB_VERSION = 1;
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "biblio");
	QDir localAppDataDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
	localAppDataDir.mkpath(".");
	db.setDatabaseName(localAppDataDir.absolutePath() + QDir::separator() + "biblio.db");
	if (!db.open()) return db.lastError();

	if (!db.tables().contains("version")) // database does not exist yet
	{
		qDebug() << "Creating biblio db ...";
		QSqlQuery q(db);

		// version table
		if (!q.exec("CREATE TABLE version(number integer primary key)")) return q.lastError();
		if (!q.prepare("INSERT INTO version(number) VALUES (?)")) return q.lastError();
		q.addBindValue(DB_VERSION);
		if (!q.exec()) return q.lastError();

		// document table
		if (!q.exec("CREATE TABLE document(id integer primary key, name text, boxnr integer, filepath text)")) return q.lastError();

		// tag table
		if (!q.exec("CREATE TABLE tag(id integer primary key, name text)")) return q.lastError();

		// document_tag table
		if (!q.exec("CREATE TABLE document_tag(id integer primary key, tag_id integer, document_id integer, "
		            "FOREIGN KEY(tag_id) REFERENCES tag(id) ON UPDATE CASCADE ON DELETE CASCADE, "
		            "FOREIGN KEY(document_id) REFERENCES document(id) ON UPDATE CASCADE ON DELETE CASCADE)")) return q.lastError();

		qDebug() << "biblio db created succesfully.";
	}
	else
	{
		QSqlQuery q(db);
		if (!q.exec("SELECT max(number) FROM version")) return q.lastError();
		if (!q.next()) return q.lastError();
		int currentVersion = q.value(0).toInt();
		if (currentVersion != DB_VERSION)
		{
			qDebug() << "biblio db version" << currentVersion;
			// for auto database model updates later
			qFatal("Invalid biblio db version");
		}
	}

	return QSqlError();
}

} // namespace

struct BibData::Pimpl
{
	struct Row
	{
		int id;
		QVariant name;
		QVariant boxNr;
		QVariant filePath;
		QVector<int> tags;
	};
	QVector<Row> data_;

	QHash<int, QString> tags_;
	QHash<QString, int> tagsReversed_;

	Pimpl()
	    : data_()
	{
		QSqlError err = initDb();
		if (err.isValid())
		{
			qDebug() << err;
		}
		err = load();
		if (err.isValid())
		{
			qDebug() << err;
		}
	}

	QSqlError load()
	{
		data_.clear();

		QSqlDatabase db = conn();
		QSqlQuery q(db);
		if (!q.exec("SELECT id, name, boxnr, filepath FROM document")) return q.lastError();
		while (q.next())
		{
			Row r;
			r.id = q.value(0).toInt();
			r.name = q.value(1).toString();
			r.boxNr = q.value(2).toInt();
			r.filePath = q.value(3).toString();
			data_.push_back(r);
		}
		if (!q.exec("SELECT id, name FROM tag")) return q.lastError();
		while (q.next())
		{
			auto id = q.value(0).toInt();
			auto name = q.value(1).toString();
			tags_[id] = name;
			tagsReversed_[name] = id;
		}
		for (Row& row : data_)
		{
			if (!q.prepare("SELECT tag_id FROM document_tag WHERE document_id = ?")) return q.lastError();
			q.addBindValue(row.id);
			if (!q.exec()) return q.lastError();
			while (q.next())
			{
				row.tags.push_back(q.value(0).toInt());
			}
		}
		return QSqlError();
	}

	int getNumRows() const
	{
		return data_.size();
	}

	QVariant get(const int row, const BibColumn column) const
	{
		if (row >= 0 && row < getNumRows())
		{
			const Row& entry = data_[row];
			if (column == BibColumn::Id)
			{
				return entry.id;
			}
			if (column == BibColumn::Name)
			{
				return entry.name;
			}
			if (column == BibColumn::BoxNr)
			{
				return entry.boxNr;
			}
			if (column == BibColumn::File)
			{
				return entry.filePath;
			}
			if (column == BibColumn::Tags)
			{
				QStringList list;
				for (const int tagId : entry.tags)
				{
					list += tags_[tagId];
				}
				return list;
			}
		}
		return QVariant();
	}

	QStringList getTags() const
	{
		return tags_.values();
	}

	bool doesTagExist(const QString& tag) const
	{
		return tagsReversed_.contains(tag);
	}

	QVariant findRow(const int documentId) const
	{
		for (int row = 0; row < data_.size(); ++row)
		{
			if (data_[row].id == documentId)
			{
				return row;
			}
		}
		return QVariant();
	}

	void insertDocument(const QString& name, const int boxNr, const QString& file, const QStringList& tags)
	{
		Row r;
		r.name = name;
		r.boxNr = boxNr;
		r.filePath = file;
		for (const auto& tag : tags)
		{
			insertTag(tag);
			r.tags.push_back(tagsReversed_[tag]);
		}
		QSqlDatabase db = conn();
		QSqlQuery q(db);
		q.prepare("INSERT INTO document(name, boxnr, filepath) VALUES(?, ?, ?)");
		q.addBindValue(r.name);
		q.addBindValue(r.boxNr);
		q.addBindValue(r.filePath);
		q.exec();
		QVariant dbId = q.lastInsertId();
		if (dbId.isValid() && !q.lastError().isValid())
		{
			r.id = dbId.toInt();
			for (const auto& t : r.tags)
			{
				insertDocumentTag(r.id, t);
			}
			data_.push_back(r);
		}
	}

	void insertTag(const QString& tag)
	{
		if (!tagsReversed_.contains(tag))
		{
			QSqlDatabase db = conn();
			QSqlQuery q(db);
			if (!q.prepare("INSERT INTO tag(name) VALUES(?)")) return;
			q.addBindValue(tag);
			if (!q.exec()) return;
			int tagId = q.lastInsertId().toInt();
			tags_[tagId] = tag;
			tagsReversed_[tag] = tagId;
		}
	}

	void insertDocumentTag(const int documentId, const int tagId)
	{
		QSqlDatabase db = conn();
		QSqlQuery q(db);
		q.prepare("INSERT INTO document_tag(document_id, tag_id) VALUES(?, ?)");
		q.addBindValue(documentId);
		q.addBindValue(tagId);
		q.exec();
	}

	void updateDocument(const int documentId, const QString& name, const int boxNr, const QString& file, const QStringList& tags)
	{
		auto row = findRow(documentId);
		if (row.isNull())
		{
			return;
		}
		Row& r = data_[row.toInt()];
		r.name = name;
		r.boxNr = boxNr;
		r.filePath = file;
		r.tags.clear();
		for (const auto& tag : tags)
		{
			insertTag(tag);
			r.tags.push_back(tagsReversed_[tag]);
		}
		QSqlDatabase db = conn();
		QSqlQuery q(db);
		q.prepare("UPDATE document SET name = ?, boxnr = ?, filepath = ? WHERE id = ?");
		q.addBindValue(r.name);
		q.addBindValue(r.boxNr);
		q.addBindValue(r.filePath);
		q.addBindValue(documentId);
		q.exec();
		// delete old tags
		q.prepare("DELETE FROM document_tag WHERE document_id = ?");
		q.addBindValue(documentId);
		q.exec();
		// add new tags
		for (const auto& t : r.tags)
		{
			insertDocumentTag(documentId, t);
		}
	}

	void deleteDocument(const int documentId)
	{
		auto row = findRow(documentId);
		if (row.isNull())
		{
			return;
		}
		QSqlDatabase db = conn();
		QSqlQuery q(db);
		q.prepare("DELETE FROM document_tag WHERE document_id = ?");
		q.addBindValue(documentId);
		if (!q.exec()) return;
		q.prepare("DELETE FROM document WHERE id = ?");
		q.addBindValue(documentId);
		if (!q.exec()) return;
		data_.remove(row.toInt());
	}
};

BibData& BibData::instance()
{
	static BibData inst;
	return inst;
}

BibData::BibData()
    : pimpl_(new Pimpl())
{
}

BibData::~BibData()
{
}

int BibData::getNumRows() const
{
	return pimpl_->getNumRows();
}

QVariant BibData::get(const int row, const BibColumn column) const
{
	return pimpl_->get(row, column);
}

QStringList BibData::getTags() const
{
	return pimpl_->getTags();
}

bool BibData::doesTagExist(const QString& tag) const
{
	return pimpl_->doesTagExist(tag);
}

void BibData::insertTag(const QString &tag)
{
	pimpl_->insertTag(tag);
}

QVariant BibData::findRow(const int documentId) const
{
	return pimpl_->findRow(documentId);
}

void BibData::insertDocument(const QString& name, const int boxNr, const QString& file, const QStringList& tags)
{
	pimpl_->insertDocument(name, boxNr, file, tags);
}

void BibData::updateDocument(const int documentId, const QString& name, const int boxNr, const QString& file, const QStringList& tags)
{
	pimpl_->updateDocument(documentId, name, boxNr, file, tags);
}

void BibData::deleteDocument(const int documentId)
{
	pimpl_->deleteDocument(documentId);
}
