#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsLineDash.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQChartsColor.h>
#include <CQBaseModel.h>
#include <CQExprModel.h>
#include <CQChartsEval.h>
#include <COSNaN.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QPen>
#include <QStringList>
#include <QRectF>

using CQChartsNameValues = std::map<QString,QString>;

class CQCharts;
class CQChartsColumnType;
class CQDataModel;

namespace CQChartsUtil {

bool isHierarchical(QAbstractItemModel *model);

int hierRowCount(QAbstractItemModel *model);

QString parentPath(QAbstractItemModel *model, const QModelIndex &parent);

//------

bool columnValueType(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                     CQBaseModel::Type &columnType, CQChartsNameValues &nameValues);

bool formatColumnTypeValue(CQCharts *charts, const QString &typeStr, double value, QString &str);

bool formatColumnValue(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                       double value, QString &str);

bool formatColumnTypeValue(CQChartsColumnType *typeData, const CQChartsNameValues &nameValues,
                           double value, QString &str);

QVariant columnDisplayData(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column, const QVariant &var, bool &converted);

QVariant columnUserData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                        const QVariant &var, bool &converted);

bool columnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                   const CQChartsColumn &column, QString &typeStr);

bool setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

#if 0
void remapColumnTime(QAbstractItemModel *model, const CQChartsColumn &column,
                     CQChartsColumnType *typeData, const CQChartsNameValues &nameValues);
#endif

//------

inline bool isNaN(double r) {
  return COSNaN::is_nan(r) && ! COSNaN::is_inf(r);
}

inline bool isInf(double r) {
  return COSNaN::is_inf(r);
}

inline double getNaN() {
  double r;

  COSNaN::set_nan(r);

  return r;
}

//------

inline bool isInteger(double r) {
  if (isNaN(r))
    return false;

  return std::abs(r - int(r)) < 1E-3;
}

inline bool realEq(double r1, double r2) {
  if (isNaN(r1) || isNaN(r2)) return false;

  return std::fabs(r2 - r1) < 1E-5;
}

inline bool isZero(double r) {
  if (isNaN(r)) return false;

  return realEq(r, 0.0);
}

//------

inline double toReal(const QString &str, bool &ok) {
  ok = true;

  double r = 0.0;

  //---

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return r;
  }

  const char *p;

#ifdef ALLOW_NAN
  if (COS::has_nan() && strncmp(&c_str[i], "NaN", 3) == 0)
    p = &c_str[i + 3];
  else {
    errno = 0;

    r = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE) {
      ok = false;
      return r;
    }
  }
#else
  errno = 0;

  r = strtod(&c_str[i], (char **) &p);

  if (errno == ERANGE) {
    ok = false;
    return r;
  }
#endif

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0')
    ok = false;

  return r;
}

inline bool toReal(const QString &str, double &r) {
  bool ok;

  r = toReal(str, ok);

  return ok;
}

//------

inline long toInt(const QString &str, bool &ok) {
  ok = true;

  long integer = 0;

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return integer;
  }

  const char *p;

  errno = 0;

  integer = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE) {
    ok = false;
    return integer;
  }

  if (*p == '.') {
    ++p;

    while (*p == '0')
      ++p;
  }

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0') {
    ok = false;
    return integer;
  }

  return integer;
}

inline bool toInt(const QString &str, long &i) {
  bool ok;

  i = toInt(str, ok);

  return ok;
}

//------

QString toString(double r, const QString &fmt="%g" );
QString toString(long   i, const QString &fmt="%ld");

//------

QString toString(const std::vector<CQChartsColumn> &columns);

bool fromString(const QString &str, std::vector<CQChartsColumn> &columns);

//------

inline QPointF toQPoint(const CQChartsGeom::Point &point) {
  return QPointF(point.x, point.y);
}

inline QPoint toQPointI(const CQChartsGeom::Point &point) {
  return QPoint(point.x, point.y);
}

