#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsRoundedPolygon.h>

#include <QPainter>
#include <QMenu>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
}

void
CQChartsBoxPlotType::
addParameters()
{
  startParameterGroup("Box Plot");

  addBoolParameter("horizontal", "Horizontal"  , "horizontal").setTip("draw bars horizontal");

  endParameterGroup();

  //---

  startParameterGroup("Raw Values");

  addColumnsParameter("value", "Value", "valueColumns", "1").setTip("value column(s)");
  addColumnParameter ("set"  , "Set"  , "setColumn"        ).setTip("X Values");

  endParameterGroup();

  //---

  startParameterGroup("Calculated Values");
  addColumnParameter("min"        , "Min"        , "minColumn"        ).
    setTip("Min Value");
  addColumnParameter("lowerMedian", "LowerMedian", "lowerMedianColumn").
    setTip("Lower Median Value");
  addColumnParameter("median"     , "Median"     , "medianColumn"     ).
    setTip("Media Value");
  addColumnParameter("upperMedian", "UpperMedian", "upperMedianColumn").
    setTip("Upper Median Value");
  addColumnParameter("max"        , "Max"        , "maxColumn"        ).
    setTip("Max Value");
  addColumnParameter("outliers"   , "Outliers"   , "outliersColumn"   ).
    setTip("Outlier Values");
  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsBoxPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws box and whiskers for the min, max, median and outlier values of the set "
         "of y values for rows with identical x values.\n"
         "<h2>Columns</h2>\n"
         "<p>Values can be supplied using:</p>\n"
         "<ul>\n"
         "<li>Raw Values with X and Y values in <b>x</b> and <b>y</b> columns.</li>\n"
         "<li>Calculated Values in the <b>min</b>, <b>lowerMedian</b>, <b>median</b>, "
         "<b>upperMedian</b>, <b>max</b> and <b>outliers</b> columns.</li>\n"
         "</ul>";
}

CQChartsPlot *
CQChartsBoxPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBoxPlot(view, model);
}

//---

CQChartsBoxPlot::
CQChartsBoxPlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("box"), model)
{
  setBoxColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBorderStroked(true);
  setBoxFilled    (true);

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolSize(4);
  setSymbolFilled(true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  addAxes();

  addKey();

  addTitle();
}

CQChartsBoxPlot::
~CQChartsBoxPlot()
{
}

//------

void
CQChartsBoxPlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumn_) {
    valueColumn_ = c;

    valueColumns_.clear();

    if (valueColumn_.isValid())
      valueColumns_.push_back(valueColumn_);

    updateRangeAndObjs();
  }
}

void
CQChartsBoxPlot::
setValueColumns(const Columns &valueColumns)
{
  if (valueColumns != valueColumns_) {
    valueColumns_ = valueColumns;

    if (! valueColumns_.empty())
      valueColumn_ = valueColumnAt(0);
    else
      valueColumn_ = CQChartsColumn();

    updateRangeAndObjs();
  }
}

QString
CQChartsBoxPlot::
valueColumnsStr() const
{
  return CQChartsColumn::columnsToString(valueColumns());
}

bool
CQChartsBoxPlot::
setValueColumnsStr(const QString &s)
{
  Columns valueColumns;

  if (! CQChartsColumn::stringToColumns(s, valueColumns))
    return false;

  setValueColumns(valueColumns);

  return true;
}

//---

bool
CQChartsBoxPlot::
isBoxFilled() const
{
  return boxData_.shape.background.visible;
}

void
CQChartsBoxPlot::
setBoxFilled(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
boxColor() const
{
  return boxData_.shape.background.color;
}

void
CQChartsBoxPlot::
setBoxColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.color, c, [&]() { update(); } );
}

QColor
CQChartsBoxPlot::
interpBoxColor(int i, int n) const
{
  return boxColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
boxAlpha() const
{
  return boxData_.shape.background.alpha;
}

void
CQChartsBoxPlot::
setBoxAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.alpha, a, [&]() { update(); } );
}

CQChartsBoxPlot::Pattern
CQChartsBoxPlot::
boxPattern() const
{
  return (Pattern) boxData_.shape.background.pattern;
}

void
CQChartsBoxPlot::
setBoxPattern(Pattern pattern)
{
  if (pattern != (Pattern) boxData_.shape.background.pattern) {
    boxData_.shape.background.pattern = (CQChartsFillData::Pattern) pattern;

    update();
  }
}

//------

bool
CQChartsBoxPlot::
isBorderStroked() const
{
  return boxData_.shape.border.visible;
}

void
CQChartsBoxPlot::
setBorderStroked(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
borderColor() const
{
  return boxData_.shape.border.color;
}

void
CQChartsBoxPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.color, c, [&]() { update(); } );
}

QColor
CQChartsBoxPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
borderAlpha() const
{
  return boxData_.shape.border.alpha;
}

void
CQChartsBoxPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsBoxPlot::
borderWidth() const
{
  return boxData_.shape.border.width;
}

void
CQChartsBoxPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.width, l, [&]() { update(); } );
}

const CQChartsLength &
CQChartsBoxPlot::
cornerSize() const
{
  return boxData_.cornerSize;
}

void
CQChartsBoxPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(boxData_.cornerSize, s, [&]() { update(); } );
}

//------

const CQChartsColor &
CQChartsBoxPlot::
whiskerColor() const
{
  return whiskerData_.color;
}

void
CQChartsBoxPlot::
setWhiskerColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(whiskerData_.color, c, [&]() { update(); } );
}

double
CQChartsBoxPlot::
whiskerAlpha() const
{
  return whiskerData_.alpha;
}

void
CQChartsBoxPlot::
setWhiskerAlpha(double a)
{
  CQChartsUtil::testAndSet(whiskerData_.alpha, a, [&]() { update(); } );
}

QColor
CQChartsBoxPlot::
interpWhiskerColor(int i, int n) const
{
  return whiskerColor().interpColor(this, i, n);
}

//------

bool
CQChartsBoxPlot::
isTextVisible() const
{
  return textData_.visible;
}

void
CQChartsBoxPlot::
setTextVisible(bool b)
{
  CQChartsUtil::testAndSet(textData_.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsBoxPlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { update(); } );
}

double
CQChartsBoxPlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsBoxPlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { update(); } );
}

QColor
CQChartsBoxPlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

