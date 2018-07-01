#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>

#include <QMenu>
#include <QAction>
#include <QPainter>

CQChartsDistributionPlotType::
CQChartsDistributionPlotType()
{
}

void
CQChartsDistributionPlotType::
addParameters()
{
  startParameterGroup("Distribution");

  addColumnParameter("color", "Color", "colorColumn").
    setTip("Custom bar color");

  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("draw bars horizontal");

  endParameterGroup();

  //---

  // group data
  startParameterGroup("Grouping");

  addColumnParameter("group", "Group", "groupColumn").setTip("Group column");

//addBoolParameter("rowGrouping", "Row Grouping", "rowGrouping").
//  setTip("Group by group columns header instead of values");

//addBoolParameter("usePath", "Use Path", "usePath").
//  setTip("Use hierarchical path as group");

//addBoolParameter("useRow", "Use Row", "useRow").
//  setTip("Use row number for group");

  addBoolParameter("exactValue", "Exact Value", "exactValue", false).
   setTip("use exact value for grouping");

  addBoolParameter("autoRange", "Auto Range", "autoRange", true).
   setTip("automatically determine value range");

  addRealParameter("start", "Start", "startValue", 0.0).
    setRequired().setTip("Start value for manual range");
  addRealParameter("delta", "Delta", "deltaValue", 1.0).
    setRequired().setTip("Delta value for manual range");

  addIntParameter("numAuto", "Num Auto", "numAuto", 10).
    setRequired().setTip("Number of auto buckets");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsDistributionPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws bars with heights for the counts of set of values.</p>\n"
         "<h2>Columns</h2>\n"
         "<p>The values to be counted are taken from the <b>Value</b> column "
         "and grouped depending on the column value type. By default integer "
         "values are grouped by matching value, real values are automatically "
         "bucketed into ranges and strings are grouped by matching value.</p>\n"
         "<p>The color of the bar can be overridden using the <b>Color</b> column.</p>\n"
         "<h2>Parameters</h2>\n"
         "<p>Setting the <b>Horizontal</b> parameter draws the bars horizontally.</p>"
         "<p>The automatic bucketing of real values can overridden by turning off "
         "the <b>autoRange</b> parameter and specifying the <b>startValue</b> and "
         "<b>deltaValue</b> parameters.</p>";
}

CQChartsPlot *
CQChartsDistributionPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsDistributionPlot(view, model);
}

//------

CQChartsDistributionPlot::
CQChartsDistributionPlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("distribution"), model), dataLabel_(this)
{
  (void) addValueSet("values");
  (void) addColorSet("color");

  setBarFill (true);
  setBarColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  //---

  setGroupColumn(0);

  setExactValue(false);

  //---

  setBorder(true);

  setLayerActive(Layer::FG, true);

  addAxes();

  addKey();

  addTitle();
}

CQChartsDistributionPlot::
~CQChartsDistributionPlot()
{
}

//---

void
CQChartsDistributionPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "colorColumn", "color");

  addProperty("grouping"       , this, "groupColumn" , "group"    );
//addProperty("grouping"       , this, "rowGrouping" , "groupRows");
//addProperty("grouping"       , this, "usePath"     , "path"     );
//addProperty("grouping"       , this, "useRow"      , "row"      );
  addProperty("grouping/bucket", this, "exactValue"  , "exact"    );
  addProperty("grouping/bucket", this, "autoRange"   , "auto"     );
  addProperty("grouping/bucket", this, "startValue"  , "start"    );
  addProperty("grouping/bucket", this, "deltaValue"  , "delta"    );
  addProperty("grouping/bucket", this, "numAuto"     , "numAuto"  );

  addProperty("options", this, "horizontal");
  addProperty("options", this, "margin"    , "barMargin");

  addProperty("stroke", this, "border"     , "visible"   );
  addProperty("stroke", this, "borderColor", "color"     );
  addProperty("stroke", this, "borderAlpha", "alpha"     );
  addProperty("stroke", this, "borderWidth", "width"     );
  addProperty("stroke", this, "borderDash" , "dash"      );
  addProperty("stroke", this, "cornerSize" , "cornerSize");

  addProperty("fill", this, "barFill"   , "visible");
  addProperty("fill", this, "barColor"  , "color"  );
  addProperty("fill", this, "barAlpha"  , "alpha"  );
  addProperty("fill", this, "barPattern", "pattern");

  dataLabel_.addProperties("dataLabel");

  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
}

