#ifndef CQChartsColumn_H
#define CQChartsColumn_H

#include <QString>
#include <QStringList>
#include <vector>
#include <iostream>
#include <cassert>

class CQChartsColumn {
 public:
  enum class Type {
    NONE,
    DATA,
    VHEADER,
    EXPR
  };

 public:
  static void registerMetaType();

 public:
  CQChartsColumn() = default;
  CQChartsColumn(int column, int role=-1);
  CQChartsColumn(const QString &s);

  CQChartsColumn(const CQChartsColumn &rhs);

 ~CQChartsColumn();

  CQChartsColumn &operator=(const CQChartsColumn &rhs);

  //--

  bool isValid() const { return type_ != Type::NONE; }

  Type type() const { return type_; }

  //--

  bool hasColumn() const { return (type_ == Type::DATA && column_ >= 0); }

  int column() const { return (hasColumn() ? column_ : -1); }

  //--

  bool hasRole() const { return (type_ == Type::DATA && role_ >= 0); }

  int role() const { return (hasRole() ? role_ : -1); }

  int role(int defRole) const { return (hasRole() ? role_ : defRole); }

  //--

  bool hasExpr() const { return (type_ == Type::EXPR && expr_); }

  QString expr() const { return QString(hasExpr() ? expr_ : ""); }

  //--

  bool setValue(const QString &str);

  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; }

  double mapMin() const { return mapMin_; }
  void setMapMin(double r) { mapMin_ = r; }

  double mapMax() const { return mapMax_; }
  void setMapMax(double r) { mapMax_ = r; }

  //---

  QString toString() const;

  void fromString(const QString &s);

  //---

  static int cmp(const CQChartsColumn &lhs, const CQChartsColumn &rhs);

  friend bool operator==(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return cmp(lhs, rhs) == 0;
  }

  friend bool operator!=(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return ! operator==(lhs, rhs);
  }

  friend bool operator<(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return cmp(lhs, rhs) < 0;
  }

  //---

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CQChartsColumn &l) {
    l.print(os);

    return os;
  }

  //---

 public:
  static bool stringToColumns(const QString &str, std::vector<CQChartsColumn> &columns);

  static QString columnsToString(const std::vector<CQChartsColumn> &columns);

 private:
  bool decodeString(const QString &str, Type &type, int &column, int &role, QString &expr);

 private:
  Type   type_   { Type::NONE };
  int    column_ { -1 };
  int    role_   { -1 };
  char*  expr_   { nullptr };
  bool   mapped_ { false };
  double mapMin_ { 0.0 };
  double mapMax_ { 1.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColumn)

#endif
