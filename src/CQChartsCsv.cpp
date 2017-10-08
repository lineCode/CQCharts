#include <CQChartsCsv.h>
#include <CQCharts.h>
#include <CQCsvModel.h>

CQChartsCsv::
CQChartsCsv(CQCharts *charts) :
 QSortFilterProxyModel(), CQChartsModelColumn(charts), charts_(charts)
{
  csvModel_ = new CQCsvModel;

  setSourceModel(csvModel_);
}

CQChartsCsv::
~CQChartsCsv()
{
  delete csvModel_;
}

void
CQChartsCsv::
setCommentHeader(bool b)
{
  csvModel_->setCommentHeader(b);
}

void
CQChartsCsv::
setFirstLineHeader(bool b)
{
  csvModel_->setFirstLineHeader(b);
}

bool
CQChartsCsv::
load(const QString &filename)
{
  return csvModel_->load(filename);
}

int
CQChartsCsv::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsCsv::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

bool
CQChartsCsv::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (role == CQCharts::Role::ColumnType) {
    setColumnType(section, value.toString());
    return true;
  }

  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsCsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return QVariant(columnType(section));

  return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant
CQChartsCsv::
data(const QModelIndex &index, int role) const
{
  QVariant var = QSortFilterProxyModel::data(index, role);

  if (role == Qt::UserRole && ! var.isValid())
    var = QSortFilterProxyModel::data(index, Qt::DisplayRole);

  if (role == Qt::DisplayRole || role == Qt::UserRole) {
    if (! index.isValid())
      return QVariant();

    assert(index.model() == this);

    QModelIndex index1 = QSortFilterProxyModel::mapToSource(index);

    assert(index.column() == index1.column());

    if (role == Qt::DisplayRole)
      return columnDisplayData(index1.column(), var);
    else
      return columnUserData(index1.column(), var);
  }

  return var;
}

QModelIndex
CQChartsCsv::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsCsv::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