//---

void
CQChartsDistributionPlot::
setHorizontal(bool b)
{
  if (b != horizontal_) {
    horizontal_ = b;

    dataLabel_.setDirection(horizontal_ ?
      CQChartsDataLabel::Direction::HORIZONTAL : CQChartsDataLabel::Direction::VERTICAL);

    updateRangeAndObjs();
  }
}

//---

void
CQChartsDistributionPlot::
setMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { update(); } );
}

//---

bool
CQChartsDistributionPlot::
isBorder() const
{
  return boxData_.shape.border.visible;
}

void
CQChartsDistributionPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsDistributionPlot::
borderColor() const
{
  return boxData_.shape.border.color;
}

void
CQChartsDistributionPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.color, c, [&]() { update(); } );
}

QColor
CQChartsDistributionPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsDistributionPlot::
borderAlpha() const
{
  return boxData_.shape.border.alpha;
}

void
CQChartsDistributionPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsDistributionPlot::
borderWidth() const
{
  return boxData_.shape.border.width;
}

void
CQChartsDistributionPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.width, l, [&]() { update(); } );
}

const CQChartsLineDash &
CQChartsDistributionPlot::
borderDash() const
{
  return boxData_.shape.border.dash;
}

void
CQChartsDistributionPlot::
setBorderDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.dash, d, [&]() { update(); } );
}

const CQChartsLength &
CQChartsDistributionPlot::
cornerSize() const
{
  return boxData_.cornerSize;
}

void
CQChartsDistributionPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(boxData_.cornerSize, s, [&]() { update(); } );
}

//---

bool
CQChartsDistributionPlot::
isBarFill() const
{
  return boxData_.shape.background.visible;
}

void
CQChartsDistributionPlot::
setBarFill(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsDistributionPlot::
barColor() const
{
  return boxData_.shape.background.color;
}

void
CQChartsDistributionPlot::
setBarColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.color, c, [&]() { update(); } );
}

QColor
CQChartsDistributionPlot::
interpBarColor(int i, int n) const
{
  return barColor().interpColor(this, i, n);
}

double
CQChartsDistributionPlot::
barAlpha() const
{
  return boxData_.shape.background.alpha;
}

void
CQChartsDistributionPlot::
setBarAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.alpha, a, [&]() { update(); } );
}

CQChartsDistributionPlot::Pattern
CQChartsDistributionPlot::
barPattern() const
{
  return (Pattern) boxData_.shape.background.pattern;
}

void
CQChartsDistributionPlot::
setBarPattern(Pattern pattern)
{
  if (pattern != (Pattern) boxData_.shape.background.pattern) {
    boxData_.shape.background.pattern = (CQChartsFillPattern::Type) pattern;

    update();
  }
}

//---

