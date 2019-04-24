#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <QPainter>
#include <QMenu>

CQChartsPiePlotType::
CQChartsPiePlotType()
{
}

void
CQChartsPiePlotType::
addParameters()
{
  startParameterGroup("Pie");

  // name, desc, propName, attributes, default
  addColumnsParameter("value", "Value", "valueColumns").
    setRequired().setNumeric().setTip("Value column(s)");

  addColumnParameter("label", "Label", "labelColumn").
    setString().setTip("Custom label");

  addColumnParameter("radius", "Radius", "radiusColumn").
    setNumeric().setTip("Custom radius");

  addColumnParameter("keyLabel", "Key Label", "keyLabelColumn").
    setString().setTip("Custom key label");

  addBoolParameter("donut", "Donut", "donut").setTip("Draw donut");
  addBoolParameter("count", "Count", "count").setTip("Display value counts");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsPiePlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draw circle segments with diameter from a set of values.</p>\n"
         "<p>The segments can be restricted to an inner radius and a label"
         "can be displated at the center of the circle.\n";
}

CQChartsPlot *
CQChartsPiePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsPiePlot(view, model);
}

//------

CQChartsPiePlot::
CQChartsPiePlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("pie"), model),
 CQChartsObjShapeData   <CQChartsPiePlot>(this),
 CQChartsObjGridLineData<CQChartsPiePlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setGridLines(false);
  setGridLinesColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.5));

  //---

  textBox_ = new CQChartsPieTextObj(this);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addKey();

  addTitle();
}

CQChartsPiePlot::
~CQChartsPiePlot()
{
  delete textBox_;
}

//---

