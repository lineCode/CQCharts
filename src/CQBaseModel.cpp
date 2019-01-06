#include <CQBaseModel.h>
#include <CQStrUtil.h>
#include <CMathUtil.h>
#include <cmath>
#include <cassert>

namespace {

using TypeName = std::map<CQBaseModelType,QString>;
using NameType = std::map<QString,CQBaseModelType>;

static TypeName s_typeName;
static NameType s_nameType;

void addType(CQBaseModelType type, const QString &name) {
  s_typeName[type] = name;
  s_nameType[name] = type;
}

void initTypes() {
  if (s_typeName.empty()) {
    addType(CQBaseModelType::BOOLEAN        , "boolean"        );
    addType(CQBaseModelType::INTEGER        , "integer"        );
    addType(CQBaseModelType::REAL           , "real"           );
    addType(CQBaseModelType::STRING         , "string"         );
    addType(CQBaseModelType::STRINGS        , "strings"        );
    addType(CQBaseModelType::POINT          , "point"          );
    addType(CQBaseModelType::LINE           , "line"           );
    addType(CQBaseModelType::RECT           , "rect"           );
    addType(CQBaseModelType::SIZE           , "size"           );
    addType(CQBaseModelType::POLYGON        , "polygon"        );
    addType(CQBaseModelType::POLYGON_LIST   , "polygon_list"   );
    addType(CQBaseModelType::COLOR          , "color"          );
    addType(CQBaseModelType::PEN            , "pen"            );
    addType(CQBaseModelType::BRUSH          , "brush"          );
    addType(CQBaseModelType::IMAGE          , "image"          );
    addType(CQBaseModelType::TIME           , "time"           );
    addType(CQBaseModelType::SYMBOL         , "symbol"         );
    addType(CQBaseModelType::SYMBOL_SIZE    , "symbol_size"    );
    addType(CQBaseModelType::FONT_SIZE      , "font_size"      );
    addType(CQBaseModelType::PATH           , "path"           );
    addType(CQBaseModelType::STYLE          , "style"          );
    addType(CQBaseModelType::CONNECTION_LIST, "connection_list");
    addType(CQBaseModelType::NAME_PAIR      , "name_pair"      );
    addType(CQBaseModelType::COLUMN         , "column"         );
    addType(CQBaseModelType::COLUMN_LIST    , "column_list"    );
    addType(CQBaseModelType::ENUM           , "enum"           );
  }
}

};

//---

CQBaseModel::
CQBaseModel()
{
  setObjectName("baseModel");

  initTypes();
}

void
CQBaseModel::
genColumnTypes()
{
  resetColumnTypes();

  // auto determine type for each column. Do column by column to allow early out
  int nc = columnCount();

  for (int column = 0; column < nc; ++column)
    genColumnType(column);
}

void
CQBaseModel::
genColumnType(int column)
{
  ColumnData &columnData = getColumnData(column);

  genColumnType(columnData);

  emit columnTypeChanged(columnData.column);
}

void
CQBaseModel::
genColumnType(const ColumnData &columnData) const
{
  std::unique_lock<std::mutex> lock(mutex_);

  if (columnData.type == CQBaseModelType::NONE) {
    CQBaseModel *th = const_cast<CQBaseModel *>(this);

    th->genColumnType(const_cast<ColumnData &>(columnData));
  }
}

void
CQBaseModel::
genColumnType(ColumnData &columnData)
{
  ColumnTypeData columnTypeData;

  QModelIndex parent;

  (void) genColumnType(parent, columnData.column, columnTypeData);

  // if inderminate (no values or all reals or integers) then use real if any reals,
  // integer if any integers and string if no values.
  if (columnTypeData.type == CQBaseModelType::NONE) {
    if      (columnTypeData.numReal)
      columnTypeData.type = CQBaseModelType::REAL;
    else if (columnTypeData.numInt)
      columnTypeData.type = CQBaseModelType::INTEGER;
    else
      columnTypeData.type = CQBaseModelType::STRING;
  }

  columnTypeData.baseType = columnTypeData.type;

  if (columnTypeData.type != columnData.type) {
    columnData.type     = columnTypeData.type;
    columnData.baseType = columnTypeData.baseType;
  }
}

