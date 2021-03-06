#ifndef CQChartsConnectionPlot_H
#define CQChartsConnectionPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsConnectionList.h>

#include <CQPerfMonitor.h>

//---

/*!
 * \brief Connection Plot Type
 * \ingroup Charts
 */
class CQChartsConnectionPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsConnectionPlotType();

  void addParameters() override;

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Graph, graph)
CQCHARTS_NAMED_SHAPE_DATA(Edge, edge)

/*!
 * \brief Connection Plot Base Class
 * \ingroup Charts
 *
 */
class CQChartsConnectionPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)

  Q_PROPERTY(CQChartsColumn linkColumn READ linkColumn WRITE setLinkColumn)
  Q_PROPERTY(CQChartsColumn pathColumn READ pathColumn WRITE setPathColumn)
  Q_PROPERTY(CQChartsColumn fromColumn READ fromColumn WRITE setFromColumn)
  Q_PROPERTY(CQChartsColumn toColumn   READ toColumn   WRITE setToColumn  )

  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn depthColumn READ depthColumn WRITE setDepthColumn)

  Q_PROPERTY(CQChartsColumn attributesColumn READ attributesColumn WRITE setAttributesColumn)

  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn WRITE setGroupColumn)
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn)

  // link separator
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

  // options
  Q_PROPERTY(bool   symmetric READ isSymmetric WRITE setSymmetric)
  Q_PROPERTY(bool   sorted    READ isSorted    WRITE setSorted   )
  Q_PROPERTY(int    maxDepth  READ maxDepth    WRITE setMaxDepth )
  Q_PROPERTY(double minValue  READ minValue    WRITE setMinValue )

 public:
  using ColumnArray = std::vector<Column>;

 public:
  CQChartsConnectionPlot(View *view, PlotType *plotType, const ModelP &model);
 ~CQChartsConnectionPlot();

  //---

  void init() override;
  void term() override;

  //---

  // get/set node column
  const Column &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const Column &c);

  // get/set connections column
  const Column &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const Column &c);

  //! get/set link column
  const Column &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const Column &c);

  //! get/set path column
  const Column &pathColumn() const { return pathColumn_; }
  void setPathColumn(const Column &c);

  //! get/set from column
  const Column &fromColumn() const { return fromColumn_; }
  void setFromColumn(const Column &c);

  //! get/set to column
  const Column &toColumn() const { return toColumn_; }
  void setToColumn(const Column &c);

  //! get/set value column
  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //! get/set depth column
  const Column &depthColumn() const { return depthColumn_; }
  void setDepthColumn(const Column &c);

  //! get/set attributes column
  const Column &attributesColumn() const { return attributesColumn_; }
  void setAttributesColumn(const Column &c);

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const Column &c);

  //! get/set name column
  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  //---

  const ColumnArray &modelColumns() const { return modelColumns_; }

  //---

  //! get/set separator
  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s);

  //! get/set symmetric
  bool isSymmetric() const { return symmetric_; }
  void setSymmetric(bool b);

  //---

  //! get/set sorted
  bool isSorted() const { return sorted_; }
  void setSorted(bool b);

  //---

  //! get/set max depth
  int maxDepth() const { return maxDepth_; }
  void setMaxDepth(int d);

  //! get/set min value
  double minValue() const { return minValue_; }
  void setMinValue(double r);

  //---

  //! get/set propagate values
  bool isPropagate() const { return propagateData_.active; }
  void setPropagate(bool b);

  //---

  const ColumnType &connectionsColumnType() const { return connectionsColumnType_; }

  const ColumnType &linkColumnType() const { return linkColumnType_; }

  //---

  //! add properties
  void addProperties() override;

  //---

  bool checkColumns() const;

  //---

  //! group data
  struct GroupData {
    QVariant value;
    int      ig { -1 };
    int      ng { 0 };

    GroupData() = default;

    GroupData(const QVariant &value, int ig, int ng) :
     value(value), ig(ig), ng(ng) {
    }

    bool isValid() const { return (ng > 0 && ig >= 0 && ig < ng); }

    double ivalue() const { return (isValid() ? double(ig)/ng : 0.0); }
  };

  //---

  /*!
   * \brief Table Connection Data
   * \ingroup Charts
   */
  class TableConnectionData {
   public:
    enum class PrimaryType {
      ANY,
      PRIMARY,
      NON_PRIMARY
    };

    //! table value data
    struct Value {
      int     to      { -1 };
      OptReal value;
      bool    primary { true };

      Value() = default;

      Value(int to, const OptReal &value, bool primary) :
       to(to), value(value), primary(primary) {
      }
    };

    using Values = std::vector<Value>;

   public:
    TableConnectionData() { }

    int from() const { return from_; }
    void setFrom(int i) { from_ = i; }

    const QString &name() const { return name_; }
    void setName(const QString &s) { name_ = s; }

    const QString &label() const { return label_; }
    void setLabel(const QString &s) { label_ = s; }

    const GroupData &group() const { return group_; }
    void setGroup(const GroupData &group) { group_ = group; }

    int depth() const { return depth_; }
    void setDepth(int depth) { depth_ = depth; }

    //---

    const Values &values() const { return values_; }

    const Value &ivalue(int i) const { return values_[i]; }

    void addValue(int to, const OptReal &value, bool primary) {
      values_.emplace_back(to, value, primary);

      totalValid_ = false;
    }

    bool hasTo(int to) const {
      for (const auto &v : values_) {
        if (v.to == to)
          return true;
      }

      return false;
    }

    void setToValue(int to, const OptReal &value) {
      for (auto &v : values_) {
        if (v.to == to) {
          v.value = value;
        }
      }
    }

    //---

    const QModelIndex &linkInd() const { return linkInd_; }
    void setLinkInd(const QModelIndex &i) { linkInd_ = i; }

    const QModelIndex &groupInd() const { return groupInd_; }
    void setGroupInd(const QModelIndex &i) { groupInd_ = i; }

    double total(bool primaryOnly=false) const {
      if (totalValid_ && totalPrimary_ == primaryOnly)
        return total_;

      auto *th = const_cast<TableConnectionData *>(this);

      th->calcTotal(primaryOnly);

      return total_;
    }

    void calcTotal(bool primaryOnly) {
      total_ = 0.0;

      for (auto &value : values_) {
        if (primaryOnly && ! value.primary)
          continue;

        if (value.value.isSet())
          total_ += value.value.real();
      }

      totalValid_   = true;
      totalPrimary_ = primaryOnly;
    }

    void sort() {
      std::sort(values_.begin(), values_.end(),
        [](const Value &lhs, const Value &rhs) {
          return lhs.value.realOr(0.0) < rhs.value.realOr(0.0);
        });
    }

   protected:
    int         from_         { -1 };    //!< from node
    QString     name_;                   //!< value name
    QString     label_;                  //!< value label
    GroupData   group_;                  //!< group
    int         depth_        { 0 };     //!< depth
    Values      values_;                 //!< connection values
    QModelIndex linkInd_;                //!< link model index
    QModelIndex groupInd_;               //!< group model index
    double      totalValid_   { false }; //!< is total valid
    bool        totalPrimary_ { false }; //!< is total for primary only
    double      total_        { 0.0 };   //!< value total
  };

  using TableConnectionDatas = std::vector<TableConnectionData>;

  //! table connection info
  struct TableConnectionInfo {
    int    numNonZero { 0 };
    double total      { 0.0 };
  };

 protected:
  //! hierarchical connection data
  struct HierConnectionData {
    ModelIndex  parentLinkInd;
    QStringList linkStrs;
    QString     parentStr;
    double      total      { 0.0 };
    double      childTotal { 0.0 };
  };

  using HierConnectionDataList = std::vector<HierConnectionData>;

  bool initHierObjs() const;

  virtual void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                             const HierConnectionData &destHierData) const = 0;
  virtual void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                             const HierConnectionData &destHierData) const = 0;

  //---

  //! link connection data
  struct LinkConnectionData {
    QString    srcStr;        //!< source string
    QString    destStr;       //!< destination string
    double     value { 0.0 }; //!< value
    int        depth { -1 };  //!< depth
    GroupData  groupData;     //!< group data
    ModelIndex groupModelInd; //!< group model index
    ModelIndex linkModelInd;  //!< link model index
    ModelIndex valueModelInd; //!< value model index
    ModelIndex nameModelInd;  //!< name model index
    ModelIndex depthModelInd; //!< depth model index
  };

  bool initLinkObjs() const;

  virtual void addLinkConnection(const LinkConnectionData &linkConnectionData) const = 0;

  //---

  using ConnectionList = CQChartsConnectionList;
  using Connections    = ConnectionList::Connections;

  //! connections data
  struct ConnectionsData {
    QModelIndex ind;
    int         node    { -1 };
    QString     name;
    GroupData   groupData;
    ModelIndex  groupModelInd;
    ModelIndex  nodeModelInd;
    ModelIndex  connectionsModelInd;
    ModelIndex  nameModelInd;
    double      total   { 0.0 };
    Connections connections;
  };

  using IdConnectionsData = std::map<int, ConnectionsData>;

  //--

  bool initConnectionObjs() const;

  virtual void addConnectionObj(int, const ConnectionsData &) const { }

  //---

  struct PathData {
    QStringList pathStrs;
    double      value { 1.0 };
    ModelIndex  pathModelInd;
    ModelIndex  valueModelInd;
  };

  bool initPathObjs() const;

  virtual void addPathValue(const PathData &) const { }

  //---

  //! from/to connection data
  struct FromToData {
    ModelIndex         fromModelInd;  //!< from model index
    QString            fromStr;       //!< from string
    ModelIndex         toModelInd;    //!< to model index
    QString            toStr;         //!< to string
    ModelIndex         valueModelInd; //!< value model index
    OptReal            value;         //!< optional value
    ModelIndex         depthModelInd; //!< depth model index
    int                depth { -1 };  //!< source node depth
    CQChartsNameValues nameValues;    //!< node/edge attributes
    GroupData          groupData;     //!< grouping data
  };

  bool initFromToObjs() const;

  virtual void addFromToValue(const FromToData &) const { }

  //---

  bool processTableModel(TableConnectionDatas &tableConnectionDatas,
                         TableConnectionInfo &tableConnectionInfo) const;

  //---

  bool groupColumnData(const ModelIndex &groupModelInd, GroupData &groupData) const;

 protected:
  //! value propagation data
  struct PropagateData {
    enum class Type {
      SUM,
      AVERAGE,
      MIN,
      MAX
    };

    bool active { true };
    Type type   { Type::SUM };
  };

  // columns
  Column      nodeColumn_;                                 //!< connection node column
  Column      connectionsColumn_;                          //!< connections column
  ColumnType  connectionsColumnType_ { ColumnType::NONE }; //!< connection column type
  Column      linkColumn_;                                 //!< link column
  Column      pathColumn_;                                 //!< path column
  Column      fromColumn_;                                 //!< from column
  Column      toColumn_;                                   //!< to column
  Column      valueColumn_;                                //!< value column
  Column      depthColumn_;                                //!< depth column
  Column      attributesColumn_;                           //!< attributes column
  Column      groupColumn_;                                //!< group column
  ColumnType  linkColumnType_ { ColumnType::NONE };        //!< link column type
  Column      nameColumn_;                                 //!< name column

  mutable ColumnArray modelColumns_; //!< used columns

  // options
  QString separator_;           //!< separator
  bool    symmetric_ { true };  //!< to/from values are symmetric (from/to auto implied)
  bool    sorted_    { false }; //!< is sorted
  int     maxDepth_  { -1 };    //!< user max depth
  double  minValue_  { -1 };    //!< user min value

  PropagateData propagateData_;
};

#endif
