#pragma once

#include <QVariant>
#include <QStringList>
#include <QScopedPointer>

enum class BibColumn
{
	Id,
	Name,
	BoxNr,
	File,
	Tags
};

class BibData
{
public:
	static BibData& instance();

	int getNumRows() const;
	QVariant get(const int row, const BibColumn column) const;

	QStringList getTags() const;
	bool doesTagExist(const QString& tag) const;
	void insertTag(const QString& tag);

	QVariant findRow(const int documentId) const;
	void insertDocument(const QString& name, const int boxNr, const QString& file, const QStringList& tags);
	void updateDocument(const int documentId, const QString& name, const int boxNr, const QString& file, const QStringList& tags);
	void deleteDocument(const int documentId);

private:
	BibData();
	~BibData();

	struct Pimpl;
	QScopedPointer<Pimpl> pimpl_;
};