void
CQChartsPiePlot::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setValueColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setRadiusColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(radiusColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setKeyLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(keyLabelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setDonut(bool b)
{
  CQChartsUtil::testAndSet(donut_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setCount(bool b)
{
  CQChartsUtil::testAndSet(count_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setOuterRadius(double r)
{
  CQChartsUtil::testAndSet(outerRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setStartAngle(double r)
{
  CQChartsUtil::testAndSet(startAngle_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setAngleExtent(double r)
{
  CQChartsUtil::testAndSet(angleExtent_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setGapAngle(double r)
{
  CQChartsUtil::testAndSet(gapAngle_, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { drawObjs(); } );
}

void
CQChartsPiePlot::
setExplodeSelected(bool b)
{
  CQChartsUtil::testAndSet(explodeSelected_, b, [&]() { drawObjs(); } );
}

void
CQChartsPiePlot::
setExplodeRadius(double r)
{
  CQChartsUtil::testAndSet(explodeRadius_, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "valueColumns"  , "values"  )->setDesc("Value columns");
  addProperty("columns", this, "labelColumn"   , "label"   )->setDesc("Label column");
  addProperty("columns", this, "radiusColumn"  , "radius"  )->setDesc("Radius column");
  addProperty("columns", this, "keyLabelColumn", "keyLabel")->setDesc("Key label column");

  CQChartsGroupPlot::addProperties();

  // options
  addProperty("options", this, "donut"      )->setDesc("Display as donut using inner radius");
  addProperty("options", this, "count"      )->setDesc("Show count of groups");
  addProperty("options", this, "innerRadius")->setDesc("Inner radius");
  addProperty("options", this, "startAngle" )->setDesc("Start angle for first segment");
  addProperty("options", this, "angleExtent")->setDesc("Angle extent for pie segments");
  addProperty("options", this, "gapAngle"   )->setDesc("Gap angle");

  // fill
  addProperty("fill", this, "filled", "visible")->setDesc("Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProperty("stroke", this, "border", "visible")->setDesc("Stroke visible");

  addLineProperties("stroke", "border", "");

  // grid
  addProperty("grid", this, "gridLines", "visible")->setDesc("Grid lines visible");

  addLineProperties("grid", "gridLines", "Grid");

  // explode
  addProperty("explode", this, "explodeSelected", "selected")->setDesc("Explode selected segments");
  addProperty("explode", this, "explodeRadius"  , "radius"  )->setDesc("Explode radius");

  // label
  addProperty("label", textBox_, "textVisible", "visible")->setDesc("Text visible");

  textBox_->addTextDataProperties(propertyModel(), "label");

  addProperty("label", this, "labelRadius", "radius" )->setDesc("Label radius");
  addProperty("label", this, "rotatedText", "rotated")->setDesc("Text rotated to segment angle");

  QString labelBoxPath = "label/box";

  textBox_->CQChartsBoxObj::addProperties(propertyModel(), labelBoxPath);

  // color map
  addColorMapProperties();
}

//---

CQChartsGeom::Range
CQChartsPiePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsPiePlot::calcRange");

  CQChartsGeom::Range dataRange;

  CQChartsGeom::Point c(0.0, 0.0);

  dataRange.updateRange(c);

  //---

  double r = std::max(1.0, labelRadius());

  double angle1 = startAngle();
  double alen   = CMathUtil::clamp(angleExtent(), -360.0, 360.0);
  double angle2 = angle1 - alen;

  // add segment outside points
  dataRange.updateRange(CQChartsUtil::AngleToPoint(c, r, angle1));
  dataRange.updateRange(CQChartsUtil::AngleToPoint(c, r, angle2));

  // add intermediate points (every 90 degree point between outside points)
  double a1 = 90.0*CMathRound::RoundDownF(angle1/90.0);

  if (angle1 < angle2) {
    for (double a = a1; a < angle2; a += 90.0) {
      if (a > angle1 && a < angle2)
        dataRange.updateRange(CQChartsUtil::AngleToPoint(c, r, a));
    }
  }
  else {
    for (double a = a1; a > angle2; a -= 90.0) {
      if (a > angle2 && a < angle1)
        dataRange.updateRange(CQChartsUtil::AngleToPoint(c, r, a));
    }
  }

  //---

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  //---

  // init grouping
  initGroupData(valueColumns(), labelColumn());

  //---

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsPiePlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsPieObj *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

    if (! pieObj || ! pieObj->visible())
      continue;

    bbox += pieObj->annotationBBox();
  }

  return bbox;
}

//------

bool
CQChartsPiePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsPiePlot::createObjs");

  CQChartsPiePlot *th = const_cast<CQChartsPiePlot *>(this);

  NoUpdate noUpdate(th);

  //---

  // calc group totals
  calcDataTotal();

  //---

  th->groupObjs_.clear();

  //---

  if (! valueColumns().isValid())
    return false;

  double ro = outerRadius();
  double ri = (isDonut() ? innerRadius()*outerRadius() : 0.0);

  int ig = 0;
  int ng = numGroups();

  double dr = (ng > 0 ? (ro - ri)/ng : 0.0);

  double r = ro;

  std::vector<int> groupInds;

  this->getGroupInds(groupInds);

  for (const auto &groupInd : groupInds) {
    auto pg = groupDatas_.find(groupInd);

    if (pg == groupDatas_.end()) {
      pg = groupDatas_.find(-1); // no group ind is (-1)

      assert(pg != groupDatas_.end());
    }

    const GroupData &groupData = (*pg).second;

    //---

    // create group obj
    CQChartsGeom::BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    CQChartsPieGroupObj *groupObj =
      new CQChartsPieGroupObj(this, rect, groupInd, groupData.name, ig, ng);

    groupObj->setColorInd(groupInd);

    groupObj->setDataTotal(groupData.dataTotal);
    groupObj->setNumValues(groupData.numValues);

    groupObj->setRadiusMax   (groupData.radiusMax);
    groupObj->setRadiusScaled(groupData.radiusScaled);

    if (! isCount()) {
      groupObj->setInnerRadius(r - dr);
      groupObj->setOuterRadius(r);
    }
    else {
      groupObj->setInnerRadius(ri);
      groupObj->setOuterRadius(ro);
    }

    groupObj->setVisible(isCount());

    objs.push_back(groupObj);

    th->groupObjs_.push_back(groupObj);

    //---

    if (groupData.groupObj != groupObj) {
      GroupData &groupData1 = const_cast<GroupData &>(groupData);

      groupData1.groupObj = groupObj;
    }

    //---

    r -= dr;

    ++ig;
  }

  //---

  // add individual values (not needed for count)

  if (! isCount()) {
    // init value sets
    //initValueSets();

    //---

    // process model data
    class PieVisitor : public ModelVisitor {
     public:
      PieVisitor(const CQChartsPiePlot *plot, PlotObjs &objs) :
       plot_(plot), objs_(objs) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        plot_->addRow(data, objs_);

        return State::OK;
      }

     private:
      const CQChartsPiePlot *plot_ { nullptr };
      PlotObjs&              objs_;
    };

    PieVisitor pieVisitor(this, objs);

    visitModel(pieVisitor);
  }

  //---

  adjustObjAngles();

  //---

  return true;
}

void
CQChartsPiePlot::
addRow(const ModelVisitor::VisitData &data, PlotObjs &objs) const
{
  for (const auto &column : valueColumns()) {
    CQChartsModelIndex ind(data.row, column, data.parent);

    addRowColumn(ind, objs);
  }
}

void
CQChartsPiePlot::
addRowColumn(const CQChartsModelIndex &ind, PlotObjs &objs) const
{
  assert(! isCount());

  // get group ind
  int groupInd = rowGroupInd(ind);

  //---

  // hide all objects of group or individual objects of single group
  bool hidden = false;

  if (numGroups() > 1)
    hidden = isSetHidden(groupInd);
  else
    hidden = isSetHidden(ind.row);

  //---

  // get column value
  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(ind, value, valueMissing))
    return;

  //---

  // get column radius
  double radius        = 0.0;
  bool   radiusMissing = false;

  bool hasRadius = false;

  if (radiusColumn().isValid()) {
    CQChartsModelIndex rind(ind.row, radiusColumn(), ind.parent);

    hasRadius = getColumnSizeValue(rind, radius, radiusMissing);
  }

  //---

  // get value label (used for unique values in group)
  CQChartsModelIndex lind(ind.row, labelColumn(), ind.parent);

  bool ok;

  QString label;

  if (numGroups() > 1) {
    if (valueColumns().count() > 1 && ! isGroupHeaders())
      label = modelHeaderString(ind.column, ok);
    else
      label = modelString(lind, ok);
  }
  else {
    label = modelString(lind, ok);
  }

  if (! label.length())
    label = QString("%1").arg(ind.row);

  //---

  // get key label
  QString keyLabel = label;

  if (keyLabelColumn().isValid()) {
    CQChartsModelIndex kind(ind.row, keyLabelColumn(), ind.parent);

    bool ok;

    keyLabel = modelString(kind, ok);
  }

  //---

  // get group obj
  auto pg = groupDatas_.find(groupInd);
  assert(pg != groupDatas_.end());

  const GroupData &groupData = (*pg).second;

  CQChartsPieGroupObj *groupObj = groupData.groupObj;

  //---

  // set radii
  double ri = (groupObj ? groupObj->innerRadius() : innerRadius());
  double ro = (groupObj ? groupObj->outerRadius() : outerRadius());
  double rv = ro;

  if (hasRadius && groupObj && groupObj->isRadiusScaled()) {
    double dr = ro - ri;
    double s  = (groupObj->radiusMax() > 0.0 ? radius/groupObj->radiusMax() : 1.0);

    rv = ri + s*dr;
  }

  //---

  QModelIndex dataInd  = modelIndex(ind);
  QModelIndex dataInd1 = normalizeIndex(dataInd);

  //---

  // get pie object (by label)
  CQChartsPieObj *obj = (groupObj ? groupObj->lookupObj(label) : nullptr);

  if (! obj) {
    CQChartsGeom::BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    int objInd = (groupObj ? groupObj->numObjs() : 0);

    obj = new CQChartsPieObj(this, rect, dataInd1);

    if (hidden)
      obj->setVisible(false);

    obj->setColorInd(objInd);

    obj->setInnerRadius(ri);
    obj->setOuterRadius(ro);
    obj->setValueRadius(rv);

    obj->setLabel  (label);
    obj->setValue  (value);
    obj->setMissing(valueMissing);

    if (hasRadius)
      obj->setRadius(radius);

    obj->setKeyLabel(keyLabel);

    //---

    CQChartsColor color;

    if (columnColor(ind.row, ind.parent, color))
      obj->setColor(color);

    //---

    objs.push_back(obj);

    if (groupObj)
      groupObj->addObject(obj);
  }
  else {
    // duplicate labels add to value (and radius ?)

    if (! valueMissing)
      obj->setValue(obj->value() + value);

    if (hasRadius)
      obj->setRadius(*obj->radius() + radius);

    // TODO: add dataInd
  }
}

void
CQChartsPiePlot::
calcDataTotal() const
{
  CQChartsPiePlot *th = const_cast<CQChartsPiePlot *>(this);

  th->groupDatas_.clear();

  // process model data
  class DataTotalVisitor : public ModelVisitor {
   public:
    DataTotalVisitor(const CQChartsPiePlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRowDataTotal(data);

      return State::OK;
    }

   private:
    const CQChartsPiePlot *plot_ { nullptr };
  };

  DataTotalVisitor dataTotalVisitor(this);

  visitModel(dataTotalVisitor);
}

void
CQChartsPiePlot::
addRowDataTotal(const ModelVisitor::VisitData &data) const
{
  for (const auto &column : valueColumns()) {
    CQChartsModelIndex ind(data.row, column, data.parent);

    addRowColumnDataTotal(ind);
  }
}

void
CQChartsPiePlot::
addRowColumnDataTotal(const CQChartsModelIndex &ind) const
{
  CQChartsPiePlot *th = const_cast<CQChartsPiePlot *>(this);

  // get group ind
  int groupInd = rowGroupInd(ind);

  //---

  // hide all objects of group or individual objects of single group
  bool hidden = false;

  if (! isCount()) {
    if (numGroups() > 1)
      hidden = isSetHidden(groupInd);
    else
      hidden = isSetHidden(ind.row);
  }
  else {
    hidden = isSetHidden(groupInd);
  }

  //---

  // get group data for group ind (add if new)
  auto pg = groupDatas_.find(groupInd);

  if (pg == groupDatas_.end()) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto pg1 = th->groupDatas_.find(groupInd);

    if (pg1 == th->groupDatas_.end()) {
      QString groupName = groupIndName(groupInd);

      pg1 = th->groupDatas_.insert(pg1, GroupDatas::value_type(groupInd, GroupData(groupName)));
    }

    pg = groupDatas_.find(groupInd);
  }

  const GroupData &groupData = (*pg).second;

  //---

  // get value
  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(ind, value, valueMissing))
    return;

  //---

  // sum values
  if (! hidden) {
    GroupData &groupData1 = const_cast<GroupData &>(groupData);

    ++groupData1.numValues;

    groupData1.dataTotal += value;
  }

  //---

  // get max radius
  if (radiusColumn().isValid()) {
    CQChartsModelIndex rind(ind.row, radiusColumn(), ind.parent);

    double radius        = 0.0;
    bool   radiusMissing = false;

    if (getColumnSizeValue(rind, radius, radiusMissing)) {
      if (! hidden) {
        GroupData &groupData1 = const_cast<GroupData &>(groupData);

        groupData1.radiusScaled = true;
        groupData1.radiusMax    = std::max(groupData.radiusMax, radius);
      }
    }
  }
}

bool
CQChartsPiePlot::
getColumnSizeValue(const CQChartsModelIndex &ind, double &value, bool &missing) const
{
  missing = false;

  ColumnType columnType = columnValueType(ind.column);

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(ind, ok);

    // allow missing value in numeric column
    if (! ok) {
      missing = true;
      value   = 1.0;

      return true;
    }

    // TODO: check allow nan
    if (CMathUtil::isNaN(value))
      return false;

    // size must be positive
    if (value <= 0.0)
      return false;
  }
  else {
    // try convert model string to real
    bool ok;

    value = modelReal(ind, ok);

    // string non-real -> 1.0
    if (! ok) {
      value = 1.0;
      return true;
    }

    // string bad size -> 1.0 (assume bad conversion)
    if (value <= 0.0) {
      value = 1.0;
      return true;
    }
  }

  return true;
}

void
CQChartsPiePlot::
adjustObjAngles() const
{
  double ro = outerRadius();
  double ri = (isDonut() ? innerRadius()*outerRadius() : 0.0);

  bool isGrouped = (numGroups() > 1);

  // calc delta radius (grouped)
  int    ng = 1, nh = 0;
  double dr = 0.0;

  if (! isCount()) {
    if (isGrouped) {
      ng = 0;

      for (auto &groupObj : groupObjs_) {
        if (! isSetHidden(groupObj->groupInd()))
          ++ng;
        else
          ++nh;
      }
    }

    dr = (ng > 0 ? (ro - ri)/ng : 0.0);
  }

  //---

  // get total values
  int totalValues = 0;

  for (auto &groupObj : groupObjs_) {
    totalValues += groupObj->numValues();
  }

  //---

  // calc angle extents for each group
  double ga1 = startAngle();
  double ga2 = ga1;

  double r = ro;

  for (auto &groupObj : groupObjs_) {
    ga1 = ga2;

    // skip hidden groups
    if (! isCount()) {
      if (isGrouped && nh > 0) {
        if (isSetHidden(groupObj->groupInd()))
          continue;
      }
    }
    else {
      if (isSetHidden(groupObj->groupInd()))
        continue;
    }

    //---

    // set group angles
    double dga = (totalValues > 0 ? 360.0*groupObj->numValues()/totalValues : 0);

    ga2 = ga1 - dga;

    groupObj->setAngles(ga1, ga2);

    //---

    if (! isCount()) {
      // set group object inner/outer radii
      if (isGrouped && nh > 0) {
        groupObj->setInnerRadius(r - dr);
        groupObj->setOuterRadius(r);
      }

      //---

      // set segment angles
      double angle1    = startAngle();
      double alen      = CMathUtil::clamp(angleExtent(), -360.0, 360.0);
      double dataTotal = groupObj->dataTotal();

      for (auto &obj : groupObj->objs()) {
        // skip hidden objects
        if (! obj->isVisible())
          continue;

        //---

        // set angle based on value
        double value = obj->value();

        double angle  = (dataTotal > 0.0 ? alen*value/dataTotal : 0.0);
        double angle2 = angle1 - angle;

        obj->setAngle1(angle1);
        obj->setAngle2(angle2);

        //---

        // set inner/outer radius and value radius
        if (isGrouped && nh > 0) {
          obj->setInnerRadius(r - dr);
          obj->setOuterRadius(r);

          double rv = r;

          if (obj->radius() && groupObj->isRadiusScaled()) {
            double s = (groupObj->radiusMax() > 0.0 ? *obj->radius()/groupObj->radiusMax() : 1.0);

            rv = r + (s - 1)*dr;
          }

          obj->setValueRadius(rv);
        }

        //---

        // move to next start angle
        angle1 = angle2;
      }

      //---

      // move to next radius
      if (isGrouped && nh > 0)
        r -= dr;
    }
  }
}

void
CQChartsPiePlot::
addKeyItems(CQChartsPlotKey *key)
{
  int row = 0;

  auto addKeyRow = [&](CQChartsPlotObj *obj) {
    CQChartsPieKeyColor *keyColor = new CQChartsPieKeyColor(this, obj);
    CQChartsPieKeyText  *keyText  = new CQChartsPieKeyText (this, obj);

    key->addItem(keyColor, row, 0);
    key->addItem(keyText , row, 1);

    ++row;
  };

  //---

  if (! isCount()) {
    int ng = groupObjs_.size();

    if (ng > 1) {
      for (const auto &groupObj : groupObjs_)
        addKeyRow(groupObj);
    }
    else {
      for (auto &plotObj : plotObjs_) {
        CQChartsPieObj *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

        if (pieObj)
          addKeyRow(plotObj);
      }
    }
  }
  else {
    for (const auto &groupObj : groupObjs_)
      addKeyRow(groupObj);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsPiePlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

bool
CQChartsPiePlot::
addMenuItems(QMenu *menu)
{
  QAction *donutAction = new QAction("Donut", menu);

  donutAction->setCheckable(true);
  donutAction->setChecked(isDonut());

  connect(donutAction, SIGNAL(triggered(bool)), this, SLOT(setDonut(bool)));

  //---

  QAction *countAction = new QAction("Count", menu);

  countAction->setCheckable(true);
  countAction->setChecked(isCount());

  connect(countAction, SIGNAL(triggered(bool)), this, SLOT(setCount(bool)));

  //---

  menu->addSeparator();

  menu->addAction(donutAction);
  menu->addAction(countAction);

  return true;
}

//------

CQChartsPieObj::
CQChartsPieObj(const CQChartsPiePlot *plot, const CQChartsGeom::BBox &rect,
               const QModelIndex &ind) :
 CQChartsPlotObj(const_cast<CQChartsPiePlot *>(plot), rect), plot_(plot), ind_(ind)
{
}

QString
CQChartsPieObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(colorInd());
}

QString
CQChartsPieObj::
calcTipId() const
{
  QModelIndex ind = plot_->unnormalizeIndex(ind_);

  // get group name and label
  bool hasGroup = (plot_->numGroups() > 1 && groupObj_);

  QString groupName, label;

  bool ok;

  CQChartsModelIndex lind(ind.row(), plot_->labelColumn(), ind.parent());

  if (hasGroup) {
    CQChartsPieGroupObj *groupObj = this->groupObj();

    groupName = groupObj->name();

    if (plot_->isGroupHeaders())
      label = plot_->modelHeaderString(ind.column(), ok);
    else
      label = plot_->modelString(lind, ok);
  }
  else {
    label = plot_->modelString(lind, ok);
  }

  //---

  // get value string
  int valueColumn = ind_.column();

  QString valueStr = plot_->columnStr(valueColumn, value_);

  //---

  // set tip values
  CQChartsTableTip tableTip;

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  tableTip.addTableRow("Name" , label);
  tableTip.addTableRow("Value", valueStr);

  if (radius()) {
    tableTip.addTableRow("Radius", *radius());
  }

  return tableTip.str();
}

void
CQChartsPieObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

  model->addProperty(path1, this, "colorInd")->setDesc("Color index");
  model->addProperty(path1, this, "angle1")->setDesc("Start angle");
  model->addProperty(path1, this, "angle2")->setDesc("End angle");
  model->addProperty(path1, this, "innerRadius")->setDesc("Inner radius");
  model->addProperty(path1, this, "outerRadius")->setDesc("Outer radius");
  model->addProperty(path1, this, "label")->setDesc("Label");
  model->addProperty(path1, this, "value")->setDesc("Value");
  model->addProperty(path1, this, "missing")->setDesc("Value missing");
//model->addProperty(path1, this, "radius")->setDesc("Radius");
  model->addProperty(path1, this, "keyLabel")->setDesc("Key label");
  model->addProperty(path1, this, "color")->setDesc("Color");
  model->addProperty(path1, this, "exploded")->setDesc("Is exploded");
}

bool
CQChartsPieObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  // calc distance from center (radius)
  CQChartsGeom::Point center(0, 0);

  double r = p.distanceTo(center);

  //---

  // check in radius extent
  double ri = innerRadius();
  double ro = valueRadius();

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(atan2(p.y - center.y, p.x - center.x));
  a = CMathUtil::normalizeAngle(a);

  double a1 = angle1(); a1 = CMathUtil::normalizeAngle(a1);
  double a2 = angle2(); a2 = CMathUtil::normalizeAngle(a2);

  if (a1 < a2) {
    // crosses zero
    if (a >= 0.0 && a <= a1)
      return true;

    if (a <= 360.0 && a >= a2)
      return true;
  }
  else {
    if (a >= a2 && a <= a1)
      return true;
  }

  return false;
}

void
CQChartsPieObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->labelColumn());

  for (const auto &c : plot_->valueColumns())
    addColumnSelectIndex(inds, c);
}

void
CQChartsPieObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

bool
CQChartsPieObj::
calcExploded() const
{
  bool isExploded = this->isExploded();

  if (isSelected() && plot_->isExplodeSelected())
    isExploded = true;

  return isExploded;
}

CQChartsGeom::BBox
CQChartsPieObj::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  if (! plot_->textBox()->isTextVisible())
    return bbox;

  if (! label().length())
    return bbox;

  //---

  CQChartsGeom::Point c = getCenter();

  //---

  // calc angle extent
  double a1 = angle1();
  double a2 = angle2();

  double a21 = a2 - a1;

  // if full circle always draw text at center
  if (CMathUtil::realEq(std::abs(a21), 360.0)) {
    CQChartsGeom::Point pc = plot_->windowToPixel(c);

    //---

    bbox = plot_->textBox()->bbox(CQChartsUtil::toQPoint(pc), label(), 0.0);
  }
  // draw on arc center line
  else {
    // calc label radius
    double ri = innerRadius();
    double ro = outerRadius();
    double lr = plot_->labelRadius();

    double lr1;

    if (! CMathUtil::isZero(ri))
      lr1 = ri + lr*(ro - ri);
    else
      lr1 = lr*ro;

    lr1 = std::max(lr1, 0.01);

    //---

    QPointF center(c.x, c.y);

    double rv = valueRadius();

    //---

    // text angle (mid angle)
    double ta = CMathUtil::avg(a1, a2);

    //---

    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->calcConnectedRadialTextBBox(center, rv, lr1, ta, label(),
                                                    plot_->isRotatedText(), bbox);
    }
    else {
      // calc text position
      double tangle = CMathUtil::Deg2Rad(ta);

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = center.x() + lr1*tc;
      double ty = center.y() + lr1*ts;

      CQChartsGeom::Point pt = plot_->windowToPixel(CQChartsGeom::Point(tx, ty));

      QPointF pt1(pt.x, pt.y);

      // calc text angle
      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      // calc text box
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      bbox = plot_->textBox()->bbox(pt1, label(), angle, align);
    }
  }

  return bbox;
}

