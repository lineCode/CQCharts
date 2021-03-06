#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPointPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsFillUnder.h>
#include <CQChartsFitData.h>
#include <CQStatData.h>
#include <CQChartsUtil.h>

class CQChartsXYPlot;
class CQChartsXYLabelObj;
class CQChartsXYPolylineObj;
class CQChartsArrow;
class CQChartsGrahamHull;

//---

/*!
 * \brief XY plot type
 * \ingroup Charts
 */
class CQChartsXYPlotType : public CQChartsPointPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief XY Plot Bivariate Line object
 * \ingroup Charts
 */
class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYBiLineObj(const Plot *plot, int groupInd, const BBox &rect,
                      double x, double y1, double y2, const QModelIndex &ind,
                      const ColorInd &is, const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  double x() const { return x_; }
  void setX(double r) { x_ = r; }

  double y1() const { return y1_; }
  void setY1(double r) { y1_ = r; }

  double y2() const { return y2_; }
  void setY2(double r) { y2_ = r; }

  //---

  QString typeName() const override { return "biline"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) override;

 private:
  void drawLines (PaintDevice *device, const Point &p1, const Point &p2);
  void drawPoints(PaintDevice *device, const Point &p1, const Point &p2);

 private:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  double      x_        { 0.0 };     //!< x
  double      y1_       { 0.0 };     //!< start y
  double      y2_       { 0.0 };     //!< end y
};

//---

/*!
 * \brief XY Plot Impulse Line object
 * \ingroup Charts
 */
class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYImpulseLineObj(const Plot *plot, int groupInd, const BBox &rect,
                           double x, double y1, double y2, const QModelIndex &ind,
                           const ColorInd &is, const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  double x() const { return x_; }
  void setX(double r) { x_ = r; }

  double y1() const { return y1_; }
  void setY1(double r) { y1_ = r; }

  double y2() const { return y2_; }
  void setY2(double r) { y2_ = r; }

  //---

  QString typeName() const override { return "impulse"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) override;

 private:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  double      x_        { 0.0 };     //!< x
  double      y1_       { 0.0 };     //!< start y
  double      y2_       { 0.0 };     //!< end y
};

//---

/*!
 * \brief XY Plot Point object
 * \ingroup Charts
 */
class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ point WRITE setPoint)
//Q_PROPERTY(double              x     READ x     WRITE setX)
//Q_PROPERTY(double              y     READ y     WRITE setY)

  Q_PROPERTY(CQChartsSymbol symbolType READ symbolType WRITE setSymbolType)
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)
  Q_PROPERTY(CQChartsLength fontSize   READ fontSize   WRITE setFontSize  )
  Q_PROPERTY(CQChartsColor  color      READ color      WRITE setColor     )

 public:
  using Plot        = CQChartsXYPlot;
  using LabelObj    = CQChartsXYLabelObj;
  using PolylineObj = CQChartsXYPolylineObj;
  using Symbol      = CQChartsSymbol;
  using Length      = CQChartsLength;
  using Color       = CQChartsColor;
  using Image       = CQChartsImage;
  using Units       = CQChartsUnits;

 public:
  CQChartsXYPointObj(const Plot *plot, int groupInd, const BBox &rect, const Point &p,
                     const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

 ~CQChartsXYPointObj();

  //---

  void setSelected(bool b) override;

  //---

  const Plot *plot() const { return plot_; }

  //---

  // position
  const Point &point() const { return pos_; }
  void setPoint(const Point &p) { pos_ = p; }

//double x() const { return pos_.x; }
//void setX(double x) { pos_.setX(x); }

//double y() const { return pos_.y; }
//void setY(double y) { pos_.setY(y); }

  //---

  // get/set associated line object
  const PolylineObj *lineObj() const { return lineObj_; }
  void setLineObj(const PolylineObj *obj) { lineObj_ = obj; }

  //---

  // get/set associated label object
  const LabelObj *labelObj() const { return labelObj_; }
  void setLabelObj(const LabelObj *obj) { labelObj_ = obj; }

  //---

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  Symbol symbolType() const;
  void setSymbolType(Symbol type) { extraData()->symbolType = type; }

  // symbol size
  Length symbolSize() const;
  void setSymbolSize(const Length &s) { extraData()->symbolSize = s; }

  // font size
  Length fontSize() const;
  void setFontSize(const Length &s) { extraData()->fontSize = s; }

  // color
  Color color() const;
  void setColor(const Color &c) { extraData()->color = c; }

  // Image
  Image image() const;
  void setImage(const Image &i) { extraData()->image = i; }

  // vector
  bool isVector() const;
  Point vector() const;
  void setVector(const Point &v) { extraData()->vector = v; }

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

 private:
  using OptPoint = boost::optional<Point>;

  struct ExtraData {
    Symbol   symbolType { Symbol::Type::NONE }; //!< symbol type
    Length   symbolSize { Units::NONE, 0.0 };   //!< symbol size
    Length   fontSize   { Units::NONE, 0.0 };   //!< font size
    Color    color;                             //!< symbol fill color
    OptPoint vector;                            //!< optional vector
    Image    image;                             //!< image
  };

 private:
  const ExtraData *extraData() const;
  ExtraData *extraData();

 private:
  const Plot*        plot_     { nullptr }; //!< parent plot
  int                groupInd_ { -1 };      //!< group ind
  Point              pos_;                  //!< position
  ExtraData*         edata_    { nullptr }; //!< extra data
  const LabelObj*    labelObj_ { nullptr }; //!< label obj
  const PolylineObj* lineObj_  { nullptr }; //!< line obj
};

