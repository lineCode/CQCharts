#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>

class CQChartsDataLabel;

//---

// bar chart plot type
class CQChartsBarChartPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }

  QString yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }
  bool allowYAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBarChartPlot;

// values for bar (normal: 1 value : range bar: multiple values)
// ToDo: handle range bar in distribution plot
class CQChartsBarChartValue {
 public:
  using NameValues = std::map<QString,QString>;

 public:
  struct ValueInd {
    double      value { 0.0 };
    QModelIndex ind;
    int         vrow { -1 };
  };

  using ValueInds = std::vector<ValueInd>;

 public:
  CQChartsBarChartValue() { }

  void addValueInd(const ValueInd &valueInd) {
    valueInds_.push_back(valueInd);
  }

  const ValueInds &valueInds() const { return valueInds_; }

  const QString &valueName() const { return valueName_; }
  void setValueName(const QString &s) { valueName_ = s; }

  const QString &groupName() const { return groupName_; }
  void setGroupName(const QString &s) { groupName_ = s; }

  const NameValues &nameValues() const { return nameValues_; }
  void setNameValues(const NameValues &v) { nameValues_ = v; }

  void setNameValue(const QString &name, QString &value) {
    nameValues_[name] = value;
  }

  QString getNameValue(const QString &name) const {
    auto p = nameValues_.find(name);
    if (p == nameValues_.end()) return "";

    return (*p).second;
  }

  void calcRange(ValueInd &minInd, ValueInd &maxInd) const {
    assert(! valueInds_.empty());

    minInd = valueInds_[0];
    maxInd = valueInds_[0];

    for (std::size_t i = 1; i < valueInds_.size(); ++i) {
      double value = valueInds_[i].value;

      if (value < minInd.value)
        minInd = valueInds_[i];

      if (value > minInd.value)
        maxInd = valueInds_[i];
    }
  }

 private:
  ValueInds  valueInds_;
  QString    valueName_;
  QString    groupName_;
  NameValues nameValues_;
};

//------

// set of value bars for group
class CQChartsBarChartValueSet {
 public:
  using Values = std::vector<CQChartsBarChartValue>;

 public:
  CQChartsBarChartValueSet() = default;

  CQChartsBarChartValueSet(const QString &name, int ind) :
   name_(name), ind_(ind) {
  }

  const QString &name() const { return name_; }

  int ind() const { return ind_; }

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

  int numValues() const { return values_.size(); }

  const Values &values() const { return values_; }

  void addValue(const CQChartsBarChartValue &value) {
    values_.push_back(value);
  }

  const CQChartsBarChartValue &value(int i) const {
    assert(i >= 0 && i < int(values_.size()));

    return values_[i];
  }

  void calcSums(double &posSum, double &negSum) const {
    posSum = 0.0;
    negSum = 0.0;

    for (auto &v : values_) {
      for (auto &vi : v.valueInds()) {
        double value = vi.value;

        if (value >= 0) posSum += value;
        else            negSum += value;
      }
    }
  }

 private:
  QString name_;            // group name
  int     ind_      { 0 };  // index
  int     groupInd_ { -1 }; // group ind
  Values  values_;          // value bars
};

//------

// bar object
class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString group READ groupStr)
  Q_PROPERTY(QString name  READ nameStr )
  Q_PROPERTY(QString value READ valueStr)

 public:
  CQChartsBarChartObj(const CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                      int iset, int nset, int ival, int nval, int isval, int nsval,
                      const QModelIndex &ind);

  QString calcId() const override;

  QString calcTipId() const override;

  QString groupStr() const;
  QString nameStr () const;
  QString valueStr() const;

  void setColor(const CQChartsColor &color) { color_ = color; }

  CQChartsGeom::BBox dataLabelRect() const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) const override;

  const CQChartsBarChartValue *value() const;

 private:
  const CQChartsBarChartPlot* plot_  { nullptr }; // parent plot
  int                         iset_  { -1 };      // set number
  int                         nset_  { -1 };      // number of sets
  int                         ival_  { -1 };      // value number
  int                         nval_  { -1 };      // number of values
  int                         isval_ { -1 };      // sub set number
  int                         nsval_ { -1 };      // number of sub sets
  QModelIndex                 ind_;               // model index
  CQChartsColor               color_;             // custom color
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n);

  void setColor(const CQChartsColor &color) { color_ = color; }

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  bool isSetHidden() const;

  void setSetHidden(bool b);

 private:
  CQChartsBarChartPlot* plot_  { nullptr }; // plot
  CQChartsColor         color_;             // custom color
};

// key text
class CQChartsBarKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBarKeyText(CQChartsBarChartPlot *plot, const QString &text, int i, int n);

  QColor interpTextColor(int i, int n) const override;
};

//---

