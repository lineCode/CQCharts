#ifndef CQChartsObj_H
#define CQChartsObj_H

#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <QObject>
#include <future>
#include <boost/optional.hpp>

class CQCharts;

/*!
 * \brief Charts object base class
 * \ingroup Charts
 */
class CQChartsObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString            id         READ id           WRITE setId        )
  Q_PROPERTY(CQChartsGeom::BBox rect       READ rect         WRITE setRect      )
  Q_PROPERTY(QString            tipId      READ tipId        WRITE setTipId     )
  Q_PROPERTY(bool               visible    READ isVisible    WRITE setVisible   )
  Q_PROPERTY(bool               selected   READ isSelected   WRITE setSelected  )
  Q_PROPERTY(bool               inside     READ isInside     WRITE setInside    )
  Q_PROPERTY(bool               selectable READ isSelectable WRITE setSelectable)
  Q_PROPERTY(bool               editable   READ isEditable   WRITE setEditable  )

 public:
  using BBox  = CQChartsGeom::BBox;
  using Size  = CQChartsGeom::Size;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsObj(CQCharts *charts, const BBox &rect=BBox());

  //---

  CQCharts *charts() const { return charts_; }

  //---

  // unique id of object
  bool hasId() const { return !!id_; }

  const QString &id() const;
  void setId(const QString &s);

  // calculate unique id of object (on demand)
  virtual QString calcId() const { return ""; }

  //---

  //! get set rect
  const BBox &rect() const { return rect_; }
  virtual void setRect(const BBox &r) { rect_ = r; dataInvalidate(); }

  //---

  virtual bool intersectShape(const Point & /*p1*/, const Point & /*p2*/,
                              Point & /*pi*/) const { return false; }

  //---

  // tip id for object (string to display in tooltip)
  bool hasTipId() const { return !!tipId_; }

  const QString &tipId() const;
  void setTipId(const QString &s) { tipId_ = s; dataInvalidate(); }
  void resetTipId() { tipId_ = OptString(); dataInvalidate(); }

  // calculate tip id (on demand)
  virtual QString calcTipId() const { return calcId(); }

  //---

  //! get/set visible
  virtual bool isVisible() const { return visible_; }
  virtual void setVisible(bool b) { visible_ = b; dataInvalidate(); }

  //! set/get selected
  virtual bool isSelected() const { return selected_; }
  virtual void setSelected(bool b) { selected_ = b; dataInvalidate(); }

  //! set/get inside
  virtual bool isInside() const { return inside_; }
  virtual void setInside(bool b) { inside_ = b; dataInvalidate(); }

  //---

  // is selectable
  virtual bool isSelectable() const { return selectable_; }
  virtual void setSelectable(bool b) { selectable_ = b; }

  // is editable
  virtual bool isEditable() const { return editable_; }
  virtual void setEditable(bool b) { editable_ = b; }

  //---

  virtual bool contains(const Point &p) const = 0;

  //---

  virtual void dataInvalidate() { }

 signals:
  //! emitted when object id changed
  void idChanged();

 protected:
  using OptString = boost::optional<QString>;

  CQCharts*          charts_     { nullptr }; //!< charts
  OptString          id_;                     //!< id
  BBox               rect_;                   //!< bbox
  OptString          tipId_;                  //!< tip id
  bool               visible_    { true };    //!< is visible
  bool               selected_   { false };   //!< is selected
  bool               inside_     { false };   //!< is mouse inside
  bool               editable_   { false };   //!< is editable
  bool               selectable_ { true };    //!< is selectable
  mutable std::mutex mutex_;                  //!< mutex
};

#endif