//---

/*!
 * \brief XY Plot Point Label object
 * \ingroup Charts
 */
class CQChartsXYLabelObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double  x     READ x     WRITE setX    )
  Q_PROPERTY(double  y     READ y     WRITE setY    )
  Q_PROPERTY(QString label READ label WRITE setLabel)

 public:
  using Plot     = CQChartsXYPlot;
  using Column   = CQChartsColumn;
  using PointObj = CQChartsXYPointObj;

 public:
  CQChartsXYLabelObj(const Plot *plot, int groupInd, const BBox &rect,
                     double x, double y, const QString &label, const QModelIndex &ind,
                     const ColorInd &is, const ColorInd &iv);

 ~CQChartsXYLabelObj();

  //---

  const Plot *plot() const { return plot_; }

  double x() const { return pos_.x; }
  void setX(double r) { pos_.setX(r); }

  double y() const { return pos_.y; }
  void setY(double r) { pos_.setY(r); }

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &v) { labelColumn_ = v; }

  //---

  // get/set associated point object
  const PointObj *pointObj() const { return pointObj_; }
  void setPointObj(const PointObj *obj) { pointObj_ = obj; }

  //---

  QString typeName() const override { return "label"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) override;

 private:
  const Plot*     plot_     { nullptr }; //!< parent plot
  int             groupInd_ { -1 };      //!< group ind
  Point           pos_;                  //!< position
  QString         label_;                //!< label string
  Column          labelColumn_;          //!< label column
  const PointObj* pointObj_ { nullptr }; //!< point obj
};

//---

class CQChartsSmooth;

/*!
 * \brief XY Plot Polyline object (connected line)
 * \ingroup Charts
 */
class CQChartsXYPolylineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYPolylineObj(const Plot *plot, int groupInd, const BBox &rect,
                        const Polygon &poly, const QString &name, const ColorInd &is,
                        const ColorInd &ig);

 ~CQChartsXYPolylineObj();

  //---

  const Plot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "polyline"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return poly_; }

  bool isSolid() const override { return false; }

  //---

//bool isVisible() const override;

  bool inside(const Point &p) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

  bool isSelectable() const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  bool isOutlier(double y) const;

  //---

  void resetBestFit();

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

  //---

  void initBestFit();
  void initStats();

  void initSmooth() const;

 private:
  using Smooth   = CQChartsSmooth;
  using FitData  = CQChartsFitData;
  using StatData = CQStatData;
  using Hull     = CQChartsGrahamHull;

  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  Polygon     poly_;                 //!< polygon
  QString     name_;                 //!< name
  Smooth*     smooth_   { nullptr }; //!< smooth object
  FitData     bestFit_;              //!< best fit data
  StatData    statData_;             //!< statistics data
  Hull*       hull_     { nullptr }; //!< hull
};

//---

/*!
 * \brief XY Plot Polygon object (fill under)
 * \ingroup Charts
 */
class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYPolygonObj(const Plot *plot, int groupInd, const BBox &rect,
                       const Polygon &poly, const QString &name, const ColorInd &is,
                       const ColorInd &ig, bool under);

 ~CQChartsXYPolygonObj();

  //---

  const Plot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "polygon"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return poly_; }

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

  bool isSelectable() const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

  //---

  void initSmooth() const;

 private:
  using Smooth = CQChartsSmooth;

  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  Polygon     poly_;                 //!< polygon
  QString     name_;                 //!< name
  bool        under_    { false };   //!< has under points
  Smooth*     smooth_   { nullptr }; //!< smooth object
};

