#ifndef CQChartsCompositePlot_H
#define CQChartsCompositePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Composite plot type
 * \ingroup Charts
 */
class CQChartsCompositePlotType : public CQChartsPlotType {
 public:
  CQChartsCompositePlotType();

  QString name() const override { return "composite"; }
  QString desc() const override { return "Composite"; }

  void addParameters() override;

  bool canProbe() const override;

  bool hasObjs() const override;

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsCompositePlot;

//---

/*!
 * \brief Composite Plot
 * \ingroup Charts
 */
class CQChartsCompositePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CompositeType compositeType  READ compositeType  WRITE setCompositeType )
  Q_PROPERTY(bool          commonXRange   READ isCommonXRange WRITE setCommonXRange  )
  Q_PROPERTY(bool          commonYRange   READ isCommonYRange WRITE setCommonYRange  )
  Q_PROPERTY(bool          currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)

  Q_ENUMS(CompositeType);

 public:
  enum class CompositeType {
    NONE,
    X1X2,
    Y1Y2,
    TABBED
  };

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsCompositePlot(View *view, const ModelP &model);
 ~CQChartsCompositePlot();

  //---

  void init() override;
  void term() override;

  //---

  const CompositeType &compositeType() const { return compositeType_; }
  void setCompositeType(const CompositeType &t);

  bool isCommonXRange() const { return commonXRange_; }
  void setCommonXRange(bool b);

  bool isCommonYRange() const { return commonYRange_; }
  void setCommonYRange(bool b);

  //---

  void addPlot(Plot *plot);

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  void clearPlotObjects() override;

  void initPlotObjs() override;

  void initObjTree() override;

  BBox calcAnnotationBBox() const override;

  void updateAxisRanges(const BBox &adjustedRange) override;

  //---

  BBox adjustedViewBBox(const Plot *plot) const override;

  //---

  void waitRange() override;
  void waitDraw() override;
  void waitObjs() override;
  void waitTree() override;

  //---

  bool hasTitle() const override;

  bool hasXAxis() const override;
  bool hasYAxis() const override;

  bool hasObjs(const CQChartsLayer::Type &layerType) const override;

  //---

  void drawBackgroundDeviceParts(PaintDevice *device, bool bgLayer, bool bgAxes,
                                 bool bgKey) const override;

  void drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid,
                             bool fg, bool annotations) const override;

  void drawForegroundDeviceParts(PaintDevice *device, bool fgAxes, bool fgKey,
                                 bool title, bool foreground, bool tabbed) const override;

  void drawOverlayDeviceParts(PaintDevice *device, bool sel_objs,
                              bool sel_annotations, bool boxes, bool edit_handles,
                              bool over_objs, bool over_annotations) const override;

  void drawBgAxes(PaintDevice *device) const override;
  void drawFgAxes(PaintDevice *device) const override;

  void drawTitle(PaintDevice *device) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  void resetKeyItems() override;

  //---

  bool selectPress  (const Point &p, SelMod selMod) override;
  bool selectMove   (const Point &p, bool first=false) override;
  bool selectRelease(const Point &p) override;

  //---

  void invalidateOverlay() override;
  void invalidateLayers() override;
  void invalidateLayer(const CQChartsBuffer::Type &layerType) override;

  //---

  Plot *currentPlot() const;
  void setCurrentPlot(Plot *plot);

  int childPlotIndex(const Plot *) const override;
  int numChildPlots() const override;
  Plot *childPlot(int i) const override;

  int currentPlotInd() const;
  void setCurrentPlotInd(int i);

 private slots:
  void currentPlotSlot();

 private:
  void updatePlots();

  void setOverlayAxisLabels();

 private:
  using Plots = std::vector<Plot*>;

  Plots         plots_;
  Plot*         currentPlot_   { nullptr };
  CompositeType compositeType_ { CompositeType::NONE };
  bool          commonXRange_  { true };
  bool          commonYRange_  { true };
};

#endif