bool
CQBaseModel::
genColumnType(const QModelIndex &parent, int c, ColumnTypeData &columnTypeData)
{
  bool done = false;

  int nr = rowCount(parent);

  for (int r = 0; r < nr && r < maxTypeRows(); ++r) {
    QModelIndex ind0 = index(r, 0, parent);

    if (rowCount(ind0) > 0) {
      if (genColumnType(ind0, c, columnTypeData)) {
        done = true;
        break;
      }
    }

    //---

    // get value
    QModelIndex ind = (c != 0 ? index(r, c, parent) : ind0);

    QVariant v = data(ind, Qt::DisplayRole);

    if      (v.type() == QVariant::Double) {
      ++columnTypeData.numReal;
    }
    else if (v.type() == QVariant::Int) {
      ++columnTypeData.numInt;
    }
    else {
      QString s = v.toString();

      if (! s.length())
        continue;

      //---

      // check if string can be converted to real
      bool ok;

      double real = toReal(s, ok);

      // if not real then assume string column and we are done
      if (! ok) {
        columnTypeData.type = CQBaseModelType::STRING;
        done = true;
        break;
      }

      // if real can also be an integer prefer integer
      if (isInteger(real))
        ++columnTypeData.numInt;
      else
        ++columnTypeData.numReal;
    }
  }

  return done;
}

CQBaseModelType
CQBaseModel::
columnType(int column) const
{
  if (column < 0 || column >= columnCount())
    return CQBaseModelType::NONE;

  const ColumnData &columnData = getColumnData(column);

  if (columnData.type == CQBaseModelType::NONE)
    genColumnType(columnData);

  return columnData.type;
}

bool
CQBaseModel::
setColumnType(int column, CQBaseModelType type)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  if (columnData.baseType == CQBaseModelType::NONE)
    genColumnType(columnData);

  if (type != columnData.type) {
    columnData.type = type;

    emit columnTypeChanged(column);
  }

  return true;
}

CQBaseModelType
CQBaseModel::
columnBaseType(int column) const
{
  if (column < 0 || column >= columnCount())
    return CQBaseModelType::NONE;

  const ColumnData &columnData = getColumnData(column);

  if (columnData.type == CQBaseModelType::NONE)
    genColumnType(columnData);

  return columnData.baseType;
}

bool
CQBaseModel::
setColumnBaseType(int column, CQBaseModelType type)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  if (type != columnData.baseType) {
    columnData.baseType = type;

    emit columnBaseTypeChanged(column);
  }

  return true;
}

QString
CQBaseModel::
columnTypeValues(int column) const
{
  if (column < 0 || column >= columnCount())
    return QString();

  const ColumnData &columnData = getColumnData(column);

  return columnData.typeValues;
}

bool
CQBaseModel::
setColumnTypeValues(int column, const QString &str)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.typeValues = str;

  emit columnTypeChanged(column);

  return true;
}

QVariant
CQBaseModel::
columnMin(int column) const
{
  if (column < 0 || column >= columnCount())
    return QVariant();

  const ColumnData &columnData = getColumnData(column);

  return columnData.min;
}

bool
CQBaseModel::
setColumnMin(int column, const QVariant &v)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.min = v;

  emit columnRangeChanged(column);

  return true;
}

QVariant
CQBaseModel::
columnMax(int column) const
{
  if (column < 0 || column >= columnCount())
    return QVariant();

  const ColumnData &columnData = getColumnData(column);

  return columnData.max;
}

bool
CQBaseModel::
setColumnMax(int column, const QVariant &v)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.max = v;

  emit columnRangeChanged(column);

  return true;
}

bool
CQBaseModel::
isColumnKey(int column) const
{
  if (column < 0 || column >= columnCount())
    return false;

  const ColumnData &columnData = getColumnData(column);

  return columnData.key;
}

bool
CQBaseModel::
setColumnKey(int column, bool b)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.key = b;

  emit columnKeyChanged(column);

  return true;
}

bool
CQBaseModel::
isColumnSorted(int column) const
{
  if (column < 0 || column >= columnCount())
    return false;

  const ColumnData &columnData = getColumnData(column);

  return columnData.sorted;
}

bool
CQBaseModel::
setColumnSorted(int column, bool b)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.sorted = b;

  emit columnSortedChanged(column);

  return true;
}

int
CQBaseModel::
columnSortOrder(int column) const
{
  if (column < 0 || column >= columnCount())
    return Qt::AscendingOrder;

  const ColumnData &columnData = getColumnData(column);

  return columnData.sortOrder;
}

bool
CQBaseModel::
setColumnSortOrder(int column, int i)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.sortOrder = i;

  emit columnSortOrderChanged(column);

  return true;
}

const CQBaseModel::ColumnData &
CQBaseModel::
getColumnData(int column) const
{
  return const_cast<CQBaseModel *>(this)->getColumnData(column);
}