inline CQChartsGeom::Point fromQPoint(const QPointF &point) {
  return CQChartsGeom::Point(point.x(), point.y());
}

inline CQChartsGeom::Point fromQPointF(const QPoint &point) {
  return CQChartsGeom::Point(point.x(), point.y());
}

inline QRectF toQRect(const CQChartsGeom::BBox &rect) {
  return QRectF(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

inline QRect toQRectI(const CQChartsGeom::BBox &rect) {
  return QRect(toQPointI(rect.getLL()), toQPointI(rect.getUR())).normalized();
}

inline CQChartsGeom::BBox fromQRect(const QRectF &rect) {
  return CQChartsGeom::BBox(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

//------

// sign of value
template<typename T>
int sign(T v) {
  return (T(0) < v) - (v < T(0));
}

// average of two reals
inline double avg(double x1, double x2) {
  return (x1 + x2)/2.0;
}

// map x in low->high to 0->1
inline double norm(double x, double low, double high) {
  if (high != low)
    return (x - low)/(high - low);
  else
    return 0;
}

// map x in 0->1 to low->high
inline double lerp(double x, double low, double high) {
  return low + (high - low)*x;
}

// map value in range low1->high2 to low2->high2
inline double map(double value, double low1, double high1, double low2, double high2) {
  return lerp(norm(value, low1, high1), low2, high2);
}

// clamp real value to range
inline double clamp(double val, double low, double high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

// clamp integer value to range
inline double iclamp(int val, int low, int high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

//---

inline bool intersectLines(const QPointF &l1s, const QPointF &l1e,
                           const QPointF &l2s, const QPointF &l2e, QPointF &pi) {
  double dx1 = l1e.x() - l1s.x();
  double dy1 = l1e.y() - l1s.y();
  double dx2 = l2e.x() - l2s.x();
  double dy2 = l2e.y() - l2s.y();

  double delta = dx1*dy2 - dy1*dx2;

  if (fabs(delta) < 1E-6) // parallel
    return false;

  double idelta = 1.0/delta;

  double dx = l2s.x() - l1s.x();
  double dy = l2s.y() - l1s.y();

  double m1 = (dx*dy2 - dy*dx2)*idelta;
//double m2 = (dx*dy1 - dy*dx1)*idelta;

  double xi = l1s.x() + m1*dx1;
  double yi = l1s.y() + m1*dy1;

  pi = QPointF(xi, yi);

  return true;
}

inline bool intersectLines(double x11, double y11, double x21, double y21,
                           double x12, double y12, double x22, double y22,
                           double &xi, double &yi) {
  QPointF pi;

  bool rc = intersectLines(QPointF(x11, y11), QPointF(x21, y21),
                           QPointF(x12, y12), QPointF(x22, y22), pi);

  xi = pi.x();
  yi = pi.y();

  return rc;
}

//---

inline QColor bwColor(const QColor &c) {
  int g = qGray(c.red(), c.green(), c.blue());

  return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
}

inline QColor invColor(const QColor &c) {
  return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
}

inline QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

inline QColor blendColors(const std::vector<QColor> &colors) {
  if (colors.empty())
    return QColor();

  double f = 1.0/colors.size();

  double r = 0.0;
  double g = 0.0;
  double b = 0.0;

  for (const auto &c : colors) {
    r += c.redF  ()*f;
    g += c.greenF()*f;
    b += c.blueF ()*f;
  }

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

//------

inline void penSetLineDash(QPen &pen, const CQChartsLineDash &dash) {
  int num = dash.getNumLengths();

  if (num > 0) {
    pen.setStyle(Qt::CustomDashLine);

    pen.setDashOffset(dash.getOffset());

    QVector<qreal> dashes;

    double w = pen.widthF();

    if (w <= 0.0) w = 1.0;

    for (int i = 0; i < num; ++i)
      dashes << dash.getLength(i)*w;

    if (num & 1)
      dashes << dash.getLength(0)*w;

    pen.setDashPattern(dashes);
  }
  else
    pen.setStyle(Qt::SolidLine);
}

//------

enum Rounding {
  ROUND_DOWN,
  ROUND_UP,
  ROUND_NEAREST
};

inline int RoundNearest(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x - 0.499999);
  else
    x1 = (x + 0.500001);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

inline int RoundUp(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x       - 1E-6);
  else
    x1 = (x + 1.0 - 1E-6);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

inline int RoundDown(double x) {
  double x1;

  if (x >= 0.0)
    x1 = (x       + 1E-6);
  else
    x1 = (x - 1.0 + 1E-6);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

inline int Round(double x, Rounding rounding=ROUND_NEAREST) {
  switch (rounding) {
    case ROUND_UP  : return RoundUp(x);
    case ROUND_DOWN: return RoundDown(x);
    default        : return RoundNearest(x);
  }
}

inline double RoundNearestF(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x - 0.499999);
  else
    x1 = (x + 0.500001);

  return std::trunc(x1);
}

inline double RoundUpF(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x       - 1E-6);
  else
    x1 = (x + 1.0 - 1E-6);

  return std::trunc(x1);
}

inline double RoundDownF(double x) {
  double x1;

  if (x >= 0.0)
    x1 = (x       + 1E-6);
  else
    x1 = (x - 1.0 + 1E-6);

  return std::trunc(x1);
}

inline double RoundF(double x, Rounding rounding=ROUND_NEAREST) {
  switch (rounding) {
    case ROUND_UP  : return RoundUpF(x);
    case ROUND_DOWN: return RoundDownF(x);
    default        : return RoundNearestF(x);
  }
}

//------

inline double Deg2Rad(double d) { return M_PI*d/180.0; }
inline double Rad2Deg(double r) { return 180.0*r/M_PI; }

inline double normalizeAngle(double a, bool isEnd=false) {
  while (a < 0.0) a += 360.0;

  if (! isEnd) {
    while (a >= 360.0) a -= 360.0;
  }
  else {
    while (a > 360.0) a -= 360.0;
  }

  return a;
}

inline CQChartsGeom::Point AngleToPoint(const CQChartsGeom::Point &c,
                                        double xr, double yr, double a) {
  double ra = CQChartsUtil::Deg2Rad(a);

  double x = c.x + xr*cos(ra);
  double y = c.y + yr*sin(ra);

  return CQChartsGeom::Point(x, y);
}

inline CQChartsGeom::Point AngleToPoint(const CQChartsGeom::Point &c, double r, double a) {
  return AngleToPoint(c, r, r, a);
}

//------

// distance between two points
inline double PointPointDistance(const CQChartsGeom::Point &point1,
                                 const CQChartsGeom::Point &point2) {
  double dx = point1.x - point2.x;
  double dy = point1.y - point2.y;

  return std::hypot(dx, dy);
}

// distance between point and line
inline bool PointLineDistance(const CQChartsGeom::Point &point,
                              const CQChartsGeom::Point &lineStart,
                              const CQChartsGeom::Point &lineEnd, double *dist) {
  double dx1 = lineEnd.x - lineStart.x;
  double dy1 = lineEnd.y - lineStart.y;

  double dx2 = point.x - lineStart.x;
  double dy2 = point.y - lineStart.y;

  double u1 = dx2*dx1 + dy2*dy1;
  double u2 = dx1*dx1 + dy1*dy1;

  if (u2 <= 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }

  double u = u1/u2;

  if      (u < 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }
  else if (u > 1.0) {
    *dist = PointPointDistance(point, lineEnd);
    return false;
  }
  else {
    CQChartsGeom::Point intersection = lineStart + u*CQChartsGeom::Point(dx1, dy1);

    *dist = PointPointDistance(point, intersection);

    return true;
  }
}

//------

bool fileToLines(const QString &filename, QStringList &lines, int maxLines=-1);

//------

QString pointToString  (const QPointF   &point);
QString rectToString   (const QRectF    &rect);
QString polygonToString(const QPolygonF &poly);

bool variantToString(const QVariant &var, QString &str);

inline QString toString(const QVariant &var, bool &ok) {
  ok = true;

  if (! var.isValid()) {
    ok = false;

    return "";
  }

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline bool toString(const QVariant &var, QString &str) {
  bool ok;

  str = toString(var, ok);

  return ok;
}

inline bool isReal(const QVariant &var) {
  return (var.type() == QVariant::Double);
}

inline double toReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.value<double>();

  QString str = toString(var, ok);

  if (! ok)
    return getNaN();

  return toReal(str, ok);
}

inline bool toReal(const QVariant &var, double &r) {
  bool ok;

  r = toReal(var, ok);

  return ok;
}

inline bool isInt(const QVariant &var) {
  return (var.type() == QVariant::Bool || var.type() == QVariant::Char ||
          var.type() == QVariant::Int  || var.type() == QVariant::LongLong ||
          var.type() == QVariant::UInt);
}

inline long toInt(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Int)
    return var.value<int>();

  if (var.type() == QVariant::Double) {
    double r = var.value<double>();

    if (isInteger(r))
      return int(r);
  }

  QString str = toString(var, ok);

  if (! ok)
    return 0;

  return toInt(str, ok);
}

inline bool isNumeric(const QVariant &var) {
  return isReal(var) || isInt(var);
}

//------

int nameToRole(const QString &name);

//------

inline QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                 Qt::Orientation orientation, int role, bool &ok) {
  ok = false;

  if (column.type() != CQChartsColumn::Type::DATA)
    return QVariant();

  int icolumn = column.column();

  if (icolumn < 0)
    return QVariant();

  QVariant var = model->headerData(icolumn, orientation, role);

  ok = var.isValid();

  return var;
}

inline QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                 Qt::Orientation orientation, bool &ok) {
  return modelHeaderValue(model, column, orientation, column.role(Qt::DisplayRole), ok);
}

inline QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                 int role, bool &ok) {
  return modelHeaderValue(model, column, Qt::Horizontal, role, ok);
}

inline QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                 bool &ok) {
  return modelHeaderValue(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

inline QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                 Qt::Orientation orient, int role, bool &ok) {
  ok = false;

  if (! column.isValid())
    return "";

  QVariant var = modelHeaderValue(model, column, orient, role, ok);
  if (! var.isValid()) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                 Qt::Orientation orient, bool &ok) {
  return modelHeaderString(model, column, orient, column.role(Qt::DisplayRole), ok);
}

inline QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                 int role, bool &ok) {
  return modelHeaderString(model, column, Qt::Horizontal, role, ok);
}

inline QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                 bool &ok) {
  return modelHeaderString(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

inline bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                Qt::Orientation orient, const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA)
    return false;

  return model->setHeaderData(column.column(), orient, var, role);
}

