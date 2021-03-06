#ifndef CQChartsHierPlot_H
#define CQChartsHierPlot_H

#include <CQChartsHierPlotType.h>
#include <CQChartsPlot.h>

//----

/*!
 * \brief Hierarchical base plot
 * \ingroup Charts
 */
class CQChartsHierPlot : public CQChartsPlot {
  Q_OBJECT

  //! columns
  Q_PROPERTY(CQChartsColumns nameColumns READ nameColumns WRITE setNameColumns)
  Q_PROPERTY(CQChartsColumn  valueColumn READ valueColumn WRITE setValueColumn)

  //! hier separator
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

  //! follow view
  Q_PROPERTY(bool followViewExpand READ isFollowViewExpand WRITE setFollowViewExpand)

 public:
  CQChartsHierPlot(View *view, CQChartsPlotType *type, const ModelP &model);
 ~CQChartsHierPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set name columns
  const Columns &nameColumns() const { return nameColumns_; }
  void setNameColumns(const Columns &c);

  //! get/set value column
  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  ColumnType valueColumnType() const { return valueColumnType_; }

  //---

  //! get/set separator
  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  //---

  //! get/set folow view expand
  bool isFollowViewExpand() const { return followViewExpand_; }
  void setFollowViewExpand(bool b);

  virtual void followViewExpandChanged() { }

  void expandedModelIndices(std::set<QModelIndex> &indSet) const;

  //---

  void addProperties() override;

  void addHierProperties();

 protected:
  // columns
  Columns nameColumns_; //!< multiple name columns
  Column  valueColumn_; //!< value column

  ColumnType valueColumnType_ { ColumnType::NONE }; //!< value column type

  // options
  QString separator_        { "/" };   //!< hierarchical name separator
  bool    followViewExpand_ { false }; //!< follow view expand
};

#endif