const QFont &
CQChartsBoxPlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsBoxPlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { update(); } );
}

//---

const CQChartsColor &
CQChartsBoxPlot::
symbolStrokeColor() const
{
  return symbolData_.stroke.color;
}

void
CQChartsBoxPlot::
setSymbolStrokeColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.color, c, [&]() { update(); } );
}

QColor
CQChartsBoxPlot::
interpSymbolStrokeColor(int i, int n) const
{
  return symbolStrokeColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
symbolStrokeAlpha() const
{
  return symbolData_.stroke.alpha;
}

void
CQChartsBoxPlot::
setSymbolStrokeAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.alpha, a, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
symbolFillColor() const
{
  return symbolData_.fill.color;
}

void
CQChartsBoxPlot::
setSymbolFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.fill.color, c, [&]() { update(); } );
}

QColor
CQChartsBoxPlot::
interpSymbolFillColor(int i, int n) const
{
  return symbolFillColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
symbolFillAlpha() const
{
  return symbolData_.fill.alpha;
}

void
CQChartsBoxPlot::
setSymbolFillAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.fill.alpha, a, [&]() { update(); } );
}

//---

void
CQChartsBoxPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns/raw", this, "valueColumns", "values");
  addProperty("columns/raw", this, "setColumn"   , "set"   );

  addProperty("columns/calculated", this, "minColumn"        , "min"        );
  addProperty("columns/calculated", this, "lowerMedianColumn", "lowerMedian");
  addProperty("columns/calculated", this, "medianColumn"     , "media"      );
  addProperty("columns/calculated", this, "upperMedianColumn", "upperMedian");
  addProperty("columns/calculated", this, "maxColumn"        , "max"        );
  addProperty("columns/calculated", this, "outliersColumn"   , "outlier"    );

  CQChartsGroupPlot::addProperties();

  // connect multiple whiskers
  addProperty("options", this, "connected" , "connected" );
  addProperty("options", this, "horizontal", "horizontal");

  // whisker box
  addProperty("box", this, "whiskerRange", "range" );
  addProperty("box", this, "boxWidth"    , "width" );
  addProperty("box", this, "boxLabels"   , "labels");

  // whisker box stroke
  addProperty("box/stroke", this, "boxStroked" , "visible"   );
  addProperty("box/stroke", this, "borderColor", "color"     );
  addProperty("box/stroke", this, "borderAlpha", "alpha"     );
  addProperty("box/stroke", this, "borderWidth", "width"     );
  addProperty("box/stroke", this, "cornerSize" , "cornerSize");

  // whisker box fill
  addProperty("box/fill", this, "boxFilled" , "visible");
  addProperty("box/fill", this, "boxColor"  , "color"  );
  addProperty("box/fill", this, "boxAlpha"  , "alpha"  );
  addProperty("box/fill", this, "boxPattern", "pattern");

  // whisker line
  addProperty("whisker", this, "whiskerColor"    , "color" );
  addProperty("whisker", this, "whiskerAlpha"    , "alpha" );
  addProperty("whisker", this, "whiskerLineWidth", "width" );
  addProperty("whisker", this, "whiskerExtent"   , "extent");

  // value labels
  addProperty("labels", this, "textVisible", "visible");
  addProperty("labels", this, "textFont"   , "font"   );
  addProperty("labels", this, "textColor"  , "color"  );
  addProperty("labels", this, "textAlpha"  , "alpha"  );
  addProperty("labels", this, "textMargin" , "margin" );

  addProperty("outlier"       , this, "showOutliers"     , "visible");
  addProperty("outlier"       , this, "symbolType"       , "symbol" );
  addProperty("outlier"       , this, "symbolSize"       , "size"   );
  addProperty("outlier/stroke", this, "symbolStroked"    , "visible");
  addProperty("outlier/stroke", this, "symbolStrokeColor", "color"  );
  addProperty("outlier/stroke", this, "symbolStrokeAlpha", "alpha"  );
  addProperty("outlier/stroke", this, "symbolLineWidth"  , "width"  );
  addProperty("outlier/fill"  , this, "symbolFilled"     , "visible");
  addProperty("outlier/fill"  , this, "symbolFillColor"  , "color"  );
  addProperty("outlier/fill"  , this, "symbolFillAlpha"  , "alpha"  );
}

//---

void
CQChartsBoxPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() { updateRangeAndObjs(); } );
}

//---

bool
CQChartsBoxPlot::
isPreCalc() const
{
  return (minColumn        ().isValid() &&
          lowerMedianColumn().isValid() &&
          medianColumn     ().isValid() &&
          upperMedianColumn().isValid() &&
          maxColumn        ().isValid());
}

void
CQChartsBoxPlot::
updateRange(bool apply)
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  xAxis->setIntegral      (true);
  xAxis->setMajorIncrement(1);

  yAxis->setIntegral      (false);
  yAxis->setMajorIncrement(0);

  //---

  if (! isPreCalc())
    updateRawRange();
  else
    updateCalcRange();

  if (apply)
    applyDataRange();
}

// calculate box plot from individual values
void
CQChartsBoxPlot::
updateRawRange()
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  yAxis->setVisible(true);

  //---

  initGroupData(valueColumns(), CQChartsColumn());

#if 0
  // non-range use group columns for grouping
  CQChartsGroupData groupData;

  groupData.exactValue = isExactValue();
  groupData.bucketer   = groupData_.bucketer;
  groupData.usePath    = false;

  // use multiple group columns
  if      (valueColumns().size() > 1) {
    groupData.columns     = valueColumns();
    groupData.rowGrouping = isRowGrouping(); // only used if multiple groups
    groupData.column      = groupColumn();
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
  // default use name column if defined
  else {
    //groupData.column = nameColumn();
  }

  initGroup(groupData);
#endif

  //---

  updateRawWhiskers();

  //---

  dataRange_.reset();

  xAxis->clearTickLabels();

  int pos = 0;
  int ig  = 0;

  bool hasColumnGroups = (valueColumns().size() > 1 && ! isRowGrouping());
  bool hasGroups       = (this->groupWhiskers().size() > 1);

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    bool hasSets = (setWhiskerMap.size() > 1);

    int is = 0;

    for (const auto &setWhiskers : setWhiskerMap) {
      bool hidden = (isGrouped() ? isSetHidden(ig) : isSetHidden(is));

      if (! hidden) {
        const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        //---

        QString tickName;

        if      (hasColumnGroups) {
          tickName = whisker.name();
        }
        else if (hasSets) {
          int setId = setWhiskers.first;

          tickName = this->setIdName(setId);
        }
        else if (hasGroups) {
          tickName = groupIndName(groupInd);
        }

        if (tickName.length())
          xAxis->setTickLabel(pos, tickName);

        //---

        double min, max;

        if (isShowOutliers()) {
          min = whisker.rvalue(0);
          max = whisker.rvalue(whisker.numValues() - 1);
        }
        else {
          min = whisker.min();
          max = whisker.max();
        }

        if (! isHorizontal()) {
          dataRange_.updateRange(pos - 0.5, min);
          dataRange_.updateRange(pos + 0.5, max);
        }
        else {
          dataRange_.updateRange(min, pos - 0.5);
          dataRange_.updateRange(max, pos + 0.5);
        }

        ++pos;
      }

      ++is;
    }

    ++ig;
  }

  //---