void
CQChartsDistributionPlot::
updateRange(bool apply)
{
  dataRange_.reset();

  //---

  // calc category type
  initValueSets();

  //---

  CQChartsValueSet *valueSet = getValueSet("values");

  // calc value range
  bool hasRange = valueSet->isNumeric();

  if (hasRange) {
    class ColumnDetails : public CQChartsModelColumnDetails {
     public:
      ColumnDetails(CQChartsDistributionPlot *plot, QAbstractItemModel *model,
                    const CQChartsColumn &column) :
       CQChartsModelColumnDetails(plot->charts(), model, column), plot_(plot) {
      }

      bool checkRow(const QVariant &var) override {
        return plot_->checkFilter(var.toReal());
      }

     private:
      CQChartsDistributionPlot *plot_ { nullptr };
    };

    //---

    bool isIntegral = (valueSet->type() == CQChartsValueSet::Type::INTEGER);

    QAbstractItemModel *model = this->model().data();

    if (model) {
      ColumnDetails columnDetails(this, model, groupColumn());

      groupData_.bucketer.setIntegral(isIntegral);
      groupData_.bucketer.setRMin(columnDetails.minValue().toReal());
      groupData_.bucketer.setRMax(columnDetails.maxValue().toReal());
    }
  }

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsDistributionPlot *plot, CQChartsValueSet *valueSet) :
     plot_(plot), valueSet_(valueSet) {
      hasRange_ = valueSet->isNumeric();
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      QModelIndex valueInd  = plot_->modelIndex(row, plot_->groupColumn(), parent);
      QModelIndex valueInd1 = plot_->normalizeIndex(valueInd);

      //---

      int bucket = -1;

      if (hasRange_) {
        bool ok;

        double value = plot_->modelReal(row, plot_->groupColumn(), parent, ok);

        if (! ok)
          return State::SKIP;

        if (CQChartsUtil::isNaN(value))
          return State::SKIP;

        if (! plot_->checkFilter(value))
          return State::SKIP;

        //----

        if (plot_->isExactValue()) {
          if      (valueSet_->isReal())
            bucket = valueSet_->rid(value);
          else if (valueSet_->isInteger())
            bucket = valueSet_->iid(value);
        }
        else
          bucket = plot_->calcBucket(value);

        ivalues_[bucket].emplace_back(valueInd1);
      }
      else {
        bool ok;

        QString value = plot_->modelString(row, plot_->groupColumn(), parent, ok);

        if (! ok)
          return State::SKIP;

        if      (plot_->isExactValue())
          bucket = valueSet_->sind(value);
        else if (plot_->isAutoRange())
          bucket = valueSet_->sbucket(value);
        else
          bucket = valueSet_->sind(value);

        ivalues_[bucket].emplace_back(valueInd1);
      }

      //---

      if (bucket >= 0) {
        if (minBucket_ <= maxBucket_) {
          minBucket_ = std::min(minBucket_, bucket);
          maxBucket_ = std::max(maxBucket_, bucket);
        }
        else {
          minBucket_ = bucket;
          maxBucket_ = bucket;
        }
      }

      return State::OK;
    }

    const IValues &ivalues() const { return ivalues_; }

    int minBucket() const { return minBucket_; }
    int maxBucket() const { return maxBucket_; }

   private:
    CQChartsDistributionPlot *plot_      { nullptr };
    CQChartsValueSet         *valueSet_  { nullptr };
    bool                      hasRange_  { false };
    IValues                   ivalues_;
    int                       minBucket_ {  1 };
    int                       maxBucket_ { -1 };
  };

  RowVisitor visitor(this, valueSet);

  visitModel(visitor);

  ivalues_ = visitor.ivalues();

  int minBucket = visitor.minBucket();
  int maxBucket = visitor.maxBucket();

  //---

  int maxValues = 0;

  for (const auto &ivalue : ivalues_)
    maxValues = std::max(maxValues, int(ivalue.second.size()));

  if (maxValues == 0)
    maxValues = 1;

  if (maxBucket < minBucket) // unset
    maxBucket = minBucket;

  //---

  if (! isHorizontal()) {
    dataRange_.updateRange(minBucket - 1.0, 0        );
    dataRange_.updateRange(maxBucket + 1.0, maxValues);
  }
  else {
    dataRange_.updateRange(0        , minBucket - 1.0);
    dataRange_.updateRange(maxValues, maxBucket + 1.0);
  }

  //---

  if (apply)
    applyDataRange();
}

//------

CQChartsGeom::BBox
CQChartsDistributionPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  CQChartsDataLabel::Position position = dataLabel().position();

  if (position != CQChartsDataLabel::TOP_OUTSIDE && position != CQChartsDataLabel::BOTTOM_OUTSIDE)
    return bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsDistributionBarObj *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

    if (barObj)
      bbox += barObj->dataLabelRect();
  }

  return bbox;
}

//------

bool
CQChartsDistributionPlot::
checkFilter(double value) const
{
  if (filterStack_.empty())
    return true;

  const Filters &filters = filterStack_.back();

  for (const auto &filter : filters) {
    if (value >= filter.minValue && value < filter.maxValue)
      return true;
  }

  return false;
}

int
CQChartsDistributionPlot::
calcBucket(double value) const
{
  if (filterStack_.empty()) {
    if (isAutoRange())
      return groupData_.bucketer.autoRealBucket(value);
    else
      return groupData_.bucketer.realBucket(value);
  }
  else {
    return groupData_.bucketer.autoRealBucket(value);
  }
}

//------

void
CQChartsDistributionPlot::
updateObjs()
{
  clearValueSets();

  CQChartsPlot::updateObjs();
}