//---

#include <CQChartsKey.h>

/*!
 * \brief XY Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsXYKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using Plot   = CQChartsXYPlot;
  using SelMod = CQChartsSelMod;
  using Obj    = CQChartsPlotObj;

 public:
  CQChartsXYKeyColor(Plot *plot, const ColorInd &is, const ColorInd &ig);

  Plot *plot() const { return plot_; }

  void doSelect(SelMod selMod) override;

  void draw(PaintDevice *device, const BBox &rect) const override;

  QBrush fillBrush() const override;

  Obj *plotObj() const;

 protected:
  void drawLine(PaintDevice *device, const BBox &rect) const;

 protected:
  Plot* plot_ { nullptr }; //!< parent plot
};

/*!
 * \brief XY Plot Key Text
 * \ingroup Charts
 */
class CQChartsXYKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYKeyText(Plot *plot, const QString &text, const ColorInd &is, const ColorInd &ig);

  Plot *plot() const { return plot_; }

  QColor interpTextColor(const ColorInd &ind) const override;

 protected:
  Plot*    plot_ { nullptr }; //!< parent plot
  ColorInd is_   { 0 };       //!< set color index
  ColorInd ig_   { 0 };       //!< group color index
};

//---

CQCHARTS_NAMED_LINE_DATA(Impulse, impulse)
CQCHARTS_NAMED_LINE_DATA(Bivariate, bivariate)
CQCHARTS_NAMED_FILL_DATA(FillUnder, fillUnder)

/*!
 * \brief XY Plot
 * \ingroup Charts
 *
 * Plot Type
 *   + \ref CQChartsXYPlotType
 *
 * Example
 *   + \image html xychart.png
 */