//xAxis->setColumn(setColumn());
  yAxis->setColumn(valueColumn());

  //---

  bool ok;

  QString xname = modelHeaderString(setColumn(), ok);

  xAxis->setLabel(xname);

  if (valueColumns().size() == 1) {
    QString yname = modelHeaderString(valueColumn(), ok);

    yAxis->setLabel(yname);
  }
}

// calculate box plot from pre-calculated values
void
CQChartsBoxPlot::
updateCalcRange()
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  yAxis->setVisible(false);

  //---

  dataRange_.reset();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    using DataList = CQChartsBoxPlot::WhiskerDataList;

   public:
    BoxPlotVisitor(CQChartsBoxPlot *plot, CQChartsAxis *xAxis) :
     plot_(plot), xAxis_(xAxis) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      CQChartsBoxWhiskerData data;

      bool ok;

      data.ind    = ind;
      data.x      = plot_->modelReal(row, plot_->xColumn          (), ind, ok);
      data.min    = plot_->modelReal(row, plot_->minColumn        (), ind, ok);
      data.lower  = plot_->modelReal(row, plot_->lowerMedianColumn(), ind, ok);
      data.median = plot_->modelReal(row, plot_->medianColumn     (), ind, ok);
      data.upper  = plot_->modelReal(row, plot_->upperMedianColumn(), ind, ok);
      data.max    = plot_->modelReal(row, plot_->maxColumn        (), ind, ok);

      data.dataMin = data.min;
      data.dataMax = data.max;

      if (plot_->isShowOutliers()) {
        data.outliers = plot_->modelReals(row, plot_->outliersColumn(), ind, ok);

        for (auto &o : data.outliers) {
          data.dataMin = std::min(data.dataMin, o);
          data.dataMax = std::max(data.dataMax, o);
        }
      }

      //---

      bool nameValid = true;

      data.name = plot_->modelString(row, plot_->idColumn(), ind, ok);

      if (! ok || ! data.name.length()) {
        data.name = plot_->modelHeaderString(row, Qt::Vertical, ok);

        if (! ok || ! data.name.length()) {
          data.name = QString("%1").arg(row);
          nameValid = false;
        }
      }

      if (nameValid)
        xAxis_->setTickLabel(row, data.name);

      //---

      dataList_.push_back(data);

      return State::OK;
    }

    const DataList &dataList() const { return dataList_; }

   private:
    CQChartsBoxPlot *plot_  { nullptr };
    CQChartsAxis    *xAxis_ { nullptr };
    DataList         dataList_;
  };

  BoxPlotVisitor boxPlotVisitor(this, xAxis);

  visitModel(boxPlotVisitor);

  //---

  whiskerDataList_ = boxPlotVisitor.dataList();

  int n = whiskerDataList_.size();

  if (n > 0) {
    if (! isHorizontal()) {
      dataRange_.updateRange(  - 0.5, 0.0);
      dataRange_.updateRange(n - 0.5, 1.0);
    }
    else {
      dataRange_.updateRange(0.0,   - 0.5);
      dataRange_.updateRange(1.0, n - 0.5);
    }
  }
  else {
    dataRange_.updateRange(0.0, 0.0);
    dataRange_.updateRange(1.0, 1.0);
  }

  //---

  bool ok;

  QString xname = modelHeaderString(xColumn(), ok);

  xAxis->setLabel(xname);
}

void
CQChartsBoxPlot::
updateRawWhiskers()
{
  groupWhiskers_.clear();

  //---

  // x data type
  setType_ = columnValueType(setColumn());

  setValueInd_.clear();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    BoxPlotVisitor(CQChartsBoxPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      plot_->addRawWhiskerRow(ind, row);

      return State::OK;
    }

   private:
    CQChartsBoxPlot *plot_ { nullptr };
  };

  BoxPlotVisitor boxPlotVisitor(this);

  visitModel(boxPlotVisitor);

  //---

  grouped_ = (numGroups() > 1);

  //---

  for (auto &groupIdWhiskers : groupWhiskers_) {
    SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    for (auto &setWhiskers : setWhiskerMap)
      setWhiskers.second.init();
  }
}

void
CQChartsBoxPlot::
addRawWhiskerRow(const QModelIndex &parent, int row)
{
  // get value set id
  int setId = -1;

  if (setColumn().isValid()) {
    if      (setType_ == ColumnType::INTEGER) {
      bool ok1;

      int i = modelHierInteger(row, setColumn(), parent, ok1);

      setId = setValueInd_.calcId(i);
    }
    else if (setType_ == ColumnType::REAL) {
      bool ok1;

      double r = modelHierReal(row, setColumn(), parent, ok1);

      if (CQChartsUtil::isNaN(r))
        return;

      setId = setValueInd_.calcId(r);
    }
    else {
      bool ok1;

      QString s = modelHierString(row, setColumn(), parent, ok1);

      setId = setValueInd_.calcId(s);
    }
  }

  //---

  for (const auto &valueColumn : valueColumns()) {
    // get group
    int groupInd = rowGroupInd(parent, row, valueColumn);

    //---

    // add value to set
    bool ok2;

    double value = modelReal(row, valueColumn, parent, ok2);

    if (! ok2) value = row;

    if (CQChartsUtil::isNaN(value))
      return;

    QModelIndex xind  = modelIndex(row, setColumn(), parent);
    QModelIndex xind1 = normalizeIndex(xind);

    CQChartsBoxPlotValue wv(value, xind1);

    auto p = groupWhiskers_.find(groupInd);

    if (p == groupWhiskers_.end()) {
      SetWhiskerMap setWhiskerMap;

      p = groupWhiskers_.insert(p, GroupSetWhiskerMap::value_type(groupInd, SetWhiskerMap()));
    }

    SetWhiskerMap &setWhiskerMap = (*p).second;

    auto p1 = setWhiskerMap.find(setId);

    if (p1 == setWhiskerMap.end()) {
      CQChartsBoxPlotWhisker whisker;

      whisker.setRange(whiskerRange());

      if (valueColumns().size() > 1 && ! isRowGrouping()) {
        bool ok;

        QString name = modelHeaderString(valueColumn, ok);

        if (ok && name.length())
          whisker.setName(name);
      }

      p1 = setWhiskerMap.insert(p1, SetWhiskerMap::value_type(setId, whisker));
    }

    (*p1).second.addValue(wv);
  }
}