void
CQChartsPieObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsPieGroupObj *groupObj = this->groupObj();

  int ng = plot_->numGroupObjs();
  int no = (groupObj ? groupObj->numObjs() : 0);

  //---

  // get pie center, radii and angles
  CQChartsGeom::Point c = getCenter();

  double ri = innerRadius();
  double ro = outerRadius();
  double rv = valueRadius();

  double a1 = angle1();
  double a2 = angle2();

  double ga = plot_->gapAngle()/2.0;

  double aa1 = a1 - ga;
  double aa2 = a2 + ga;

  //---

  // draw grid lines (as pie)
  if (plot_->isGridLines()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor gridColor = plot_->interpGridLinesColor(0, 1);

    plot_->setPen(pen, true, gridColor, plot_->gridLinesAlpha(),
                  plot_->gridLinesWidth(), plot_->gridLinesDash());

    painter->setPen  (pen);
    painter->setBrush(brush);

    CQChartsGeom::Point c(0.0, 0.0);

    plot_->drawPieSlice(painter, c, ri, ro, a1, a2);
  }

  //---

  // calc stroke and brush
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBorderColor(0, 1);

  QColor fc;

  if      (color().isValid())
    fc = plot_->charts()->interpColor(color(), colorInd(), no);
  else if (plot_->fillColor().type() != CQChartsColor::Type::PALETTE)
    fc = plot_->charts()->interpColor(plot_->fillColor(), colorInd(), no);
  else if (groupObj)
    fc = plot_->interpGroupPaletteColor(groupObj->colorInd(), ng, colorInd(), no);

  plot_->setPenBrush(pen, brush,
    plot_->isBorder(), bc, plot_->borderAlpha(), plot_->borderWidth(), plot_->borderDash(),
    plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw pie slice
  plot_->drawPieSlice(painter, c, ri, rv, aa1, aa2);
}