// bar chart plot
//  x     : name
//  y     : value(s)
//  group : group(s)
//  bar   : custom color, stacked, percent, range, horizontal, margin, border, fill
class CQChartsBarChartPlot : public CQChartsBarPlot,
 public CQChartsObjDotPointData<CQChartsBarChartPlot> {
  Q_OBJECT

  // style
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn labelColumn READ labelColumn WRITE setLabelColumn)

  // options
  Q_PROPERTY(PlotType  plotType  READ plotType  WRITE setPlotType )
  Q_PROPERTY(ValueType valueType READ valueType WRITE setValueType)

  Q_PROPERTY(bool percent    READ isPercent    WRITE setPercent   )
  Q_PROPERTY(bool colorBySet READ isColorBySet WRITE setColorBySet)

  // dot line
  Q_PROPERTY(bool           dotLines     READ isDotLines   WRITE setDotLines    )
  Q_PROPERTY(CQChartsLength dotLineWidth READ dotLineWidth WRITE setDotLineWidth)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Dot,dot)

  Q_ENUMS(PlotType)
  Q_ENUMS(ValueType)

 public:
  enum class PlotType {
    NORMAL,
    STACKED,
  };

  enum class ValueType {
    VALUE,
    RANGE,
    MIN,
    MAX,
    MEAN
  };

 public:
  CQChartsBarChartPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsBarChartPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  //---

  PlotType plotType() const { return plotType_; }

  bool isNormal () const { return (plotType() == PlotType::NORMAL ); }
  bool isStacked() const { return (plotType() == PlotType::STACKED); }

  ValueType valueType() const { return valueType_; }

  bool isValueValue() const { return (valueType_ == ValueType::VALUE); }
  bool isValueRange() const { return (valueType_ == ValueType::RANGE); }
  bool isValueMin  () const { return (valueType_ == ValueType::MIN  ); }
  bool isValueMax  () const { return (valueType_ == ValueType::MAX  ); }
  bool isValueMean () const { return (valueType_ == ValueType::MEAN ); }

  bool isPercent() const { return percent_; }

  //---

  bool isColorBySet() const { return colorBySet_; }
  void setColorBySet(bool b);

  //---

  bool isDotLines() const { return dotLines_; }

  const CQChartsLength &dotLineWidth() const { return dotLineWidth_; }
  void setDotLineWidth(const CQChartsLength &l);

  //---

  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  QString valueStr(double v) const;

  void addKeyItems(CQChartsPlotKey *key) override;

   //---

  bool isSetHidden  (int i) const;
  bool isValueHidden(int i) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  double getPanX(bool is_shift) const override;
  double getPanY(bool is_shift) const override;

  //---

 public slots:
  // set horizontal
  void setHorizontal(bool b) override;

  // set plot type
  void setPlotType(PlotType plotType);

  // set normal or stacked
  void setNormal (bool b);
  void setStacked(bool b);

  // set value type
  void setValueType(ValueType valueType);

  void setValueValue(bool b);
  void setValueRange(bool b);
  void setValueMin  (bool b);
  void setValueMax  (bool b);
  void setValueMean (bool b);

  // set percent
  void setPercent(bool b);

  // set dot lines
  void setDotLines(bool b);

 private:
  void addRow(const ModelVisitor::VisitData &data, CQChartsGeom::Range &dataRange) const;

  void addRowColumn(const ModelVisitor::VisitData &data, const CQChartsColumns &valueColumns,
                    CQChartsGeom::Range &dataRange) const;

  void initRangeAxes();

  void initObjAxes();

 private:
  using ValueSets     = std::vector<CQChartsBarChartValueSet>;
  using ValueNames    = std::vector<QString>;
  using ValueGroupInd = std::map<int,int>;

  struct ValueData {
    ValueSets     valueSets;     // value sets
    ValueGroupInd valueGroupInd; // group ind to value index map

    void clear() {
      valueSets    .clear();
      valueGroupInd.clear();
    }
  };

 public:
  int numValueSets() const { return valueData_.valueSets.size(); }

  const CQChartsBarChartValueSet &valueSet(int i) const {
    assert(i >= 0 && i < int(valueData_.valueSets.size()));
    return valueData_.valueSets[i];
  }

  int numSetValues() const {
    return (! valueData_.valueSets.empty() ? valueData_.valueSets[0].numValues() : 0);
  }

 private:
  const CQChartsBarChartValueSet *groupValueSet(int groupId) const;
  CQChartsBarChartValueSet *groupValueSet(int groupId);

 private:
  CQChartsColumn     nameColumn_;                          // name column
  CQChartsColumn     labelColumn_;                         // data label column
  PlotType           plotType_       { PlotType::NORMAL }; // plot type
  ValueType          valueType_      { ValueType::VALUE }; // bar value type
  bool               percent_        { false };            // percent values
  bool               colorBySet_     { false };            // color bars by set or value
  bool               dotLines_       { false };            // show dot lines
  CQChartsLength     dotLineWidth_   { "3px" };            // dot line width
  CQChartsDataLabel* dataLabel_      { nullptr };          // data label data
  int                numVisible_     { 0 };                // number of visible bars
  double             barWidth_       { 1.0 };              // bar width
  ValueData          valueData_;                           // value data
};

#endif