CQBaseModel::ColumnData &
CQBaseModel::
getColumnData(int column)
{
  assert(column >= 0 || column < columnCount());

  auto p = columnDatas_.find(column);

  if (p != columnDatas_.end()) {
    ColumnData &columnData = (*p).second;

    assert(columnData.column == column);

    return columnData;
  }

  //---

  std::unique_lock<std::mutex> lock(mutex_);

  CQBaseModel *th = const_cast<CQBaseModel *>(this);

  auto p1 = th->columnDatas_.find(column);

  if (p1 == th->columnDatas_.end())
    p1 = th->columnDatas_.insert(p1, ColumnDatas::value_type(column, ColumnData(column)));

  return (*p1).second;
}

void
CQBaseModel::
resetColumnType(int column)
{
  auto p = columnDatas_.find(column);

  if (p != columnDatas_.end())
    columnDatas_.erase(p);
}

void
CQBaseModel::
resetColumnTypes()
{
  for (auto &p : columnDatas_) {
    ColumnData &columnData = p.second;

    columnData.type = CQBaseModelType::NONE;
  }
}

//------

QVariant
CQBaseModel::
rowGroup(int row) const
{
  if (row < 0 || row >= rowCount())
    return QVariant();

  const RowData &rowData = getRowData(row);

  return rowData.group;
}

bool
CQBaseModel::
setRowGroup(int row, const QVariant &v)
{
  if (row < 0 || row >= rowCount())
    return false;

  RowData &rowData = getRowData(row);

  rowData.group = v;

  return true;
}

const CQBaseModel::RowData &
CQBaseModel::
getRowData(int row) const
{
  return const_cast<CQBaseModel *>(this)->getRowData(row);
}

CQBaseModel::RowData &
CQBaseModel::
getRowData(int row)
{
  assert(row >= 0 || row < rowCount());

  auto p = rowDatas_.find(row);

  if (p != rowDatas_.end()) {
    RowData &rowData = (*p).second;

    assert(rowData.row == row);

    return rowData;
  }

  //---

  std::unique_lock<std::mutex> lock(mutex_);

  CQBaseModel *th = const_cast<CQBaseModel *>(this);

  auto p1 = th->rowDatas_.find(row);

  if (p1 == th->rowDatas_.end())
    p1 = th->rowDatas_.insert(p1, RowDatas::value_type(row, RowData(row)));

  return (*p1).second;
}

//------

QVariant
CQBaseModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  // generic column data
  if      (orientation == Qt::Horizontal) {
    if      (role == static_cast<int>(CQBaseModelRole::Type)) {
      return QVariant((int) columnType(section));
    }
    else if (role == static_cast<int>(CQBaseModelRole::BaseType)) {
      return QVariant((int) columnBaseType(section));
    }
    else if (role == static_cast<int>(CQBaseModelRole::TypeValues)) {
      return QVariant(columnTypeValues(section));
    }
    else if (role == static_cast<int>(CQBaseModelRole::Min)) {
      return columnMin(section);
    }
    else if (role == static_cast<int>(CQBaseModelRole::Max)) {
      return columnMax(section);
    }
    else if (role == static_cast<int>(CQBaseModelRole::Key)) {
      return isColumnKey(section);
    }
    else if (role == static_cast<int>(CQBaseModelRole::Sorted)) {
      return isColumnSorted(section);
    }
    else if (role == static_cast<int>(CQBaseModelRole::SortOrder)) {
      return columnSortOrder(section);
    }
    else {
      return QAbstractItemModel::headerData(section, orientation, role);
    }
  }
  // generic row data
  else if (orientation == Qt::Vertical) {
    if (role == static_cast<int>(CQBaseModelRole::Group)) {
      return rowGroup(section);
    }
    else {
      return QAbstractItemModel::headerData(section, orientation, role);
    }
  }
  else {
    assert(false);
  }

  return QVariant();
}