void
CQChartsPieObj::
drawFg(QPainter *painter) const
{
  if (! visible())
    return;

  // draw segment label
  CQChartsGeom::Point c = getCenter();

  drawSegmentLabel(painter, c);
}

CQChartsGeom::Point
CQChartsPieObj::
getCenter() const
{
  CQChartsGeom::Point c(0.0, 0.0);

  //---

  bool isExploded = calcExploded();

  if (! isExploded)
    return c;

  //---

  // get adjusted center (exploded state)
  double rv = valueRadius();

  double a1 = angle1();
  double a2 = angle2();

  double angle = CMathUtil::Deg2Rad(CMathUtil::avg(a1, a2));

  double dx = plot_->explodeRadius()*rv*cos(angle);
  double dy = plot_->explodeRadius()*rv*sin(angle);

  c.x += dx;
  c.y += dy;

  return c;
}

void
CQChartsPieObj::
drawSegmentLabel(QPainter *painter, const CQChartsGeom::Point &c) const
{
  if (! plot_->textBox()->isTextVisible())
    return;

  if (! label().length())
    return;

  //---

  CQChartsPieGroupObj *groupObj = this->groupObj();

  // calc label radius
  double ri = innerRadius();
  double ro = outerRadius();
  double lr = plot_->labelRadius();

  double lr1;

  if (! CMathUtil::isZero(ri))
    lr1 = ri + lr*(ro - ri);
  else
    lr1 = lr*ro;

  lr1 = std::max(lr1, 0.01);

  //---

  // calc text angle
  double a1 = angle1();
  double a2 = angle2();

  double ta = CMathUtil::avg(a1, a2);

  //---

  int ng = plot_->numGroupObjs();
  int no = (groupObj ? groupObj->numObjs() : 0);

  // calc label pen
  // TODO: label alpha
  QPen lpen;

  QColor bg;

  if (groupObj)
    bg = plot_->interpGroupPaletteColor(groupObj->colorInd(), ng, colorInd(), no);

  plot_->setPen(lpen, true, bg, 1.0);

  //---

  // calc angle extent
  double a21 = a2 - a1;

  // if full circle always draw text at center
  if (CMathUtil::realEq(std::abs(a21), 360.0)) {
    CQChartsGeom::Point pc = plot_->windowToPixel(c);

    //---

    plot_->textBox()->draw(painter, CQChartsUtil::toQPoint(pc), label(), 0.0);
  }
  // draw on arc center line
  else {
    double rv = valueRadius();

    QPointF center(c.x, c.y);

    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->drawConnectedRadialText(painter, center, rv, lr1, ta, label(),
                                                lpen, plot_->isRotatedText());
    }
    else {
    //plot_->textBox()->drawConnectedRadialText(painter, center, rv, lr1, ta, label(),
    //                                          lpen, plot_->isRotatedText());

      // calc text position
      double tangle = CMathUtil::Deg2Rad(ta);

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = center.x() + lr1*tc;
      double ty = center.y() + lr1*ts;

      CQChartsGeom::Point pt = plot_->windowToPixel(CQChartsGeom::Point(tx, ty));

      QPointF pt1(pt.x, pt.y);

      // calc text angle
      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      // draw label
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      plot_->textBox()->draw(painter, pt1, label(), angle, align);
    }
  }
}