CQChartsGeom::BBox
CQChartsBoxPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsBoxPlotWhiskerObj *boxObj = dynamic_cast<CQChartsBoxPlotWhiskerObj *>(plotObj);

    if (boxObj)
      bbox += boxObj->annotationBBox();
  }

  return bbox;
}

//------

bool
CQChartsBoxPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  if (! isPreCalc())
    return initRawObjs();
  else
    return initCalcObjs();
}

bool
CQChartsBoxPlot::
initRawObjs()
{
  int ig = 0;
  int ng = numGroups();

  int pos = 0;

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (! isConnected()) {
      int is = 0;
      int ns = setWhiskerMap.size();

      for (const auto &setWhiskers : setWhiskerMap) {
        bool hidden = (isGrouped() ? isSetHidden(ig) : isSetHidden(is));

        if (! hidden) {
          int setId = setWhiskers.first;

          const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

          //----

          CQChartsGeom::BBox rect;

          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - 0.10, whisker.lower(), pos + 0.10, whisker.upper());
          else
            rect = CQChartsGeom::BBox(whisker.lower(), pos - 0.10, whisker.upper(), pos + 0.10);

          CQChartsBoxPlotWhiskerObj *boxObj =
            new CQChartsBoxPlotWhiskerObj(this, rect, setId, whisker, ig, ng, is, ns);

          addPlotObject(boxObj);

          ++pos;
        }

        ++is;
      }
    }
    else {
      bool hidden = (isGrouped() ? isSetHidden(ig) : false);

      if (! hidden) {
        CQChartsGeom::BBox rect(dataRange_.xmin(), dataRange_.ymin(),
                                dataRange_.xmax(), dataRange_.ymax());

        CQChartsBoxPlotConnectedObj *connectedObj =
          new CQChartsBoxPlotConnectedObj(this, rect, groupInd, ig, ng);

        addPlotObject(connectedObj);
      }
    }

    ++ig;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

bool
CQChartsBoxPlot::
initCalcObjs()
{
  int pos = 0;

  for (const auto &whiskerData : whiskerDataList_) {
    CQChartsGeom::BBox rect;

    if (! isHorizontal())
      rect = CQChartsGeom::BBox(pos - 0.10, 0.0, pos + 0.10, 1.0);
    else
      rect = CQChartsGeom::BBox(0.0, pos - 0.10, 1.0, pos + 0.10);

    CQChartsBoxPlotDataObj *boxObj = new CQChartsBoxPlotDataObj(this, rect, whiskerData);

    addPlotObject(boxObj);

    ++pos;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsBoxPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ig = 0;
  int ng = numGroups();

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    QString groupName = groupIndName(groupInd);

    int is = 0;
    int ns = setWhiskerMap.size();

    for (const auto &setWhiskers : setWhiskerMap) {
      int setId = setWhiskers.first;

      QString setName = this->setIdName(setId);

      QString name = (isGrouped() ? groupName : setName);

      int i = (isGrouped() ? ig : is);
      int n = (isGrouped() ? ng : ns);

      CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, i, n);
      CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, i, name);

      key->addItem(color, i, 0);
      key->addItem(text , i, 1);

      if (isGrouped())
        break;

      ++is;
    }

    ++ig;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

bool
CQChartsBoxPlot::
probe(ProbeData &probeData) const
{
  if (! dataRange_.isSet())
    return false;

  if (! isHorizontal()) {
    if (probeData.x < dataRange_.xmin() + 0.5)
      probeData.x = dataRange_.xmin() + 0.5;

    if (probeData.x > dataRange_.xmax() - 0.5)
      probeData.x = dataRange_.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    if (probeData.y < dataRange_.ymin() + 0.5)
      probeData.y = dataRange_.ymin() + 0.5;

    if (probeData.y > dataRange_.ymax() - 0.5)
      probeData.y = dataRange_.ymax() - 0.5;

    probeData.y = std::round(probeData.y);
  }

  return true;
}

//------

bool
CQChartsBoxPlot::
addMenuItems(QMenu *menu)
{
  QAction *horizontalAction = new QAction("Horizontal", menu);

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));

  menu->addSeparator();

  menu->addAction(horizontalAction);

  return true;
}

//------

void
CQChartsBoxPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsBoxPlotWhiskerObj::
CQChartsBoxPlotWhiskerObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                          const CQChartsBoxPlotWhisker &whisker, int ig, int ng, int is, int ns) :
 CQChartsBoxPlotObj(plot, rect), setId_(setId), whisker_(whisker),
 ig_(ig), ng_(ng), is_(is), ns_(ns)
{
}

QString
CQChartsBoxPlotWhiskerObj::
calcId() const
{
  QString setName = plot_->setIdName(setId_);

  return QString("%1:%2:%3").arg(setName).arg(whisker_.lower()).arg(whisker_.upper());
}

void
CQChartsBoxPlotWhiskerObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->setColumn  ());
  addColumnSelectIndex(inds, plot_->valueColumn());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

void
CQChartsBoxPlotWhiskerObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    for (auto &value : whisker_.values()) {
      addSelectIndex(inds, value.ind.row(), column, value.ind.parent());
    }
  }
}

