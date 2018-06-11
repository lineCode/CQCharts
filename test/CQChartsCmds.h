#ifndef CQChartsCmds_H
#define CQChartsCmds_H

#include <CQCharts.h>
#include <CQChartsInitData.h>
#include <CQChartsGeom.h>
#include <CQExprModel.h>
#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QPointer>

#include <boost/optional.hpp>

class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsGradientPalette;
class CQChartsPaletteColorData;
class CQChartsColumn;
class CQChartsModelData;
class CQChartsPlotType;
class CQChartsAnnotation;

#ifdef CQ_CHARTS_CEIL
class ClLanguageCommand;
class ClLanguageArgs;
#endif

class CQChartsCmds;

class CExpr;
class CQTcl;

class QAbstractItemModel;
class QItemSelectionModel;

//---

class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   const QString &procName);

 private:
  QString getCeilCmd(const QString &id) const;
  QString getTclCmd(const QString &id) const;

 public slots:
  void objIdPressed       (const QString &);
  void annotationIdPressed(const QString &);

 private:
  CQChartsCmds* cmds_ { nullptr };
  CQChartsView* view_ { nullptr };
  CQChartsPlot* plot_ { nullptr };
  QString       procName_;
};

//---

class CQChartsCmds : public QObject {
  Q_OBJECT

 public:
  using ParserType = CQCharts::ParserType;
  using Args       = std::vector<QString>;
  using Vars       = std::vector<QVariant>;
  using OptReal    = boost::optional<double>;
  using ModelP     = QSharedPointer<QAbstractItemModel>;
  using Strs       = std::vector<QString>;
  using ViewP      = QPointer<CQChartsView>;
  using Plots      = std::vector<CQChartsPlot *>;

 public:
  CQChartsCmds(CQCharts *charts);
 ~CQChartsCmds();

  CQCharts *charts() const { return charts_; }

  CExpr* expr() const { return expr_; }

  const ParserType &parserType() const;
  void setParserType(const ParserType &type);

  void addCeilCommand(const QString &name);
  void addTclCommand (const QString &name);

  void addCommand(const QString &name);
  void addCommands();

  bool processCmd(const QString &cmd, const Vars &vars);

  static QString fixTypeName(const QString &typeName);

  void setViewProperties(CQChartsView *view, const QString &properties);
  void setPlotProperties(CQChartsPlot *plot, const QString &properties);

  //---

  static void setColumnFormats(CQCharts *charts, const ModelP &model, const QString &columnType);

  bool loadFileModel(const QString &filename, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  //---

  bool sortModel(ModelP &model, const QString &args);
  bool sortModel(ModelP &model, int column, Qt::SortOrder order);

  //---

  CQChartsPlot *createPlot(CQChartsView *view, const ModelP &model, QItemSelectionModel *sm,
                           CQChartsPlotType *type, const CQChartsNameValueData &nameValueData,
                           bool reuse, const CQChartsGeom::BBox &bbox);

  //---

  int initModelData(ModelP &model);

  CQChartsModelData *getModelDataOrCurrent(int ind);

  CQChartsModelData *getModelData(int ind);

  //---

  CQChartsView *getView(bool reuse=true);

  CQChartsView *addView();

  //---

  static bool isCompleteLine(QString &line, bool &join);

  void parseLine(const QString &line, bool log=true);

  void parseScriptLine(const QString &line);

  static bool stringToColumn(QAbstractItemModel *model, const QString &str,
                             CQChartsColumn &column);

  //---

  CQTcl *qtcl() const { return qtcl_; }

 private:
  friend class CQChartsCeilCmd;

  bool loadModelCmd   (const Vars &vars);
  void processModelCmd(const Vars &vars);
  void sortModelCmd   (const Vars &vars);
  void filterModelCmd (const Vars &vars);

  void correlationModelCmd(const Vars &vars);
  void exportModelCmd     (const Vars &vars);

  void groupPlotsCmd(const Vars &vars);
  void placePlotsCmd(const Vars &vars);

  void measureTextCmd(const Vars &vars);

  void createViewCmd(const Vars &vars);

  void createPlotCmd(const Vars &vars);
  void removePlotCmd(const Vars &vars);

  void getPropertyCmd(const Vars &vars);
  void setPropertyCmd(const Vars &vars);

  void getChartsDataCmd(const Vars &vars);
  void setChartsDataCmd(const Vars &vars);

  void getPaletteCmd(const Vars &vars);
  void setPaletteCmd(const Vars &vars);

  void createTextShapeCmd    (const Vars &vars);
  void createArrowShapeCmd   (const Vars &vars);
  void createRectShapeCmd    (const Vars &vars);
  void createEllipseShapeCmd (const Vars &vars);
  void createPolygonShapeCmd (const Vars &vars);
  void createPolylineShapeCmd(const Vars &vars);
  void createPointShapeCmd   (const Vars &vars);

  void connectChartCmd(const Vars &vars);

  void loadModelDlgCmd  (const Vars &vars);
  void createModelDlgCmd(const Vars &vars);
  void createPlotDlgCmd (const Vars &vars);

  void letCmd     (const Vars &vars);
  void ifCmd      (const Vars &vars);
  void whileCmd   (const Vars &vars);
  void continueCmd(const Vars &vars);
  void printCmd   (const Vars &vars);
  void sourceCmd  (const Vars &vars);

  //---

  void setPaletteData(CQChartsGradientPalette *palette,
                      const CQChartsPaletteColorData &paletteData);

  QStringList stringToCmds(const QString &str) const;

#ifdef CQ_CHARTS_CEIL
  Vars parseCommandArgs(ClLanguageCommand *command, ClLanguageArgs *largs);
#endif

  //---

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  QAbstractItemModel *loadCsv (const QString &filename, const CQChartsInputData &inputData);
  QAbstractItemModel *loadTsv (const QString &filename, const CQChartsInputData &inputData);
  QAbstractItemModel *loadJson(const QString &filename, bool &hierarchical);
  QAbstractItemModel *loadData(const QString &filename, const CQChartsInputData &inputData);

  QAbstractItemModel *createExprModel(int n=100);

  QAbstractItemModel *createVarsModel(const Vars &vars);

  //---

  CQChartsView *getViewByName(const QString &viewName) const;

  bool getPlotsByName(CQChartsView *view, const Vars &plotNames, Plots &plot) const;

  CQChartsPlot *getOptPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsPlot *getPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsAnnotation *getAnnotationByName(CQChartsView *view, const QString &name) const;
  CQChartsAnnotation *getAnnotationByName(CQChartsPlot *plot, const QString &name) const;

  //---

  void setCmdRc(int rc);
  void setCmdRc(double rc);
  void setCmdRc(const QString &rc);
  void setCmdRc(const QVariant &rc);
  void setCmdRc(const QList<QVariant> &rc);

  static void errorMsg(const QString &msg);

 signals:
  void updateModelDetails(int ind);
  void updateModel(int ind);

 private:
  using CommandNames = std::vector<QString>;

  CQCharts*    charts_       { nullptr };
  CExpr*       expr_         { nullptr };
  bool         continueFlag_ { false };
  CQTcl*       qtcl_         { nullptr };
  CommandNames commandNames_;
};

#endif