//------

CQChartsPieGroupObj::
CQChartsPieGroupObj(const CQChartsPiePlot *plot, const CQChartsGeom::BBox &bbox,
                    int groupInd, const QString &name, int ig, int ng) :
 CQChartsGroupObj(const_cast<CQChartsPiePlot *>(plot), bbox), plot_(plot), groupInd_(groupInd),
 name_(name), ig_(ig), ng_(ng)
{
}

QString
CQChartsPieGroupObj::
calcId() const
{
  return QString("group:%1").arg(ig_);
}

QString
CQChartsPieGroupObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name());
  tableTip.addTableRow("Count", numValues());

  return tableTip.str();
}

void
CQChartsPieGroupObj::
addObject(CQChartsPieObj *obj)
{
  obj->setGroupObj(this);

  objs_.push_back(obj);
}

CQChartsPieObj *
CQChartsPieGroupObj::
lookupObj(const QString &name) const
{
  // TODO: use map
  for (const auto &obj : objs_)
    if (obj->label() == name)
      return obj;

  return nullptr;
}

bool
CQChartsPieGroupObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point center(0, 0);

  double r = p.distanceTo(center);

  double ro = plot_->outerRadius();
  double ri = (plot_->isDonut() ? plot_->innerRadius()*plot_->outerRadius() : 0.0);

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(atan2(p.y - center.y, p.x - center.x));
  a = CMathUtil::normalizeAngle(a);

  double a1 = startAngle_; a1 = CMathUtil::normalizeAngle(a1);
  double a2 = endAngle_  ; a2 = CMathUtil::normalizeAngle(a2);

  if (a1 < a2) {
    // crosses zero
    if (a >= 0.0 && a <= a1)
      return true;

    if (a <= 360.0 && a >= a2)
      return true;
  }
  else {
    if (a >= a2 && a <= a1)
      return true;
  }

  return false;
}