inline bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                Qt::Orientation orientation, const QVariant &var) {
  return setModelHeaderValue(model, column, orientation, var, column.role(Qt::EditRole));
}

inline bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                const QVariant &var, int role) {
  return setModelHeaderValue(model, column, Qt::Horizontal, var, role);
}

inline bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                const QVariant &var) {
  return setModelHeaderValue(model, column, Qt::Horizontal, var, column.role(Qt::EditRole));
}

//--

inline bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                          const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA)
    return false;

  QModelIndex ind = model->index(row, column.column(), QModelIndex());

  return model->setData(ind, var, role);
}

inline bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                          const QVariant &var) {
  return setModelValue(model, row, column, var, column.role(Qt::EditRole));
}

//------

inline bool stringToBool(const QString &str, bool *ok) {
  QString lstr = str.toLower();

  if (lstr == "0" || lstr == "false" || lstr == "no") {
    *ok = true;
    return false;
  }

  if (lstr == "1" || lstr == "true" || lstr == "yes") {
    *ok = true;
    return true;
  }

  *ok = false;

  return false;
}

//------

inline bool varToBool(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Bool)
    return var.toBool();

  return false;
}

inline bool isColor(const QVariant &var) {
  return (var.type() == QVariant::Color);
}

inline CQChartsColor varToColor(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Color)
    return var.value<QColor>();

  QColor c(var.toString());

  if (c.isValid())
    return CQChartsColor(c);

  ok = false;

  return CQChartsColor();
}