bool
CQChartsDistributionPlot::
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

  // init color value set
  initValueSets();

  //---

  valueAxis()->clearTickLabels();
  countAxis()->clearTickLabels();

  valueAxis()->setIntegral        (true);
  valueAxis()->setGridMid         (true);
  valueAxis()->setMajorIncrement  (1);
  valueAxis()->setRequireTickLabel(true );

  countAxis()->setIntegral        (true);
  valueAxis()->setGridMid         (false);
  countAxis()->setMajorIncrement  (0);
  countAxis()->setRequireTickLabel(false);

  //---

  CQChartsValueSet *valueSet = getValueSet("values");

  bool hasRange = valueSet->isNumeric();

  int i = 0;
  int n = numValues();

  for (const auto &ivalue : ivalues_) {
    int           bucket = ivalue.first;
    const Values &values = ivalue.second;

    CQChartsGeom::BBox bbox;

    if (! isHorizontal())
      bbox = CQChartsGeom::BBox(bucket - 0.5, 0, bucket + 0.5, values.size());
    else
      bbox = CQChartsGeom::BBox(0, bucket - 0.5, values.size(), bucket + 0.5);

    CQChartsDistributionBarObj *barObj =
      new CQChartsDistributionBarObj(this, bbox, bucket, values, i, n);

    addPlotObject(barObj);

    //---

    if (hasRange) {
      if (valueAxis()->tickLabelPlacement() == CQChartsAxis::TickLabelPlacement::MIDDLE) {
        QString bucketStr = bucketValuesStr(bucket, BucketValueType::ALL);

        valueAxis()->setTickLabel(bucket, bucketStr);
      }
      else {
        QString bucketStr1 = bucketValuesStr(bucket, BucketValueType::START);
        QString bucketStr2 = bucketValuesStr(bucket, BucketValueType::END  );

        valueAxis()->setTickLabel(bucket    , bucketStr1);
        valueAxis()->setTickLabel(bucket + 1, bucketStr2);
      }
    }
    else {
      QString bucketStr = bucketValuesStr(bucket, BucketValueType::START);

      valueAxis()->setTickLabel(bucket, bucketStr);
    }

    //---

    ++i;
  }

  //---

  bool ok;

  QString valueName = modelHeaderString(groupColumn(), ok);

  valueAxis()->setLabel(valueName);
  countAxis()->setLabel("Count");

  //---

  resetKeyItems();

  //---

  return true;
}

CQChartsAxis *
CQChartsDistributionPlot::
valueAxis() const
{
  return (! isHorizontal() ? xAxis_ : yAxis_);
}

CQChartsAxis *
CQChartsDistributionPlot::
countAxis() const
{
  return (! isHorizontal() ? yAxis_ : xAxis_);
}

void
CQChartsDistributionPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int row = key->maxRow();

  int n = numValues();

  for (const auto &ivalue : ivalues_) {
    int bucket = ivalue.first;

    QString bucketStr = bucketValuesStr(bucket);

    CQChartsDistKeyColorBox *color = new CQChartsDistKeyColorBox(this, bucket, n);
    CQChartsKeyText         *text  = new CQChartsKeyText        (this, bucketStr);

    key->addItem(color, row, 0);
    key->addItem(text , row, 1);

    ++row;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int bucket, BucketValueType type) const
{
  CQChartsValueSet *valueSet = getValueSet("values");

  //---

  bool hasRange = valueSet->isNumeric();

  if (hasRange) {
    if (isExactValue()) {
      if      (valueSet->isReal()) {
        double r = valueSet->idr(bucket);

        return QString("%1").arg(r);
      }
      else if (valueSet->isInteger()) {
        int i = valueSet->idi(bucket);

        return QString("%1").arg(i);
      }
    }

    //--

    double value1, value2;

    bucketValues(bucket, value1, value2);

    if      (type == BucketValueType::ALL)
      return groupData_.bucketer.bucketName(value1, value2, /*arrow*/true);
    else if (type == BucketValueType::START)
      return QString("%1").arg(value1);
    else
      return QString("%1").arg(value2);
  }
  else {
    if      (isExactValue())
      return valueSet->inds(bucket);
    else if (isAutoRange())
      return valueSet->buckets(bucket);
    else
      return valueSet->inds(bucket);
  }
}

void
CQChartsDistributionPlot::
bucketValues(int bucket, double &value1, double &value2) const
{
  bool isAuto = (! filterStack_.empty() || isAutoRange());

  if (isAuto)
    groupData_.bucketer.autoBucketValues(bucket, value1, value2);
  else
    groupData_.bucketer.bucketRValues(bucket, value1, value2);

  if (CQChartsUtil::isZero(value1)) value1 = 0.0;
  if (CQChartsUtil::isZero(value2)) value2 = 0.0;
}