void
CQChartsPieGroupObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsGeom::Point c(0, 0);

  double ro = plot_->outerRadius();
  double ri = (plot_->isDonut() ? plot_->innerRadius()*plot_->outerRadius() : 0.0);

  //---

  double a1 = startAngle_;
  double a2 = endAngle_;

  double ga = plot_->gapAngle()/2.0;

  double aa1 = a1 - ga;
  double aa2 = a2 + ga;

  //---

  // set pen and brush
  // TODO: more customization support

  QColor bg = bgColor();
  QColor fg = plot_->interpPlotBorderColor(0, 1);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, fg, 1.0);
  plot_->setBrush(brush, true, bg, 1.0);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw pie slice
  plot_->drawPieSlice(painter, c, ri, ro, aa1, aa2);
}

void
CQChartsPieGroupObj::
drawFg(QPainter *painter) const
{
  if (! visible())
    return;

  CQChartsGeom::Point c(0, 0);

  double a1 = startAngle_;
  double a2 = endAngle_;

  double ta = CMathUtil::avg(a1, a2);

  double tangle = CMathUtil::Deg2Rad(ta);

  double tc = cos(tangle);
  double ts = sin(tangle);

  double tx = c.getX() + 0.5*tc;
  double ty = c.getY() + 0.5*ts;

  CQChartsGeom::Point pt = plot_->windowToPixel(CQChartsGeom::Point(tx, ty));

  QPointF pt1(pt.x, pt.y);

  QString label = QString("%1").arg(numValues());

  //---

  // set text pen
  QPen pen;

  QColor fg = plot_->interpPlotBorderColor(0, 1);

  plot_->setPen(pen, true, fg, 1.0);

  //---

  painter->setPen(pen);

  CQChartsTextOptions textOptions = plot_->adjustTextOptions();

  CQChartsDrawUtil::drawTextAtPoint(painter, pt1, label, textOptions);
}