void
CQChartsBoxPlotWhiskerObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double pos;

  if (! plot_->isHorizontal())
    pos = rect_.getXMid();
  else
    pos = rect_.getYMid();

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (! plot_->isHorizontal()) {
    plot_->windowToPixel(pos - wd1, whisker_.min   (), px1, py1);
    plot_->windowToPixel(pos - wd2, whisker_.lower (), px2, py2);
    plot_->windowToPixel(pos      , whisker_.median(), px3, py3);
    plot_->windowToPixel(pos + wd2, whisker_.upper (), px4, py4);
    plot_->windowToPixel(pos + wd1, whisker_.max   (), px5, py5);
  }
  else {
    plot_->windowToPixel(whisker_.min   (), pos - wd1, px1, py1);
    plot_->windowToPixel(whisker_.lower (), pos - wd2, px2, py2);
    plot_->windowToPixel(whisker_.median(), pos      , px3, py3);
    plot_->windowToPixel(whisker_.upper (), pos + wd2, px4, py4);
    plot_->windowToPixel(whisker_.max   (), pos + wd1, px5, py5);
  }

  //---

  bool hasRange = (fabs(whisker_.max() - whisker_.min()) > 1E-6);

  //---

  QColor whiskerColor = plot_->interpWhiskerColor(0, 1);

  whiskerColor.setAlphaF(plot_->whiskerAlpha());

  double whiskerLineSize;

  if (! plot_->isHorizontal())
    whiskerLineSize = plot_->lengthPixelWidth (plot_->whiskerLineWidth());
  else
    whiskerLineSize = plot_->lengthPixelHeight(plot_->whiskerLineWidth());

  //---

  // draw extent line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));
  else
    painter->drawLine(QPointF(px1, py3), QPointF(px5, py3));

  //---

  // draw lower/upper horizontal lines
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal()) {
    painter->drawLine(QPointF(px1, py1), QPointF(px5, py1));
    painter->drawLine(QPointF(px1, py5), QPointF(px5, py5));
  }
  else {
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py5));
    painter->drawLine(QPointF(px5, py1), QPointF(px5, py5));
  }

  //---

  // draw box
  QRectF rect = QRectF(px2, py2, px4 - px2, py4 - py2);

  // set fill and stroke
  QBrush brush;

  if (plot_->isBoxFilled()) {
    QColor boxColor = (plot_->isGrouped() ? plot_->interpBoxColor(ig_, ng_) :
                                            plot_->interpBoxColor(is_, ns_));

    boxColor.setAlphaF(plot_->boxAlpha());

    brush.setColor(boxColor);
    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->boxPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorderStroked()) {
    QColor borderColor = (plot_->isGrouped() ? plot_->interpBorderColor(ig_, ng_) :
                                               plot_->interpBorderColor(is_, ns_));

    borderColor.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (borderColor);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
  double cys = plot_->lengthPixelHeight(plot_->cornerSize());

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);

  //---

  // draw median line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));
  else
    painter->drawLine(QPointF(px3, py2), QPointF(px3, py4));

  //---

  // draw labels
  if (plot_->isTextVisible()) {
    painter->setFont(plot_->textFont());

    QColor tc = plot_->interpTextColor(0, 1);

    tc.setAlphaF(plot_->textAlpha());

    painter->setPen(tc);

    if (hasRange) {
      QString strl = QString("%1").arg(whisker_.min   ());
      QString lstr = QString("%1").arg(whisker_.lower ());
      QString mstr = QString("%1").arg(whisker_.median());
      QString ustr = QString("%1").arg(whisker_.upper ());
      QString strh = QString("%1").arg(whisker_.max   ());

      if (! plot_->isHorizontal()) {
        drawHText(painter, px1, px5, py1, strl, /*onLeft*/true );
        drawHText(painter, px2, px4, py2, lstr, /*onLeft*/false);
        drawHText(painter, px2, px4, py3, mstr, /*onLeft*/true );
        drawHText(painter, px2, px4, py4, ustr, /*onLeft*/false);
        drawHText(painter, px1, px5, py5, strh, /*onLeft*/true );
      }
      else {
        drawVText(painter, py1, py5, px1, strl, /*onBottom*/false);
        drawVText(painter, py2, py4, px2, lstr, /*onBottom*/true );
        drawVText(painter, py2, py4, px3, mstr, /*onBottom*/false);
        drawVText(painter, py2, py4, px4, ustr, /*onBottom*/true );
        drawVText(painter, py1, py5, px5, strh, /*onBottom*/false);
      }
    }
    else {
      QString strl = QString("%1").arg(whisker_.min());

      if (! plot_->isHorizontal())
        drawHText(painter, px1, px5, py1, strl, /*onLeft*/true);
      else
        drawVText(painter, py1, py5, px1, strl, /*onBottom*/false);
    }
  }

  //---

  // draw outlier symbols
  if (plot_->isShowOutliers()) {
    CQChartsSymbol symbol      = plot_->symbolType();
    double         symbolSize  = plot_->symbolSize();
    bool           stroked     = plot_->isSymbolStroked();
    QColor         strokeColor = plot_->interpSymbolStrokeColor(0, 1);
    bool           filled      = plot_->isSymbolFilled();
    QColor         fillColor   = plot_->interpSymbolFillColor(0, 1);

    QPen   pen  (strokeColor);
    QBrush brush(fillColor);

    plot_->updateObjPenBrushState(this, pen, brush);

    for (auto &o : whisker_.outliers()) {
      double px1, py1;

      if (! plot_->isHorizontal())
        plot_->windowToPixel(pos, whisker_.rvalue(o), px1, py1);
      else
        plot_->windowToPixel(whisker_.rvalue(o), pos, px1, py1);

      plot_->drawSymbol(painter, QPointF(px1, py1), symbol, symbolSize,
                        stroked, pen.color(), 1, filled, brush.color());
    }
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotWhiskerObj::
annotationBBox() const
{
  double pos;

  if (! plot_->isHorizontal())
    pos = rect_.getXMid();
  else
    pos = rect_.getYMid();

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (! plot_->isHorizontal()) {
    plot_->windowToPixel(pos - wd1, whisker_.min   (), px1, py1);
    plot_->windowToPixel(pos - wd2, whisker_.lower (), px2, py2);
    plot_->windowToPixel(pos      , whisker_.median(), px3, py3);
    plot_->windowToPixel(pos + wd2, whisker_.upper (), px4, py4);
    plot_->windowToPixel(pos + wd1, whisker_.max   (), px5, py5);
  }
  else {
    plot_->windowToPixel(whisker_.min   (), pos - wd1, px1, py1);
    plot_->windowToPixel(whisker_.lower (), pos - wd2, px2, py2);
    plot_->windowToPixel(whisker_.median(), pos      , px3, py3);
    plot_->windowToPixel(whisker_.upper (), pos + wd2, px4, py4);
    plot_->windowToPixel(whisker_.max   (), pos + wd1, px5, py5);
  }

  //---

  bool hasRange = (fabs(whisker_.max() - whisker_.min()) > 1E-6);

  //---

  CQChartsGeom::BBox pbbox;

  if (plot_->isTextVisible()) {
    if (hasRange) {
      QString strl = QString("%1").arg(whisker_.min   ());
      QString lstr = QString("%1").arg(whisker_.lower ());
      QString mstr = QString("%1").arg(whisker_.median());
      QString ustr = QString("%1").arg(whisker_.upper ());
      QString strh = QString("%1").arg(whisker_.max   ());

      if (! plot_->isHorizontal()) {
        addHBBox(pbbox, px1, px5, py1, strl, /*onLeft*/false);
        addHBBox(pbbox, px2, py4, py2, lstr, /*onLeft*/true );
        addHBBox(pbbox, px2, px4, py3, mstr, /*onLeft*/false);
        addHBBox(pbbox, px2, px4, py4, ustr, /*onLeft*/true );
        addHBBox(pbbox, px1, px5, py5, strh, /*onLeft*/false);
      }
      else {
        addVBBox(pbbox, py1, py5, px1, strl, /*onBottom*/true );
        addVBBox(pbbox, py2, py4, px2, lstr, /*onBottom*/false);
        addVBBox(pbbox, py2, py4, px3, mstr, /*onBottom*/true );
        addVBBox(pbbox, py2, py4, px4, ustr, /*onBottom*/false);
        addVBBox(pbbox, py1, py5, px5, strh, /*onBottom*/true );
      }
    }
    else {
      QString strl = QString("%1").arg(whisker_.min   ());

      if (! plot_->isHorizontal())
        addHBBox(pbbox, px1, px5, py1, strl, /*onLeft*/false);
      else
        addVBBox(pbbox, py1, py5, px1, strl, /*onBottom*/true);
    }
  }
  else {
    if (! plot_->isHorizontal()) {
      pbbox += CQChartsGeom::Point(px5, py1);
      pbbox += CQChartsGeom::Point(px2, py2);
      pbbox += CQChartsGeom::Point(px4, py3);
      pbbox += CQChartsGeom::Point(px2, py4);
      pbbox += CQChartsGeom::Point(px5, py5);
    }
    else {
      pbbox += CQChartsGeom::Point(px1, py5);
      pbbox += CQChartsGeom::Point(px2, py2);
      pbbox += CQChartsGeom::Point(px3, py4);
      pbbox += CQChartsGeom::Point(px4, py2);
      pbbox += CQChartsGeom::Point(px5, py5);
    }
  }

  //---

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(pbbox, bbox);

  return bbox;
}

//------

CQChartsBoxPlotDataObj::
CQChartsBoxPlotDataObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                       const CQChartsBoxWhiskerData &data) :
 CQChartsBoxPlotObj(plot, rect), data_(data)
{
}

