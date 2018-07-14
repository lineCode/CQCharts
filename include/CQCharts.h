#ifndef CQCharts_H
#define CQCharts_H

#include <CQChartsModelData.h>
#include <CQBaseModel.h>

#include <QObject>
#include <QAbstractItemModel>

class CQChartsWindow;
class CQChartsView;
class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsColumnTypeMgr;

class CQCharts : public QObject {
  Q_OBJECT

 public:
  using PlotTypes  = std::vector<CQChartsPlotType *>;
  using ModelP     = QSharedPointer<QAbstractItemModel>;
  using ModelDatas = std::vector<CQChartsModelData *>;
  using Views      = std::vector<CQChartsView *>;

 public:
  CQCharts();

  virtual ~CQCharts();

  void init();

  //---

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  void getPlotTypes(PlotTypes &types) const;

  //---

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

  //---

  CQChartsModelData *initModelData(ModelP &model);

  CQChartsModelData *getModelData(QAbstractItemModel *model) const;
  CQChartsModelData *getModelData(int ind) const;

  int currentModelInd() const { return currentModelInd_; }
  void setCurrentModelInd(int ind);

  CQChartsModelData *currentModelData() const;

  void getModelDatas(ModelDatas &modelDatas) const;

  void setModelName(CQChartsModelData *modelData, const QString &name);

  //---

  CQChartsView *addView(const QString &id="");

  virtual CQChartsView *createView();

  CQChartsView *getView(const QString &id) const;

  CQChartsView *currentView() const;

  void getViews(Views &views) const;

  void getViewIds(QStringList &names) const;

  void removeView(CQChartsView *view);

  //---

  CQChartsWindow *createWindow(CQChartsView *view);

  //---

  void emitModelTypeChanged(int modelId);

  //---

  void errorMsg(const QString &msg) const;

 signals:
  void modelDataAdded(int);

  void modelNameChanged(const QString &);

  void modelTypeChanged(int);

  void windowCreated(CQChartsWindow *window);

  void viewAdded(CQChartsView *view);

  void plotAdded(CQChartsPlot *plot);

 private:
  int addModelData(ModelP &model);

 private:
  using NameViews = std::map<QString,CQChartsView*>;

  CQChartsPlotTypeMgr*   plotTypeMgr_     { nullptr };
  CQChartsColumnTypeMgr* columnTypeMgr_   { nullptr };
  int                    currentModelInd_ { -1 };
  ModelDatas             modelDatas_;
  NameViews              views_;
};

#endif