QColor
CQChartsPieGroupObj::
bgColor() const
{
  return plot_->interpPaletteColor(ig_, ng_);
}

//------

CQChartsPieKeyColor::
CQChartsPieKeyColor(CQChartsPiePlot *plot, CQChartsPlotObj *obj) :
 CQChartsKeyColorBox(plot, 0, 1), obj_(obj)
{
}

bool
CQChartsPieKeyColor::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  int ih = 0;

  if      (group)
    ih = group->groupInd();
  else if (obj)
    ih = obj->colorInd();

  plot->setSetHidden(ih, ! plot->isSetHidden(ih));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsPieKeyColor::
fillBrush() const
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  int ng = plot->numGroups();

  QColor c;
  int    no = 1;

  if      (group) {
    if (! plot->isCount()) {
      int ig = group->groupInd();

      no = group->numObjs();

      c = plot->interpGroupPaletteColor(ig, ng, 0, no);

      if (plot->isSetHidden(ig))
        c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());
    }
    else
      c = group->bgColor();
  }
  else if (obj) {
    CQChartsPieGroupObj *group = obj->groupObj();

    no = (group ? group->numObjs() : 1);

    int ig = (group ? group->colorInd() : 0);
    int io = obj->colorInd();

    c = plot->interpGroupPaletteColor(ig, ng, io, no);

    if (plot->isSetHidden(io))
      c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());
  }

  if (obj && obj->color().isValid())
    c = plot_->charts()->interpColor(obj->color(), obj->colorInd(), no);

  return c;
}

//------

CQChartsPieKeyText::
CQChartsPieKeyText(CQChartsPiePlot *plot, CQChartsPlotObj *plotObj) :
 CQChartsKeyText(plot, "", 0, 0), obj_(plotObj)
{
  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if (group)
    setText(group->name());
  else
    setText(obj->keyLabel());
}

QColor
CQChartsPieKeyText::
interpTextColor(int i, int n) const
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  int ih = 0;

  if      (group)
    ih = group->groupInd();
  else if (obj)
    ih = obj->colorInd();

  if (plot && plot->isSetHidden(ih))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

//------

CQChartsPieTextObj::
CQChartsPieTextObj(const CQChartsPiePlot *plot) :
 CQChartsRotatedTextBoxObj(const_cast<CQChartsPiePlot *>(plot)), plot_(plot)
{
}