//---

inline QString varToString(const QVariant &var, bool &ok) {
  QString str;

  ok = variantToString(var, str);

  return str;
}

std::vector<double> varToReals(const QVariant &var, bool &ok);

std::vector<double> stringToReals(const QString &str, bool &ok);

//---

inline QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  if (! ind.isValid()) {
    ok = false;

    return QVariant();
  }

  QVariant var = model->data(ind, role);

  ok = var.isValid();

  return var;
}

inline QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, Qt::EditRole, ok);

  if (! ok)
    var = modelValue(model, ind, Qt::DisplayRole, ok);

  return var;
}

inline QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) {
  if (! column.isValid()) {
    ok = false;

    return QVariant();
  }

  if      (column.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, column.column(), parent);

    return modelValue(model, ind, role, ok);
  }
  else if (column.type() == CQChartsColumn::Type::VHEADER) {
    QVariant var = model->headerData(row, Qt::Vertical, role);

    ok = var.isValid();

    return var;
  }
  else if (column.type() == CQChartsColumn::Type::EXPR) {
    QVariant var;

    ok = CQChartsEvalInst->evalExpr(row, column.expr(), var);

    return var;
  }
  else {
    ok = false;

    return QVariant();
  }
}

inline QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) {
  QVariant var;

  if (! column.hasRole()) {
    var = modelValue(model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      var = modelValue(model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    var = modelValue(model, row, column, parent, column.role(), ok);

  return var;
}

//---

inline QString modelString(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QString str = modelString(model, ind, Qt::EditRole, ok);

  if (! ok)
    str = modelString(model, ind, Qt::DisplayRole, ok);

  return str;
}

inline QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(model, row, column, parent, role, ok);
  if (! ok) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) {
  QString str;

  if (! column.hasRole()) {
    str = modelString(model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      str = modelString(model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    str = modelString(model, row, column, parent, column.role(), ok);

  return str;
}

inline QString modelHierString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                               const QModelIndex &parent, int role, bool &ok) {
  QString s = modelString(model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(model, row1, column, parent1, role, ok);
    }
  }

  return s;
}

inline QString modelHierString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                               const QModelIndex &parent, bool &ok) {
  QString s = modelString(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(model, row1, column, parent1, ok);
    }
  }

  return s;
}

//---

inline double modelReal(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return 0.0;

  return toReal(var, ok);
}

inline double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  double r = modelReal(model, ind, Qt::EditRole, ok);

  if (! ok)
    r = modelReal(model, ind, Qt::DisplayRole, ok);

  return r;
}

