#include <CQChartsGroupPlot.h>
#include <CQChartsVariant.h>
#include <CQPerfMonitor.h>

CQChartsGroupPlotType::
CQChartsGroupPlotType()
{
}

void
CQChartsGroupPlotType::
addParameters()
{
  startParameterGroup("Grouping");

  if (isGroupRequired())
    addColumnParameter("group", "Group", "groupColumn").
     setRequired().setGroupable().setTip("Group column");
  else
    addColumnParameter("group", "Group", "groupColumn").
     setGroupable().setTip("Group column");

  if (allowRowGrouping())
    addBoolParameter("rowGrouping", "Row Grouping", "rowGrouping").
      setTip("Group by rows instead of column headers");

  if (allowUsePath())
    addBoolParameter("usePath", "Use Path", "usePath").
      setTip("Use hierarchical path as group");

  if (allowUseRow())
    addBoolParameter("useRow", "Use Row", "useRow").
      setTip("Use row number for group");

  addBoolParameter("exactValue", "Exact Value", "exactValue", true).
   setTip("use exact value for grouping");

  addBoolParameter("autoRange", "Auto Range", "autoRange", true).
   setTip("automatically determine value range");

  addRealParameter("start", "Start", "startValue", 0.0).
    setRequired().setTip("Start value for manual range");
  addRealParameter("delta", "Delta", "deltaValue", 1.0).
    setRequired().setTip("Delta value for manual range");

  addIntParameter("numAuto", "Num Auto", "numAuto", 10).
    setRequired().setTip("Number of auto groups");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

//---

CQChartsGroupPlot::
CQChartsGroupPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsPlot(view, plotType, model)
{
}

CQChartsGroupPlot::
~CQChartsGroupPlot()
{
}

void
CQChartsGroupPlot::
setGroupColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGroupPlot::
addProperties()
{
  CQChartsGroupPlotType *type = dynamic_cast<CQChartsGroupPlotType *>(this->type());
  assert(type);

  addProperty("dataGrouping", this, "groupColumn", "group");

  if (type->allowRowGrouping())
    addProperty("dataGrouping", this, "rowGrouping", "rowGroups");

  if (type->allowUsePath())
    addProperty("dataGrouping", this, "usePath", "path");

  if (type->allowUseRow())
    addProperty("dataGrouping", this, "useRow", "row");

  addProperty("dataGrouping/bucket", this, "exactValue", "exact"  );
  addProperty("dataGrouping/bucket", this, "autoRange" , "auto"   );
  addProperty("dataGrouping/bucket", this, "startValue", "start"  );
  addProperty("dataGrouping/bucket", this, "deltaValue", "delta"  );
  addProperty("dataGrouping/bucket", this, "numAuto"   , "numAuto");
}

void
CQChartsGroupPlot::
initGroupData(const CQChartsColumns &dataColumns, const CQChartsColumn &nameColumn, bool hier)
{
  CQPerfTrace trace("CQChartsGroupPlot::initGroupData");

  // if multiple data columns then use name column and data labels
  //   if row grouping we are creating a value set per row (1 value per data column)
  //   if column grouping we are creating a value set per data column (1 value per row)
  // if group column defined use that
  // otherwise (single data column) just use name column (if any)
  CQChartsGroupData groupData;

  groupData.exactValue = isExactValue();
  groupData.bucketer   = groupData_.bucketer;
  groupData.usePath    = false;
  groupData.hier       = hier;

  // use multiple group columns
  if      (dataColumns.count() > 1) {
    groupData.columns     = dataColumns;
    groupData.rowGrouping = isRowGrouping(); // only used for multiple columns

    if      (groupColumn().isValid())
      groupData.column = groupColumn();
    else if (nameColumn.isValid())
      groupData.column = nameColumn;
  }
  // use single group column
  else if (groupColumn().isValid()) {
    groupData.column = groupColumn();
  }
  // use path and hierarchical
  else if (isUsePath() && isHierarchical()) {
    groupData.usePath = true;
  }
  // use row
  else if (isUseRow()) {
    groupData.useRow = true;
  }
#if 0
  // default use name column if defined (?? enables grouping when no groupiing wanted)
  else if (nameColumn.isValid()) {
    groupData.column = nameColumn;
  }
#endif

  initGroup(groupData);
}

//---

// init group buckets depending on:
//  group column
//  multiple value columns
//  row grouping
void
CQChartsGroupPlot::
initGroup(const CQChartsGroupData &data)
{
  groupBucket_.reset();

  QAbstractItemModel *model = this->model().data();
  if (! model) return;

  //---

  // when not row grouping we use the column header as the grouping id so all row
  // values in the column are added to the group
  if (data.columns.count() > 1 && ! data.rowGrouping) {
    groupBucket_.setDataType   (CQChartsColumnBucket::DataType::HEADER);
    groupBucket_.setColumnType (ColumnType::INTEGER);
    groupBucket_.setRowGrouping(false);

    for (const auto &column : data.columns) {
      bool ok;

      QString name = modelHeaderString(column, ok);

      if (! name.length())
        name = QString("%1").arg(column.column());

      int ind = groupBucket_.addValue(column.column());

      groupBucket_.setIndName(ind, name);
    }

    return;
  }

  //---

  // for specified grouping columns, set column and column type
  if      (data.columns.count() > 1) {
    CQChartsColumn column;

    if (data.column.isValid())
      column = data.column;

    if (! data.column.isValid()) {
      // TODO: combine multiple column values
      column = data.columns.column();
      assert(column.isValid());
    }

    ColumnType columnType = CQBaseModel::Type::STRING;

    if (column.type() == CQChartsColumn::Type::DATA ||
        column.type() == CQChartsColumn::Type::DATA_INDEX)
      columnType = columnValueType(column);

    if (isHierarchical())
      groupBucket_.setDataType(CQChartsColumnBucket::DataType::COLUMN_ROOT);
    else
      groupBucket_.setDataType(CQChartsColumnBucket::DataType::COLUMN);

    groupBucket_.setColumnType(columnType);
    groupBucket_.setColumn    (column);
  }
  // for specified grouping column, set column and column type
  else if (data.column.isValid()) {
    ColumnType columnType = CQBaseModel::Type::STRING;

    if (data.column.type() == CQChartsColumn::Type::DATA ||
        data.column.type() == CQChartsColumn::Type::DATA_INDEX)
      columnType = columnValueType(data.column);

    if (isHierarchical())
      groupBucket_.setDataType(CQChartsColumnBucket::DataType::COLUMN_ROOT);
    else
      groupBucket_.setDataType(CQChartsColumnBucket::DataType::COLUMN);

    groupBucket_.setColumnType(columnType);
    groupBucket_.setColumn    (data.column);
  }
  // no group column then use parent path (hierarchical)
  else if (data.usePath) {
    assert(isHierarchical());

    groupBucket_.setDataType  (CQChartsColumnBucket::DataType::PATH);
    groupBucket_.setColumnType(ColumnType::STRING);
  }
  else {
    groupBucket_.setDataType  (CQChartsColumnBucket::DataType::NONE);
    groupBucket_.setColumnType(data.useRow ? ColumnType::INTEGER : ColumnType::STRING);
  }

  groupBucket_.setUseRow    (data.useRow);
  groupBucket_.setExactValue(data.exactValue);
  groupBucket_.setBucketer  (data.bucketer);

  //---

  // process model data
  class GroupVisitor : public ModelVisitor {
   public:
    GroupVisitor(CQChartsGroupPlot *plot, CQChartsColumnBucket *bucket, bool hier) :
     plot_(plot), bucket_(bucket), hier_(hier) {
    }

    State visit(QAbstractItemModel *model, const VisitData &data) override {
      // add column value
      if      (bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN ||
               bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT) {
        bool ok;

        QVariant value;

        if (bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT)
          value = plot_->modelRootValue(data.row, bucket_->column(), data.parent, ok);
        else
          value = plot_->modelHierValue(data.row, bucket_->column(), data.parent, ok);

        if (value.isValid())
          bucket_->addValue(value);

        if (hier_) {
          QStringList paths = plot_->pathStrs(value.toString());

          for (int i = 0; i < paths.length(); ++i)
            bucket_->addValue(paths[i]);
        }
      }
      // add parent path (hierarchical)
      else if (bucket_->dataType() == CQChartsColumnBucket::DataType::PATH) {
        QString path = CQChartsUtil::parentPath(model, data.parent);

        bucket_->addString(path);
      }
      // use row number
      else if (bucket_->isUseRow()) {
        bucket_->addValue(data.row); // default to row
      }
      else {
        bucket_->addString(""); // no bucket
      }

      return State::OK;
    }

   private:
    CQChartsGroupPlot*    plot_   { nullptr };
    CQChartsColumnBucket* bucket_ { nullptr };
    bool                  hier_   { false };
  };

  GroupVisitor groupVisitor(this, &groupBucket_, data.hier);

  visitModel(groupVisitor);
}

//---

std::vector<int>
CQChartsGroupPlot::
rowHierGroupInds(const CQChartsModelIndex &ind) const
{
  std::vector<int> inds;

  if (! rowGroupInds(ind, inds, /*hier*/true))
    return inds;

  return inds;
}

int
CQChartsGroupPlot::
rowGroupInd(const CQChartsModelIndex &ind) const
{
  std::vector<int> inds;

  if (! rowGroupInds(ind, inds, /*hier*/false))
    return -1;

  assert(inds.size() == 1);

  int groupInd = inds[0];

  const_cast<CQChartsGroupPlot *>(this)->setModelGroupInd(ind, groupInd);

  return groupInd;
}

bool
CQChartsGroupPlot::
rowGroupInds(const CQChartsModelIndex &ind, std::vector<int> &inds, bool hier) const
{
  QAbstractItemModel *model = this->model().data();
  if (! model) return false;

  // header has multiple groups (one per column)
  if      (groupBucket_.dataType() == CQChartsColumnBucket::DataType::HEADER) {
    int ind1 = groupBucket_.ind(ind.column.column());

    inds.push_back(ind1);
  }
  // get group id from value in group column
  else if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN ||
           groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT) {
    bool ok;

    QVariant value;

    if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT)
       value = modelRootValue(ind.row, groupBucket_.column(), ind.parent, ok);
    else
       value = modelHierValue(ind.row, groupBucket_.column(), ind.parent, ok);

    if (! value.isValid())
      return false;

    int ind1 = -1;

    if      (groupBucket_.isExactValue()) {
      ind1 = groupBucket_.ind(value);

      inds.push_back(ind1);
    }
    else if (CQChartsVariant::isNumeric(value)) {
      bool ok;

      double r = CQChartsVariant::toReal(value, ok);

      ind1 = groupBucket_.bucket(r);

      inds.push_back(ind1);
    }
    else {
      if (hier) {
        inds = pathInds(value.toString());
      }
      else {
        ind1 = groupBucket_.sbucket(value);

        inds.push_back(ind1);
      }
    }
  }
  // get group id from parent path name
  else if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::PATH) {
    QString path = CQChartsUtil::parentPath(model, ind.parent);

    if (hier) {
      inds = pathInds(path);
    }
    else {
      int ind = groupBucket_.ind(path);

      inds.push_back(ind);
    }
  }
  else if (groupBucket_.isUseRow()) {
    int ind1 = ind.row; // default to row

    inds.push_back(ind1);
  }
  else {
    return false; // no bucket
  }

  return true;
}