QString
CQChartsBoxPlotDataObj::
calcId() const
{
  return data_.name;
}

void
CQChartsBoxPlotDataObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn          ());
  addColumnSelectIndex(inds, plot_->minColumn        ());
  addColumnSelectIndex(inds, plot_->lowerMedianColumn());
  addColumnSelectIndex(inds, plot_->medianColumn     ());
  addColumnSelectIndex(inds, plot_->upperMedianColumn());
  addColumnSelectIndex(inds, plot_->maxColumn        ());
  addColumnSelectIndex(inds, plot_->outliersColumn   ());
}

void
CQChartsBoxPlotDataObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    addSelectIndex(inds, data_.ind.row(), column, data_.ind.parent());
  }
}

void
CQChartsBoxPlotDataObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double pos;

  if (! plot_->isHorizontal())
    pos = rect_.getXMid();
  else
    pos = rect_.getYMid();

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (! plot_->isHorizontal()) {
    plot_->windowToPixel(pos - wd1, remapPos(data_.min   ), px1, py1);
    plot_->windowToPixel(pos - wd2, remapPos(data_.lower ), px2, py2);
    plot_->windowToPixel(pos      , remapPos(data_.median), px3, py3);
    plot_->windowToPixel(pos + wd2, remapPos(data_.upper ), px4, py4);
    plot_->windowToPixel(pos + wd1, remapPos(data_.max   ), px5, py5);
  }
  else {
    plot_->windowToPixel(remapPos(data_.min   ), pos - wd1, px1, py1);
    plot_->windowToPixel(remapPos(data_.lower ), pos - wd2, px2, py2);
    plot_->windowToPixel(remapPos(data_.median), pos      , px3, py3);
    plot_->windowToPixel(remapPos(data_.upper ), pos + wd2, px4, py4);
    plot_->windowToPixel(remapPos(data_.max   ), pos + wd1, px5, py5);
  }

  //---

  QColor whiskerColor = plot_->interpWhiskerColor(0, 1);

  whiskerColor.setAlphaF(plot_->whiskerAlpha());

  double whiskerLineSize;

  if (! plot_->isHorizontal())
    whiskerLineSize = plot_->lengthPixelWidth (plot_->whiskerLineWidth());
  else
    whiskerLineSize = plot_->lengthPixelHeight(plot_->whiskerLineWidth());

  //---

  // draw extent line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));
  else
    painter->drawLine(QPointF(px1, py3), QPointF(px5, py3));

  //---

  // draw lower/upper horizontal lines
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal()) {
    painter->drawLine(QPointF(px1, py1), QPointF(px5, py1));
    painter->drawLine(QPointF(px1, py5), QPointF(px5, py5));
  }
  else {
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py5));
    painter->drawLine(QPointF(px5, py1), QPointF(px5, py5));
  }

  //---

  // draw box
  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  // set fill and stroke
  QBrush brush;

  if (plot_->isBoxFilled()) {
    QColor boxColor = plot_->interpBoxColor(0, 1);

    boxColor.setAlphaF(plot_->boxAlpha());

    brush.setColor(boxColor);
    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->boxPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorderStroked()) {
    QColor borderColor = plot_->interpBorderColor(0, 1);

    borderColor.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (borderColor);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
  double cys = plot_->lengthPixelHeight(plot_->cornerSize());

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);

  //---

  // draw median line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));
  else
    painter->drawLine(QPointF(px3, py2), QPointF(px3, py4));

  //---

  // draw labels
  if (plot_->isTextVisible()) {
    painter->setFont(plot_->textFont());

    QColor tc = plot_->interpTextColor(0, 1);

    tc.setAlphaF(plot_->textAlpha());

    painter->setPen(tc);

    QString strl = QString("%1").arg(data_.min   );
    QString lstr = QString("%1").arg(data_.lower );
    QString mstr = QString("%1").arg(data_.median);
    QString ustr = QString("%1").arg(data_.upper );
    QString strh = QString("%1").arg(data_.max   );

    if (! plot_->isHorizontal()) {
      drawHText(painter, px1, px5, py1, strl, /*onLeft*/false);
      drawHText(painter, px2, py4, py2, lstr, /*onLeft*/true );
      drawHText(painter, px2, px4, py3, mstr, /*onLeft*/false);
      drawHText(painter, px2, px4, py4, ustr, /*onLeft*/true );
      drawHText(painter, px1, px5, py5, strh, /*onLeft*/false);
    }
    else {
      drawVText(painter, py1, py5, px1, strl, /*onBottom*/true );
      drawVText(painter, py2, py4, px2, lstr, /*onBottom*/false);
      drawVText(painter, py2, py4, px3, mstr, /*onBottom*/true );
      drawVText(painter, py2, py4, px4, ustr, /*onBottom*/false);
      drawVText(painter, py1, py5, px5, strh, /*onBottom*/true );
    }
  }

  //---

  // draw outlier symbols
  if (plot_->isShowOutliers()) {
    CQChartsSymbol symbol      = plot_->symbolType();
    double         symbolSize  = plot_->symbolSize();
    bool           stroked     = plot_->isSymbolStroked();
    QColor         strokeColor = plot_->interpSymbolStrokeColor(0, 1);
    bool           filled      = plot_->isSymbolFilled();
    QColor         fillColor   = plot_->interpSymbolFillColor(0, 1);

    QPen   pen  (strokeColor);
    QBrush brush(fillColor);

    plot_->updateObjPenBrushState(this, pen, brush);

    for (auto &o : data_.outliers) {
      double px1, py1;

      if (! plot_->isHorizontal())
        plot_->windowToPixel(pos, remapPos(o), px1, py1);
      else
        plot_->windowToPixel(remapPos(o), pos, px1, py1);

      plot_->drawSymbol(painter, QPointF(px1, py1), symbol, symbolSize,
                        stroked, pen.color(), 1, filled, brush.color());
    }
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotDataObj::
annotationBBox() const
{
  double pos;

  if (! plot_->isHorizontal())
    pos = rect_.getXMid();
  else
    pos = rect_.getYMid();

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (! plot_->isHorizontal()) {
    plot_->windowToPixel(pos - wd1, remapPos(data_.min   ), px1, py1);
    plot_->windowToPixel(pos - wd2, remapPos(data_.lower ), px2, py2);
    plot_->windowToPixel(pos      , remapPos(data_.median), px3, py3);
    plot_->windowToPixel(pos + wd2, remapPos(data_.upper ), px4, py4);
    plot_->windowToPixel(pos + wd1, remapPos(data_.max   ), px5, py5);
  }
  else {
    plot_->windowToPixel(remapPos(data_.min   ), pos - wd1, px1, py1);
    plot_->windowToPixel(remapPos(data_.lower ), pos - wd2, px2, py2);
    plot_->windowToPixel(remapPos(data_.median), pos      , px3, py3);
    plot_->windowToPixel(remapPos(data_.upper ), pos + wd2, px4, py4);
    plot_->windowToPixel(remapPos(data_.max   ), pos + wd1, px5, py5);
  }

  //---

  CQChartsGeom::BBox pbbox;

  if (plot_->isTextVisible()) {
    QString strl = QString("%1").arg(data_.min   );
    QString lstr = QString("%1").arg(data_.lower );
    QString mstr = QString("%1").arg(data_.median);
    QString ustr = QString("%1").arg(data_.upper );
    QString strh = QString("%1").arg(data_.max   );

    if (! plot_->isHorizontal()) {
      addHBBox(pbbox, px1, px5, py1, strl, /*onLeft*/false);
      addHBBox(pbbox, px2, py4, py2, lstr, /*onLeft*/true );
      addHBBox(pbbox, px2, px4, py3, mstr, /*onLeft*/false);
      addHBBox(pbbox, px2, px4, py4, ustr, /*onLeft*/true );
      addHBBox(pbbox, px1, px5, py5, strh, /*onLeft*/false);
    }
    else {
      addVBBox(pbbox, py1, py5, px1, strl, /*onBottom*/true );
      addVBBox(pbbox, py2, py4, px2, lstr, /*onBottom*/false);
      addVBBox(pbbox, py2, py4, px3, mstr, /*onBottom*/true );
      addVBBox(pbbox, py2, py4, px4, ustr, /*onBottom*/false);
      addVBBox(pbbox, py1, py5, px5, strh, /*onBottom*/true );
    }
  }
  else {
    pbbox += CQChartsGeom::Point(px5, py1);
    pbbox += CQChartsGeom::Point(px2, py2);
    pbbox += CQChartsGeom::Point(px4, py3);
    pbbox += CQChartsGeom::Point(px2, py4);
    pbbox += CQChartsGeom::Point(px5, py5);
  }

  //---

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(pbbox, bbox);

  return bbox;
}

double
CQChartsBoxPlotDataObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  return CQChartsUtil::map(y, data_.dataMin, data_.dataMax, ymargin_, 1.0 - ymargin_);
}

