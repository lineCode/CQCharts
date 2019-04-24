#ifndef CQChartsRectEdit_H
#define CQChartsRectEdit_H

#include <CQChartsRect.h>
#include <QFrame>

class CQChartsUnitsEdit;
class CQRectEdit;

//! \brief rectangle edit
class CQChartsRectEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsRectEdit(QWidget *parent=nullptr);

  const CQChartsRect &rect() const;
  void setRect(const CQChartsRect &pos);

 signals:
  void rectChanged();

 private slots:
  void editChanged();
  void unitsChanged();

 private:
  void rectToWidgets();
  void widgetsToRect();

  void connectSlots(bool b);

 private:
  CQChartsRect       rect_;                  //!< rect data
  CQRectEdit*        edit_      { nullptr }; //!< rect edit
  CQChartsUnitsEdit* unitsEdit_ { nullptr }; //!< units edit
  bool               connected_ { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

//! \brief type for CQChartsRect
class CQChartsRectPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsRectPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

//! \brief editor factory for CQChartsRect
class CQChartsRectPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsRectPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