std::vector<int>
CQChartsGroupPlot::
pathInds(const QString &path) const
{
  std::vector<int> inds;

  QStringList paths = pathStrs(path);

  for (int i = 0; i < paths.length(); ++i) {
    const QString &path1 = paths[i];

    int ind = groupBucket_.ind(path1);

    inds.push_back(ind);
  }

  return inds;
}

QStringList
CQChartsGroupPlot::
pathStrs(const QString &path) const
{
  QStringList paths;

  QStringList strs = path.split("/", QString::KeepEmptyParts);
  assert(strs.length() > 0);

  QString path1;

  for (int i = 0; i < strs.length(); ++i) {
    if (path1.length())
      path1 += "/";

    path1 += strs[i];

    paths.push_back(path1);
  }

  return paths;
}

void
CQChartsGroupPlot::
setModelGroupInd(const CQChartsModelIndex &ind, int groupInd)
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  QVariant var(groupInd);

  int role = (int) CQBaseModel::Role::Group;

  model->setHeaderData(ind.row, Qt::Vertical, var, role);
}

int
CQChartsGroupPlot::
numGroups() const
{
  return groupBucket_.numUnique();
}

//---

void
CQChartsGroupPlot::
getGroupInds(std::vector<int> &inds) const
{
  for (int groupInd = groupBucket_.imin(); groupInd <= groupBucket_.imax(); ++groupInd)
    inds.push_back(groupInd);
}

//---

QString
CQChartsGroupPlot::
groupIndName(int ind, bool hier) const
{
  if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN ||
      groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT) {
    if      (groupBucket_.isExactValue()) {
      return groupBucket_.indName(ind);
    }
    else if (groupBucket_.columnType() == ColumnType::REAL ||
             groupBucket_.columnType() == ColumnType::INTEGER) {
      return groupBucket_.bucketName(ind);
    }
    else {
      if (hier)
        return groupBucket_.iname(ind);
      else
        return groupBucket_.buckets(ind);
    }
  }
  else {
    return groupBucket_.indName(ind);
  }
}

//---

void
CQChartsGroupPlot::
printGroup() const
{
  groupBucket_.print(std::cerr);
}
