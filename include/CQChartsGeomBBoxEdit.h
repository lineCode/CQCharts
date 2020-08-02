#ifndef CQChartsGeomBBoxEdit_H
#define CQChartsGeomBBoxEdit_H

#include <CQChartsGeom.h>

class CQChartsPlot;
class CQChartsLineEdit;
class QToolButton;

/*!
 * \brief geometry bbox edit
 * \ingroup Charts
 */
class CQChartsGeomBBoxEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::BBox bbox READ getValue WRITE setValue)

 public:
  using BBox = CQChartsGeom::BBox;

 public:
  CQChartsGeomBBoxEdit(QWidget *parent, const BBox &value=BBox(0, 0, 1, 1));
  CQChartsGeomBBoxEdit(const BBox &value=BBox(0, 0, 1, 1));

 ~CQChartsGeomBBoxEdit() { }

  const BBox &getValue() const { return bbox_; }
  void setValue(const BBox &bbox);

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p);

  void setRegion(const CQChartsGeom::BBox &bbox);

 private:
  void init(const BBox &value);

 private slots:
  void editingFinishedI();

  void regionSlot(bool b);
  void regionReleaseSlot(const CQChartsGeom::BBox &bbox);

 signals:
  void valueChanged();

  void regionChanged();

 private:
  void updateRange();

  void bboxToWidget();
  bool widgetToBBox();

 private:
  BBox              bbox_           { 0, 0, 1, 1 };
  CQChartsPlot*     plot_           { nullptr };
  CQChartsLineEdit* edit_           { nullptr };
  QToolButton*      regionButton_   { nullptr };
  mutable bool      disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomBBox
 * \ingroup Charts
 */
class CQChartsGeomBBoxPropertyViewType : public CQPropertyViewType {
 public:
  using PropertyItem = CQPropertyViewItem;
  using Delegate     = CQPropertyViewDelegate;
  using StyleOption  = QStyleOptionViewItem;

 public:
  CQChartsGeomBBoxPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(PropertyItem *item, const QVariant &value) override;

  void draw(PropertyItem *item, const Delegate *delegate, QPainter *painter,
            const StyleOption &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_bbox"; }

 private:
  QString valueString(PropertyItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsGeomBBox
 * \ingroup Charts
 */
class CQChartsGeomBBoxPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsGeomBBoxPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
