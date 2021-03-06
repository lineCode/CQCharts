#ifndef CQChartsAngle_H
#define CQChartsAngle_H

#include <CMathUtil.h>
#include <QString>

/*!
 * \brief class to contain charts angle (degrees)
 * \ingroup Charts
 *
 * TODO: support postfix for type of degrees/radians
 */
class CQChartsAngle {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  enum Type {
    DEGREES,
    RADIANS
  };

 public:
  //! default constructor
  CQChartsAngle() = default;

  explicit CQChartsAngle(const QString &str);

  explicit CQChartsAngle(double a) :
   a_(a) {
  }

  double value() const { return a_; }
  void setValue(double a) { a_ = a; }

  //---

  bool isValid() const { return true; }

  //---

  double degrees() const { return a_; }
  double radians() const { return CMathUtil::Deg2Rad(a_); }

  //---

  friend CQChartsAngle operator+(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return CQChartsAngle(lhs.a_ + rhs.a_);
  }

  CQChartsAngle &operator+=(const CQChartsAngle &rhs) { a_ += rhs.a_; return *this; }

  friend CQChartsAngle operator-(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return CQChartsAngle(lhs.a_ - rhs.a_);
  }

  CQChartsAngle &operator-=(const CQChartsAngle &rhs) { a_ -= rhs.a_; return *this; }

  //---

  //! operator ==
  friend bool operator==(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return (lhs.a_ == rhs.a_);
  }

  //! operator !=
  friend bool operator!=(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

 private:
  double a_ { 0.0 }; //!< angle
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsAngle)

#endif