class CQChartsXYPlot : public CQChartsPointPlot,
 public CQChartsObjLineData         <CQChartsXYPlot>,
 public CQChartsObjPointData        <CQChartsXYPlot>,
 public CQChartsObjImpulseLineData  <CQChartsXYPlot>,
 public CQChartsObjBivariateLineData<CQChartsXYPlot>,
 public CQChartsObjFillUnderFillData<CQChartsXYPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn       READ xColumn       WRITE setXColumn      )
  Q_PROPERTY(CQChartsColumns yColumns      READ yColumns      WRITE setYColumns     )
  Q_PROPERTY(CQChartsColumn  labelColumn   READ labelColumn   WRITE setLabelColumn  )
  Q_PROPERTY(CQChartsColumn  vectorXColumn READ vectorXColumn WRITE setVectorXColumn)
  Q_PROPERTY(CQChartsColumn  vectorYColumn READ vectorYColumn WRITE setVectorYColumn)

  // x is string
  Q_PROPERTY(bool mapXColumn READ isMapXColumn WRITE setMapXColumn)

  // bivariate
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Bivariate, bivariate)

  // stacked, cumulative
  Q_PROPERTY(bool stacked    READ isStacked    WRITE setStacked   )
  Q_PROPERTY(bool cumulative READ isCumulative WRITE setCumulative)

  // column series
  Q_PROPERTY(bool columnSeries READ isColumnSeries WRITE setColumnSeries);

  // vectors
  Q_PROPERTY(bool vectors READ isVectors WRITE setVectors)

  // impulse
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Impulse, impulse)

  // point: (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

  Q_PROPERTY(bool pointLineSelect READ isPointLineSelect WRITE setPointLineSelect)
  Q_PROPERTY(int  pointDelta      READ pointDelta        WRITE setPointDelta)
  Q_PROPERTY(int  pointCount      READ pointCount        WRITE setPointCount)
  Q_PROPERTY(int  pointStart      READ pointStart        WRITE setPointStart)

  // lines (selectable, rounded, display, stroke)
  Q_PROPERTY(bool linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(bool roundedLines    READ isRoundedLines    WRITE setRoundedLines   )

  CQCHARTS_LINE_DATA_PROPERTIES

  // key
  Q_PROPERTY(bool keyLine READ isKeyLine WRITE setKeyLine)

  // fill under
  Q_PROPERTY(bool                  fillUnderSelectable
             READ isFillUnderSelectable WRITE setFillUnderSelectable)
  Q_PROPERTY(CQChartsFillUnderPos  fillUnderPos
             READ fillUnderPos          WRITE setFillUnderPos       )
  Q_PROPERTY(CQChartsFillUnderSide fillUnderSide
             READ fillUnderSide         WRITE setFillUnderSide      )

  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(FillUnder, fillUnder)

 public:
  using FillUnderPos  = CQChartsFillUnderPos;
  using FillUnderSide = CQChartsFillUnderSide;

 private:
  struct FillUnderData {
    bool          selectable { false }; //!< is fill under selectable
    FillUnderPos  pos;                  //!< fill under position
    FillUnderSide side;                 //!< fill under side
  };

 public:
  using PolylineObj    = CQChartsXYPolylineObj;
  using PolygonObj     = CQChartsXYPolygonObj;
  using PointObj       = CQChartsXYPointObj;
  using BiLineObj      = CQChartsXYBiLineObj;
  using LabelObj       = CQChartsXYLabelObj;
  using ImpulseLineObj = CQChartsXYImpulseLineObj;

 public:
  CQChartsXYPlot(View *view, const ModelP &model);
 ~CQChartsXYPlot();

  //---

  void init() override;
  void term() override;

  //---

  // x, y, label columns
  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &c);

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &c);

  // vector columns
  const Column &vectorXColumn() const { return vectorXColumn_; }
  void setVectorXColumn(const Column &c);

  const Column &vectorYColumn() const { return vectorYColumn_; }
  void setVectorYColumn(const Column &c);

  //---

  //! get/set map x column
  bool isMapXColumn() const { return mapXColumn_; }
  void setMapXColumn(bool b);

  //---

  // stacked, cumulative
  bool isStacked   () const { return stacked_; }
  bool isCumulative() const { return cumulative_; }

  //---

  // get/set is time series
  bool isColumnSeries() const { return columnSeries_; }
  void setColumnSeries(bool b) { columnSeries_ = b; }

  //---

  // select lines when point selected
  bool isPointLineSelect() const { return pointLineSelect_; }
  void setPointLineSelect(bool b) { pointLineSelect_ = b; }

  // point delta/count
  int pointDelta() const { return pointDelta_; }
  void setPointDelta(int i);

  int pointCount() const { return pointCount_; }
  void setPointCount(int i);

  int pointStart() const { return pointStart_; }
  void setPointStart(int i);

  //---

  // lines selectable, rounded
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b);

  //---

  // draw line on key
  bool isKeyLine() const { return keyLine_; }
  void setKeyLine(bool b);

 private:
  void resetBestFit() override;

  //---

 public:
  // fill under
  bool isFillUnderSelectable() const { return fillUnderData_.selectable; }
  void setFillUnderSelectable(bool b);

  const FillUnderPos &fillUnderPos() const { return fillUnderData_.pos; }
  void setFillUnderPos(const FillUnderPos &p);

  const FillUnderSide &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const FillUnderSide &s);

  //---

  // vectors
  bool isVectors() const;

  //---

  // custom color interp (for overlay)
  QColor interpColor(const Color &c, const ColorInd &ind) const override;

  //---

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void updateColumnNames() override;

  //---

  bool headerSeriesData(std::vector<double> &x) const;

  bool rowData(const ModelVisitor::VisitData &data, double &x, std::vector<double> &yv,
               QModelIndex &ind, bool skipBad) const;

  //---

  PolylineObj *addPolyLine(const Polygon &polyLine, int groupInd, const ColorInd &is,
                           const ColorInd &ig, const QString &name, PlotObjs &pointObjs,
                           PlotObjs &objs) const;

  void addPolygon(const Polygon &poly, int groupInd, const ColorInd &is,
                  const ColorInd &ig, const QString &name, PlotObjs &objs, bool under) const;

  //---

  virtual PointObj *createPointObj(int groupInd, const BBox &rect, const Point &p,
                                   const ColorInd &is, const ColorInd &ig,
                                   const ColorInd &iv) const;

  virtual BiLineObj *createBiLineObj(int groupInd, const BBox &rect, double x, double y1,
                                     double y2, const QModelIndex &ind, const ColorInd &is,
                                     const ColorInd &iv) const;

  virtual LabelObj *createLabelObj(int groupInd, const BBox &rect, double x, double y,
                                   const QString &label, const QModelIndex &ind,
                                   const ColorInd &is, const ColorInd &iv) const;

  virtual ImpulseLineObj *createImpulseLineObj(int groupInd, const BBox &rect, double x,
                                               double y1, double y2, const QModelIndex &ind,
                                               const ColorInd &is, const ColorInd &iv) const;

  virtual PolylineObj *createPolylineObj(int groupInd, const BBox &rect, const Polygon &poly,
                                         const QString &name, const ColorInd &is,
                                         const ColorInd &ig) const;

  virtual PolygonObj *createPolygonObj(int groupInd, const BBox &rect, const Polygon &poly,
                                       const QString &name, const ColorInd &is,
                                       const ColorInd &ig, bool under) const;

  //---

  QString valueName(int is, int ns, int irow, bool tip=false) const;

  void addKeyItems(PlotKey *key) override;

  //---

  double symbolWidth () const { return symbolWidth_ ; }
  double symbolHeight() const { return symbolHeight_; }

  //---

  bool probe(ProbeData &probeData) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  //---

  BBox dataFitBBox() const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  BBox calcAnnotationBBox() const override;

  double xAxisHeight(const CQChartsAxisSide::Type &side) const override;
  double yAxisWidth (const CQChartsAxisSide::Type &side) const override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *device) const override;

  //---

  void drawXAxisAt(PaintDevice *device, Plot *plot, double pos) const override;
  void drawYAxisAt(PaintDevice *device, Plot *plot, double pos) const override;

  //---

  void drawArrow(PaintDevice *device, const Point &p1, const Point &p2) const;

  //---

  void drawXRug(PaintDevice *device) const;
  void drawYRug(PaintDevice *device) const;

  void drawXYRug(PaintDevice *device, const RugP &rug, double delta=0.0) const;

  //---

  // axis names
  bool xAxisName(QString &name, const QString &def="") const override;
  bool yAxisName(QString &name, const QString &def="") const override;

  bool xColumnName(QString &name, const QString &def, bool tip=false) const;
  bool yColumnName(QString &name, const QString &def, bool tip=false) const;

  //---

  // object for group
  PlotObj *getGroupObj(int ig) const;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 public slots:
  // set points visible
  void setPointsSlot(bool b);

  // set lines visible
  void setLinesSlot(bool b);

  // set bivariate
  void setBivariateLinesSlot(bool b);

  // set stacked
  void setStacked(bool b);

  // set cumulative
  void setCumulative(bool b);

  // set impulse
  void setImpulseLinesSlot(bool b);

  // set vectors
  void setVectors(bool b);

  // set fill under
  void setFillUnderFilledSlot(bool b);

 private:
  struct IndPoly {
    using Inds = std::vector<QModelIndex>;

    Inds    inds;
    Polygon poly;
  };

  using SetIndPoly      = std::vector<IndPoly>;
  using GroupSetIndPoly = std::map<int, SetIndPoly>;

 private:
  void initAxes();

  void createGroupSetIndPoly(GroupSetIndPoly &groupSetIndPoly) const;
  bool createGroupSetObjs(const GroupSetIndPoly &groupSetIndPoly, PlotObjs &objs) const;

  bool addBivariateLines(int groupInd, const SetIndPoly &setPoly,
                         const ColorInd &ig, PlotObjs &objs) const;
  bool addLines(int groupInd, const SetIndPoly &setPoly,
                const ColorInd &ig, PlotObjs &objs) const;

  Point calcFillUnderPos(double x, double y) const;

  int numSets() const;

 private:
  using Arrow = CQChartsArrow;

  // columns
  Column  xColumn_;       //!< x column
  Columns yColumns_;      //!< y columns
  Column  labelColumn_;   //!< label column
  Column  vectorXColumn_; //!< vector x direction column
  Column  vectorYColumn_; //!< vector y direction column

  bool mapXColumn_ { false }; //!< map x column value to unique id

  ColumnType xColumnType_ { ColumnType::NONE }; //!< x column type

  bool columnSeries_ { false }; //!< are column as series

  // point data
  bool pointLineSelect_ { false }; //!< select line of point
  int  pointDelta_      { -1 };    //!< point delta
  int  pointCount_      { -1 };    //!< point count
  int  pointStart_      { 0 };     //!< point start (0=start, -1=end, -2=middle)

  // plot type
  bool stacked_         { false }; //!< is stacked
  bool cumulative_      { false }; //!< cumulate values
  bool roundedLines_    { false }; //!< draw rounded (smooth) lines
  bool linesSelectable_ { false }; //!< are lines selectable

  // key
  bool keyLine_ { false }; //!< draw line on key

  // fill under data
  FillUnderData fillUnderData_; //!< fill under data

  // vector data
  Arrow* arrowObj_ { nullptr }; //!< vectors data

  double symbolWidth_  { 1.0 }; //!< current symbol width
  double symbolHeight_ { 1.0 }; //!< current symbol height

  // axis side data
  using AxisSideSize = std::map<CQChartsAxisSide::Type,double>;

  mutable AxisSideSize xAxisSideHeight_; //!< top or bottom
  mutable AxisSideSize yAxisSideWidth_;  //!< left or right
};

#endif
