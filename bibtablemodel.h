#pragma once

#include <QSortFilterProxyModel>
#include <QScopedPointer>

class BibTableModel : public QSortFilterProxyModel
{
public:
	BibTableModel();
	~BibTableModel();

	void setSearchTerm(const QString& term);

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

private:
	struct Pimpl;
	QScopedPointer<Pimpl> pimpl_;
	QString searchTerm_;
};