inline double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                        const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(model, row, column, parent, role, ok);
  if (! ok) return 0.0;

  return toReal(var, ok);
}

inline double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                        const QModelIndex &parent, bool &ok) {
  double r;

  if (! column.hasRole()) {
    r = modelReal(model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      r = modelReal(model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    r = modelReal(model, row, column, parent, column.role(), ok);

  return r;
}

inline double modelHierReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                            const QModelIndex &parent, int role, bool &ok) {
  double r = modelReal(model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(model, row1, column, parent1, role, ok);
    }
  }

  return r;
}

inline double modelHierReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                            const QModelIndex &parent, bool &ok) {
  double r = modelReal(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(model, row1, column, parent1, ok);
    }
  }

  return r;
}

//---

inline long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return 0;

  return toInt(var, ok);
}

inline long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  long l = modelInteger(model, ind, Qt::EditRole, ok);

  if (! ok)
    l = modelInteger(model, ind, Qt::DisplayRole, ok);

  return l;
}

inline long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                         const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(model, row, column, parent, role, ok);
  if (! ok) return 0;

  return toInt(var, ok);
}

inline long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                         const QModelIndex &parent, bool &ok) {
  long l;

  if (! column.hasRole()) {
    l = modelInteger(model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      l = modelInteger(model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    l = modelInteger(model, row, column, parent, column.role(), ok);

  return l;
}

inline long modelHierInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                             const QModelIndex &parent, int role, bool &ok) {
  long l = modelInteger(model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      l = modelInteger(model, row1, column, parent1, role, ok);
    }
  }

  return l;
}

inline long modelHierInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                             const QModelIndex &parent, bool &ok) {
  long l = modelInteger(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      l = modelInteger(model, row1, column, parent1, ok);
    }
  }

  return l;
}