//---

bool
CQChartsDistributionPlot::
probe(ProbeData &probeData) const
{
  if (! dataRange_.isSet())
    return false;

  if (! isHorizontal()) {
    probeData.direction = ProbeData::Direction::VERTICAL;

    if (probeData.x < dataRange_.xmin() + 0.5)
      probeData.x = dataRange_.xmin() + 0.5;

    if (probeData.x > dataRange_.xmax() - 0.5)
      probeData.x = dataRange_.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    probeData.direction = ProbeData::Direction::HORIZONTAL;

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
CQChartsDistributionPlot::
addMenuItems(QMenu *menu)
{
  QAction *horizontalAction = new QAction("Horizontal", menu);

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));

  //---

  PlotObjs objs;

  selectedObjs(objs);

  QAction *pushAction   = new QAction("Push"   , menu);
  QAction *popAction    = new QAction("Pop"    , menu);
  QAction *popTopAction = new QAction("Pop Top", menu);

  connect(pushAction  , SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction   , SIGNAL(triggered()), this, SLOT(popSlot()));
  connect(popTopAction, SIGNAL(triggered()), this, SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(! filterStack_.empty());
  popTopAction->setEnabled(! filterStack_.empty());

  //---

  menu->addSeparator();

  menu->addAction(horizontalAction);
  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  return true;
}

//---

void
CQChartsDistributionPlot::
pushSlot()
{
  PlotObjs objs;

  selectedObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w;

    pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

    objsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  Filters filters;

  for (const auto &obj : objs) {
    CQChartsDistributionBarObj *distObj = qobject_cast<CQChartsDistributionBarObj *>(obj);

    double value1, value2;

    bucketValues(distObj->bucket(), value1, value2);

    filters.emplace_back(value1, value2);
  }

  filterStack_.push_back(filters);

  updateRangeAndObjs();
}

void
CQChartsDistributionPlot::
popSlot()
{
  if (! filterStack_.empty()) {
    filterStack_.pop_back();

    updateRangeAndObjs();
  }
}

void
CQChartsDistributionPlot::
popTopSlot()
{
  if (! filterStack_.empty()) {
    filterStack_.clear();

    updateRangeAndObjs();
  }
}

//------

void
CQChartsDistributionPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

double
CQChartsDistributionPlot::
getPanX(bool is_shift) const
{
  return windowToViewWidth(is_shift ? 2.0 : 1.0);
}

double
CQChartsDistributionPlot::
getPanY(bool is_shift) const
{
  return windowToViewHeight(is_shift ? 2.0 : 1.0);
}

//------

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                           int bucket, const Values &values, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), bucket_(bucket), values_(values), i_(i), n_(n)
{
  assert(i >= 0 && i < n);
}

QString
CQChartsDistributionBarObj::
calcId() const
{
  QString bucketStr = plot_->bucketValuesStr(bucket_);

  return QString("%1 : %2").arg(bucketStr).arg(values_.size());
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
dataLabelRect() const
{
  if (! plot_->dataLabel().isVisible())
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox rect = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(rect);

  QString ystr = QString("%1").arg(values_.size());

  return plot_->dataLabel().calcRect(qrect, ystr);
}

void
CQChartsDistributionBarObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->groupColumn());
}

void
CQChartsDistributionBarObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    for (const auto &value : values_) {
      addSelectIndex(inds, value.row(), column, value.parent());
    }
  }
}

