#include "bibtablemodel.h"
#include "bibdata.h"

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

namespace {

struct ColumnInfo
{
	BibColumn column;
	QString displayName;
};

const QVector<ColumnInfo> COLUMNS =
{
    { BibColumn::Name, "Naam" },
    { BibColumn::BoxNr, "Schuif" },
    { BibColumn::File, "Bestand" },
    { BibColumn::Tags, "Kernwoorden" }
};

} // namespace

struct BibTableModel::Pimpl : public virtual QAbstractTableModel
{
	Pimpl()
	{
	}

	virtual int rowCount(const QModelIndex& = QModelIndex()) const
	{
		return BibData::instance().getNumRows();
	}

	virtual int columnCount(const QModelIndex& = QModelIndex()) const
	{
		return COLUMNS.count();
	}

	virtual QVariant data(const QModelIndex& index, int role) const
	{
		if (index.isValid())
		{
			if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
			{
				if (index.row() < rowCount() && index.column() < columnCount())
				{
					auto col = COLUMNS[index.column()].column;
					auto data = BibData::instance().get(index.row(), col);
					if (col == BibColumn::Tags)
					{
						auto tags = data.toStringList();
						tags.sort(Qt::CaseInsensitive);
						return tags.join(" / ");
					}
					return data;
				}
			}
			if (role == Qt::UserRole)
			{
				return BibData::instance().get(index.row(), BibColumn::Id);
			}
		}
		return QVariant();
	}

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (orientation == Qt::Horizontal)
		{
			if (role == Qt::DisplayRole)
			{
				if (section >= 0 && section < columnCount())
				{
					return COLUMNS[section].displayName;
				}
			}
		}
		return QVariant();
	}

	virtual Qt::ItemFlags flags(const QModelIndex& index) const
	{
		return QAbstractTableModel::flags(index);
	}
};

BibTableModel::BibTableModel()
    : pimpl_(new Pimpl())
    , searchTerm_()
{
	setSourceModel(pimpl_.data());
}

BibTableModel::~BibTableModel()
{
}

void BibTableModel::setSearchTerm(const QString& term)
{
	searchTerm_ = term;
	invalidateFilter();
}

bool BibTableModel::filterAcceptsRow(int source_row, const QModelIndex& /*source_parent*/) const
{
	return BibData::instance().get(source_row, BibColumn::Tags).toStringList().join(" ").contains(searchTerm_, Qt::CaseInsensitive);
}