//---

inline CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind,
                                int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return CQChartsColor();

  if (isColor(var))
    return CQChartsColor(var.value<QColor>());

  CQChartsColor color;

  if (isReal(var)) {
    double r;

    if (toReal(var, r))
      color = CQChartsColor(CQChartsColor::Type::PALETTE, r);
  }
  else {
    QString str;

    if (CQChartsUtil::toString(var, str))
      color = CQChartsColor(str);
  }

  return color;
}

inline CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  CQChartsColor c = modelColor(model, ind, Qt::EditRole, ok);

  if (! ok)
    c = modelColor(model, ind, Qt::DisplayRole, ok);

  return c;
}

inline CQChartsColor modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                                const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(model, row, column, parent, role, ok);
  if (! ok) return CQChartsColor();

  if (isColor(var))
    return CQChartsColor(var.value<QColor>());

  CQChartsColor color;

  if (isReal(var)) {
    double r;

    if (toReal(var, r))
      color = CQChartsColor(CQChartsColor::Type::PALETTE, r);
  }
  else {
    QString str;

    if (CQChartsUtil::toString(var, str))
      color = CQChartsColor(str);
  }

  return color;
}

inline CQChartsColor modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                                const QModelIndex &parent, bool &ok) {
  CQChartsColor c;

  if (! column.hasRole()) {
    c = modelColor(model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      c = modelColor(model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    c = modelColor(model, row, column, parent, column.role(), ok);

  return c;
}

//------

inline bool isValidModelColumn(QAbstractItemModel *model, int column) {
  return (column >= 0 && column < model->columnCount());
}

inline int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name) {
  int role = Qt::DisplayRole;

  for (int icolumn = 0; icolumn < model->columnCount(); ++icolumn) {
    QVariant var = model->headerData(icolumn, Qt::Horizontal, role);

    if (! var.isValid())
      continue;

    //QString name1 = CQChartsUtil::toString(var, rc);

    QString name1;

    bool rc = variantToString(var, name1);
    assert(rc);

    if (name == name1)
      return icolumn;
  }

  //---

  bool ok;

  int column = name.toInt(&ok);

  if (ok)
    return column;

  return -1;
}

inline bool stringToColumn(const QAbstractItemModel *model, const QString &str,
                           CQChartsColumn &column) {
  CQChartsColumn column1(str);

  if (column1.isValid()) {
    column = column1;

    return true;
  }

  //---

  if (! str.length())
    return false;

  int icolumn = modelColumnNameToInd(model, str);

  if (icolumn >= 0) {
    column = CQChartsColumn(icolumn);
    return true;
  }

  return false;
}

//------

inline bool decodeModelFilterStr(QAbstractItemModel *model, const QString &filter,
                                 QString &filter1, int &column) {
  filter1 = filter;
  column  = -1;

  if (! filter1.length())
    return false;

  QStringList strs = filter1.split(':', QString::KeepEmptyParts);

  if (strs.size() != 2)
    return false;

  column = modelColumnNameToInd(model, strs[0]);

  if (column < 0)
    return false;

  filter1 = strs[1];

  return true;
}

//------

void exportModel(QAbstractItemModel *model, CQBaseModel::DataType type,
                 bool hheader=true, bool vheader=false,
                 std::ostream &os=std::cout);

}

//------

namespace CQChartsUtil {

void processAddExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, CQExprModel::Function function,
                      const CQChartsColumn &column, const QString &expr);

CQExprModel *getExprModel(QAbstractItemModel *model);
CQDataModel *getDataModel(QAbstractItemModel *model);

}