//------

CQChartsBoxPlotConnectedObj::
CQChartsBoxPlotConnectedObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                            int groupId, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupId_(groupId), i_(i), n_(n)
{
  initPolygon();
}

QString
CQChartsBoxPlotConnectedObj::
calcId() const
{
  QString groupName = plot_->groupIndName(groupId_);

  return QString("%1").arg(groupName);
}

void
CQChartsBoxPlotConnectedObj::
initPolygon()
{
  QPolygonF maxPoly, minPoly;

  int i = 0;

  for (const auto &groupIdWhiskers : plot_->groupWhiskers()) {
    const CQChartsBoxPlot::SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (i == i_) {
      for (const auto &setWhiskers : setWhiskerMap) {
        double                        pos     = setWhiskers.first;
        const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        double min    = whisker.min   ();
        double max    = whisker.max   ();
        double median = whisker.median();

        line_ << CQChartsUtil::toQPoint(CQChartsGeom::Point(pos, median));

        maxPoly << CQChartsUtil::toQPoint(CQChartsGeom::Point(pos, max));
        minPoly << CQChartsUtil::toQPoint(CQChartsGeom::Point(pos, min));
      }

      break;
    }

    ++i;
  }

  int np = maxPoly.count();

  for (int i = 0; i < np; ++i)
    poly_ << maxPoly.at(i);

  for (int i = 0; i < np; ++i)
    poly_ << minPoly.at(np - 1 - i);
}

