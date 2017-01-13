#pragma once

#include <QSortFilterProxyModel>
#include <QScopedPointer>

class BibTableModel : public QSortFilterProxyModel
{
public:
	BibTableModel();
	~BibTableModel();

private:
	struct Pimpl;
	QScopedPointer<Pimpl> pimpl_;
};