//------

namespace CQChartsUtil {

// compare reals with tolerance
struct RealCmp {
  bool operator()(const double &lhs, const double &rhs) const {
    if (CQChartsUtil::realEq(lhs, rhs))
      return false;

    return lhs < rhs;
  }
};

// compare colors
struct ColorCmp {
  bool operator()(const CQChartsColor &lhs, const CQChartsColor &rhs) const {
    return (CQChartsColor::cmp(lhs, rhs) < 0);
  }

  bool operator()(const QColor &lhs, const QColor &rhs) const {
    return (CQChartsColor::cmp(lhs, rhs) < 0);
  }
};

//------

bool stringToPolygons(const QString &str, std::vector<QPolygonF> &polys);

bool stringToPolygon (const QString &str, QPolygonF &poly );
bool stringToRect    (const QString &str, QRectF    &rect );
bool stringToPoint   (const QString &str, QPointF   &point);

//------

QString pathToString(const CQChartsPath &path);
bool stringToPath(const QString &str, CQChartsPath &path);

//------

QString styleToString(const CQChartsStyle &style);
bool stringToStyle(const QString &str, CQChartsStyle &style);

//------

QString timeToString(const QString &fmt, double r);
bool    stringToTime(const QString &fmt, const QString &str, double &t);

//------

bool formatStringInRect(const QString &str, const QFont &font,
                        const QRectF &rect, QStringList &strs);

}

//------

#include <functional>

class CQChartsScopeGuard {
 public:
  template<class Callable>
  CQChartsScopeGuard(Callable &&f) :
   f_(std::forward<Callable>(f)) {
  }

  CQChartsScopeGuard(CQChartsScopeGuard &&other) :
   f_(std::move(other.f_)) {
    other.f_ = nullptr;
  }

 ~CQChartsScopeGuard() {
    if (f_)
      f_(); // must not throw
  }

  void dismiss() noexcept {
    f_ = nullptr;
  }

  CQChartsScopeGuard(const CQChartsScopeGuard&) = delete;
  void operator = (const CQChartsScopeGuard&) = delete;

 private:
  std::function<void()> f_;
};

//------

namespace CQChartsUtil {

template<class T, class Callable>
void testAndSet(T &t, const T &v, Callable &&f) {
  if (v != t) {
    t = v;

    f();
  }
}

}

//------

namespace CQChartsUtil {

inline bool getBoolEnv(const char *name, bool def=false) {
  char *env = getenv(name);

  if (! env)
    return def;

  if      (strcmp(env, "0") == 0 || strcmp(env, "false") == 0 || strcmp(env, "no" ) == 0)
    return false;
  else if (strcmp(env, "1") == 0 || strcmp(env, "true" ) == 0 || strcmp(env, "yes") == 0)
    return true;

  assert(false);

  return true;
}

}

//------

#include <CQChartsModelVisitor.h>

namespace CQChartsUtil {

bool visitModel(QAbstractItemModel *model, CQChartsModelVisitor &visitor);

bool visitModel(QAbstractItemModel *model, const QModelIndex &parent, int r,
                CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State visitModelIndex(QAbstractItemModel *model, const QModelIndex &parent,
                                            CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State visitModelRow(QAbstractItemModel *model, const QModelIndex &parent,
                                          int r, CQChartsModelVisitor &visitor);

}

//------

#endif