bool
CQChartsBoxPlotConnectedObj::
inside(const CQChartsGeom::Point &p) const
{
  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsBoxPlotConnectedObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  // draw range polygon
  int np = poly_.count();

  if (np) {
    QPainterPath path;

    path.moveTo(plot_->windowToPixel(poly_.at(0)));

    for (int i = 1; i < np; ++i)
      path.lineTo(plot_->windowToPixel(poly_.at(i)));

    path.closeSubpath();

    QColor fillColor = plot_->interpBoxColor(i_, n_);

    QBrush pbrush;

    if (plot_->isBoxFilled()) {
      fillColor.setAlphaF(plot_->boxAlpha());

      pbrush.setColor(fillColor);
      pbrush.setStyle(CQChartsFillPattern::toStyle(
       (CQChartsFillPattern::Type) plot_->boxPattern()));
    }

    QPen ppen;

    if (plot_->isBorderStroked()) {
      QColor borderColor = plot_->interpBorderColor(i_, n_);

      borderColor.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth());

      ppen.setColor (borderColor);
      ppen.setWidthF(bw);
    }
    else {
      ppen.setStyle(Qt::NoPen);
    }

    plot_->updateObjPenBrushState(this, ppen, pbrush);

    painter->setPen  (ppen);
    painter->setBrush(pbrush);

    painter->drawPath(path);
  }

  //---

  // draw median line
  QPolygonF line;

  for (int i = 0; i < line_.count(); ++i)
    line << plot_->windowToPixel(line_.at(i));

  QPen lpen;

  QColor lineColor = plot_->interpBorderColor(i_, n_);

  lineColor.setAlphaF(plot_->borderAlpha());

  double bw = plot_->lengthPixelWidth(plot_->borderWidth());

  lpen.setColor (lineColor);
  lpen.setWidthF(bw);

  QBrush lbrush;

  plot_->updateObjPenBrushState(this, lpen, lbrush);

  painter->setPen(lpen);

  painter->drawPolyline(line);
}

//------

CQChartsBoxPlotObj::
CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(plot, rect), plot_(plot)
{
}

void
CQChartsBoxPlotObj::
drawHText(QPainter *painter, double xl, double xr, double y, const QString &text, bool onLeft)
{
  double margin = plot_->textMargin();

  bool invertX = plot_->isInvertX();

  if (invertX)
    onLeft = ! onLeft;

  double x = ((onLeft && ! invertX) || (! onLeft && invertX) ? xl : xr);

  QFontMetricsF fm(plot_->textFont());

  double yf = (fm.ascent() - fm.descent())/2.0;

  if (onLeft)
    painter->drawText(QPointF(x - margin - fm.width(text), y + yf), text);
  else
    painter->drawText(QPointF(x + margin, y + yf), text);
}

void
CQChartsBoxPlotObj::
drawVText(QPainter *painter, double yb, double yt, double x, const QString &text, bool onBottom)
{
  double margin = plot_->textMargin();

  bool invertY = plot_->isInvertY();

  if (invertY)
    onBottom = ! onBottom;

  double y = ((onBottom && ! invertY) || (! onBottom && invertY) ? yb : yt);

  QFontMetricsF fm(plot_->textFont());

  double xf = fm.width(text)/2.0;
  double fa = fm.ascent ();
  double fd = fm.descent();

  if (onBottom)
    painter->drawText(QPointF(x - xf, y + margin + fa), text);
  else
    painter->drawText(QPointF(x - xf, y - margin - fd), text);
}

void
CQChartsBoxPlotObj::
addHBBox(CQChartsGeom::BBox &pbbox, double xl, double xr, double y,
         const QString &text, bool onLeft) const
{
  double margin = plot_->textMargin();

  bool invertX = plot_->isInvertX();

  if (invertX)
    onLeft = ! onLeft;

  double x = ((onLeft && ! invertX) || (! onLeft && invertX) ? xl : xr);

  QFontMetricsF fm(plot_->textFont());

  double fa = fm.ascent ();
  double fd = fm.descent();
  double yf = (fa - fd)/2.0;

  double tx;

  if (onLeft)
    tx = x - margin - fm.width(text);
  else
    tx = x + margin + fm.width(text);

  pbbox += CQChartsGeom::Point(tx, y + yf - fa);
  pbbox += CQChartsGeom::Point(tx, y + yf + fd);
}

void
CQChartsBoxPlotObj::
addVBBox(CQChartsGeom::BBox &pbbox, double yb, double yt, double x,
         const QString &text, bool onBottom) const
{
  double margin = plot_->textMargin();

  bool invertY = plot_->isInvertY();

  if (invertY)
    onBottom = ! onBottom;

  double y = ((onBottom && ! invertY) || (! onBottom && invertY) ? yb : yt);

  QFontMetricsF fm(plot_->textFont());

  double xf = fm.width(text)/2.0;
  double fa = fm.ascent ();
  double fd = fm.descent();

  double ty;

  if (onBottom)
    ty = y + margin + fa;
  else
    ty = y - margin - fd;

  pbbox += CQChartsGeom::Point(x - xf, ty);
  pbbox += CQChartsGeom::Point(x + xf, ty);
}

//------

CQChartsBoxKeyColor::
CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsBoxKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsBoxKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

//------

CQChartsBoxKeyText::
CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsBoxKeyText::
interpTextColor(int i, int n) const
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