void
CQChartsDistributionBarObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  painter->save();

  //---

  CQChartsGeom::BBox pbbox = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(pbbox);

  double s = (! plot_->isHorizontal() ? qrect.width() : qrect.height());

  bool useLine = (s <= 2);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    // get bar colors
    CQChartsColor barColor = plot_->interpBarColor(i_, n_);

    using ColorCount = std::map<int,int>;
    using ColorSet   = std::map<CQChartsColor,int>;

    ColorCount colorCount;
    ColorSet   colorSet;
    int        nv = 0;

    if (plot_->colorColumn().isValid()) {
      for (const auto &value : values_) {
        QModelIndex colorInd = plot_->unnormalizeIndex(value);

        OptColor optColor;

        if (! plot_->colorSetColor("color", colorInd.row(), optColor))
          optColor = barColor;

        CQChartsColor color = *optColor;

        auto p = colorSet.find(color);

        if (p == colorSet.end()) {
          int ind = colorSet.size();

          p = colorSet.insert(p, ColorSet::value_type(color, ind));
        }

        ++colorCount[(*p).second];

        ++nv;
      }
    }

    if (colorSet.empty()) {
      colorSet[barColor] = 0;

      colorCount[0] = 1;

      nv = 1;
    }

    //---

    double size = (! plot_->isHorizontal() ? qrect.height() : qrect.width());

    double dsize = size/nv;

    double pos1 = 0.0, pos2 = 0.0;

    for (auto &p : colorSet) {
      const CQChartsColor &color = p.first;
      int                  n     = colorCount[p.second];

      pos1 = pos2;
      pos2 = pos1 + dsize*n;

      QRectF qrect1;

      if (! plot_->isHorizontal())
        qrect1 = QRectF(qrect.x(), qrect.bottom() - pos2, qrect.width(), pos2 - pos1);
      else
        qrect1 = QRectF(qrect.left() + pos1, qrect.y(), pos2 - pos1, qrect.height());

      //---

      // calc pen (stroke)
      QPen pen;

      if (plot_->isBorder()) {
        QColor bc = plot_->interpBorderColor(0, 1);

        bc.setAlphaF(plot_->borderAlpha());

        double bw = plot_->lengthPixelWidth(plot_->borderWidth());

        pen.setColor (bc);
        pen.setWidthF(bw);

        CQChartsUtil::penSetLineDash(pen, plot_->borderDash());
      }
      else {
        pen.setStyle(Qt::NoPen);
      }

      // calc brush (fill)
      QBrush barBrush;

      if (plot_->isBarFill()) {
        QColor barColor = color.interpColor(plot_, 0, 1);

        barColor.setAlphaF(plot_->barAlpha());

        barBrush.setColor(barColor);
        barBrush.setStyle(CQChartsFillPattern::toStyle(
         (CQChartsFillPattern::Type) plot_->barPattern()));

        if (useLine) {
          pen.setColor (barColor);
          pen.setWidthF(0);
        }
      }
      else {
        barBrush.setStyle(Qt::NoBrush);

        if (useLine)
          pen.setWidthF(0);
      }

      plot_->updateObjPenBrushState(this, pen, barBrush);

      //---

      // draw rect
      painter->setPen(pen);
      painter->setBrush(barBrush);

      if (! useLine) {
        double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
        double cys = plot_->lengthPixelHeight(plot_->cornerSize());

        CQChartsRoundedPolygon::draw(painter, qrect1, cxs, cys);
      }
      else {
        if (! plot_->isHorizontal()) {
          double xc = qrect1.center().x();

          painter->drawLine(xc, qrect1.bottom(), xc, qrect1.top());
        }
        else {
          double yc = qrect1.center().y();

          painter->drawLine(qrect1.left(), yc, qrect1.right(), yc);
        }
      }
    }
  }
  else {
    QString ystr = QString("%1").arg(values_.size());

    plot_->dataLabel().draw(painter, qrect, ystr);
  }

  painter->restore();
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
calcRect() const
{
  CQChartsGeom::BBox bbox;

  double m = plot_->lengthPixelSize(plot_->margin(), ! plot_->isHorizontal());

  if (2*m > 1)
    m = m/2;

  if (! plot_->isHorizontal())
    bbox = CQChartsGeom::BBox(bucket_ - 0.5 + m, 0, bucket_ + 0.5 - m, values_.size());
  else
    bbox = CQChartsGeom::BBox(0, bucket_ - 0.5 + m, values_.size(), bucket_ + 0.5 - m);

  CQChartsGeom::BBox pbbox;

  plot_->windowToPixel(bbox, pbbox);

  return pbbox;
}

//------

CQChartsDistKeyColorBox::
CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

QBrush
CQChartsDistKeyColorBox::
fillBrush() const
{
  CQChartsDistributionPlot *plot = qobject_cast<CQChartsDistributionPlot *>(plot_);

  QColor barColor = plot->interpBarColor(i_, n_);

  CQChartsDistributionPlot::OptColor color;

  const CQChartsDistributionPlot::Values &values = plot->ivalues(i_);

  if (! values.empty()) {
    QModelIndex colorInd = plot->unnormalizeIndex(values[0]);

    (void) plot->colorSetColor("color", colorInd.row(), color);

    if (color)
      barColor = color->interpColor(plot, 0, 1);
  }

  return barColor;
}