bool
CQBaseModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  // generic column data
  if      (orientation == Qt::Horizontal) {
    if      (role == static_cast<int>(CQBaseModelRole::Type)) {
      bool ok;

      CQBaseModelType type = variantToType(value, &ok);
      if (! ok) return false;

      return setColumnType(section, type);
    }
    else if (role == static_cast<int>(CQBaseModelRole::BaseType)) {
      bool ok;

      CQBaseModelType type = variantToType(value, &ok);
      if (! ok) return false;

      return setColumnBaseType(section, type);
    }
    else if (role == static_cast<int>(CQBaseModelRole::TypeValues)) {
      QString str = value.toString();

      return setColumnTypeValues(section, str);
    }
    else if (role == static_cast<int>(CQBaseModelRole::Min)) {
      return setColumnMin(section, value);
    }
    else if (role == static_cast<int>(CQBaseModelRole::Max)) {
      return setColumnMax(section, value);
    }
    else if (role == static_cast<int>(CQBaseModelRole::Key)) {
      return setColumnKey(section, value.toBool());
    }
    else if (role == static_cast<int>(CQBaseModelRole::Sorted)) {
      return setColumnSorted(section, value.toBool());
    }
    else if (role == static_cast<int>(CQBaseModelRole::SortOrder)) {
      return setColumnSortOrder(section, value.toInt());
    }
    else {
      return QAbstractItemModel::setHeaderData(section, orientation, role);
    }
  }
  // generic row data
  else if (orientation == Qt::Vertical) {
    if (role == static_cast<int>(CQBaseModelRole::Group)) {
      return setRowGroup(section, value);
    }
    else {
      return QAbstractItemModel::setHeaderData(section, orientation, role);
    }
  }
  else {
    assert(false);
  }

  return false;
}

QVariant
CQBaseModel::
data(const QModelIndex &index, int role) const
{
  if (role == Qt::TextAlignmentRole) {
    CQBaseModelType type = columnType(index.column());

    if (type == CQBaseModelType::INTEGER || type == CQBaseModelType::REAL)
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }

  //return QAbstractItemModel::data(index, role);
  return QVariant();
}

//------

int
CQBaseModel::
modelColumnNameToInd(const QString &name) const
{
  return modelColumnNameToInd(this, name);
}

int
CQBaseModel::
modelColumnNameToInd(const QAbstractItemModel *model, const QString &name)
{
  int role = Qt::DisplayRole;

  for (int icolumn = 0; icolumn < model->columnCount(); ++icolumn) {
    QVariant var = model->headerData(icolumn, Qt::Horizontal, role);

    if (! var.isValid())
      continue;

    QString name1 = var.toString();

    if (name == name1)
      return icolumn;
  }

  //---

  bool ok;

  int column = name.toInt(&ok);

  if (ok)
    return column;

  return -1;
}

//------

CQBaseModelType
CQBaseModel::
variantToType(const QVariant &var, bool *ok)
{
  CQBaseModelType type = CQBaseModelType::NONE;

  if (var.type() == QVariant::Int) {
    type = (CQBaseModelType) var.toInt(ok);
  }
  else {
    QString str = var.toString();

    type = nameType(str);
  }

  if (! isType((int) type)) {
    if (ok)
      *ok = false;
  }

  return type;
}

QVariant
CQBaseModel::
typeToVariant(CQBaseModelType type)
{
  return QVariant((int) type);
}

bool
CQBaseModel::
isSameType(const QVariant &var, CQBaseModelType type)
{
  if (type == CQBaseModelType::REAL && var.type() == QVariant::Double)
    return true;

  if (type == CQBaseModelType::INTEGER && var.type() == QVariant::Int)
    return true;

#if 0
  if (type == CQBaseModelType::TIME && var.type() == QVariant::Double)
    return true;
#endif

  return false;
}

QVariant
CQBaseModel::
typeStringToVariant(const QString &str, CQBaseModelType type)
{
  if      (type == CQBaseModelType::REAL) {
    bool ok;

    double real = toReal(str, ok);

    if (ok)
      return QVariant(real);
  }
  else if (type == CQBaseModelType::INTEGER) {
    bool ok;

    long integer = toInt(str, ok);

    if (ok)
      return QVariant(int(integer));
  }
#if 0
  else if (type == CQBaseModelType::TIME) {
    bool ok;

    double real = toReal(str, ok);

    if (ok)
      return QVariant(real);
  }
#endif

  return QVariant(str);
}

bool
CQBaseModel::
isType(int type)
{
  return (s_typeName.find((CQBaseModelType) type) != s_typeName.end());
}

QString
CQBaseModel::
typeName(CQBaseModelType type)
{
  auto p = s_typeName.find(type);

  if (p == s_typeName.end())
    return "none";

  return (*p).second;
}

CQBaseModelType
CQBaseModel::
nameType(const QString &name)
{
  auto p = s_nameType.find(name);

  if (p == s_nameType.end())
    return CQBaseModelType::NONE;

  return (*p).second;
}

double
CQBaseModel::
toReal(const QString &str, bool &ok)
{
  return CQStrUtil::toReal(str, ok);
}

long
CQBaseModel::
toInt(const QString &str, bool &ok)
{
  return CQStrUtil::toInt(str, ok);
}

bool
CQBaseModel::
isInteger(double r)
{
  return CMathUtil::isInteger(r);
}
