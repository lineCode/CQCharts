#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <CQChartsTextOptions.h>
#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsSides.h>
#include <CQChartsGeom.h>

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QString>

class CQChartsPaintDevice;

struct CQChartsPenBrush {
  QPen   pen;
  QBrush brush;
};

//---

class CQChartsPenData {
 public:
  CQChartsPenData() = default;

  explicit CQChartsPenData(bool visible, const QColor &color=QColor(),
                           double alpha=1.0, const CQChartsLength &width=CQChartsLength(),
                           const CQChartsLineDash &dash=CQChartsLineDash()) :
   visible_(visible), color_(color), alpha_(alpha), width_(width), dash_(dash) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  double alpha() const { return alpha_; }
  void setAlpha(double r) { alpha_ = r; }

  const CQChartsLength &width() const { return width_; }
  void setWidth(const CQChartsLength &v) { width_ = v; }

  const CQChartsLineDash &dash() const { return dash_; }
  void setDash(const CQChartsLineDash &v) { dash_ = v; }

 private:
  bool             visible_ { true };  //!< visible
  QColor           color_;             //!< pen color
  double           alpha_   { 1.0 };   //!< pen alpha
  CQChartsLength   width_   { "0px" }; //!< pen width
  CQChartsLineDash dash_    { };       //!< pen dash
};

//---

class CQChartsBrushData {
 public:
  CQChartsBrushData() = default;

  explicit CQChartsBrushData(bool visible, const QColor &color=QColor(), double alpha=1.0,
                            const CQChartsFillPattern &pattern=CQChartsFillPattern()) :
   visible_(visible), color_(color), alpha_(alpha), pattern_(pattern) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  double alpha() const { return alpha_; }
  void setAlpha(double r) { alpha_ = r; }

  const CQChartsFillPattern &pattern() const { return pattern_; }
  void setPattern(const CQChartsFillPattern &v) { pattern_ = v; }

  //---

 private:
  bool                visible_ { true };                             //!< visible
  QColor              color_;                                        //!< fill color
  double              alpha_   { 1.0 };                              //!< fill alpha
  CQChartsFillPattern pattern_ { CQChartsFillPattern::Type::SOLID }; //!< fill pattern
};

//---

namespace CQChartsDrawUtil {

inline QPainterPath polygonToPath(const CQChartsGeom::Polygon &polygon, bool closed=false) {
  QPainterPath path;

  path.moveTo(polygon.qpoint(0));

  for (int i = 1; i < polygon.size(); ++i)
    path.lineTo(polygon.qpoint(i));

  if (closed)
    path.closeSubpath();

  return path;
}

}

//---

namespace CQChartsDrawUtil {

void setPenBrush(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush);

void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                        const CQChartsLength &xsize=CQChartsLength(),
                        const CQChartsLength &ysize=CQChartsLength(),
                        const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));
void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly,
                        const CQChartsLength &xsize=CQChartsLength(),
                        const CQChartsLength &ysize=CQChartsLength());

void drawTextInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                   const QString &text, const CQChartsTextOptions &options);

void drawRotatedTextInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                          const QString &text, const QPen &pen, const CQChartsTextOptions &options);

void drawTextAtPoint(CQChartsPaintDevice *device, const CQChartsGeom::Point &p, const QString &text,
                     const CQChartsTextOptions &options=CQChartsTextOptions(),
                     bool centered=false, double dx=0.0, double dy=0.0);

void drawAlignedText(CQChartsPaintDevice *device, const CQChartsGeom::Point &p,
                     const QString &text, Qt::Alignment align, double dx=0.0, double dy=0.0);

CQChartsGeom::BBox calcAlignedTextRect(CQChartsPaintDevice *device, const QFont &font,
                                       const CQChartsGeom::Point &p, const QString &text,
                                       Qt::Alignment align, double dx, double dy);

void drawContrastText(CQChartsPaintDevice *device, const CQChartsGeom::Point &p,
                      const QString &text, double alpha);

CQChartsGeom::Size calcTextSize(const QString &text, const QFont &font,
                                const CQChartsTextOptions &options);

void drawCenteredText(CQChartsPaintDevice *device, const CQChartsGeom::Point &pos,
                      const QString &text);

void drawSimpleText(CQChartsPaintDevice *device, const CQChartsGeom::Point &pos,
                    const QString &text);

void drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
                const CQChartsGeom::Point &c, const CQChartsLength &size);
void drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
                const CQChartsGeom::BBox &bbox);

void drawPieSlice(CQChartsPaintDevice *device, const CQChartsGeom::Point &c,
                  double ri, double ro, double a1, double a2,
                  bool isInvertX=false, bool isInvertY=false);

void drawArc(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
             double angle, double dangle);

void drawArcSegment(CQChartsPaintDevice *device, const CQChartsGeom::BBox &ibbox,
                    const CQChartsGeom::BBox &obbox, double angle, double dangle);

void drawArcsConnector(CQChartsPaintDevice *device, const CQChartsGeom::BBox &ibbox,
                       double a1, double da1, double a2, double da2, bool isSelf);

}

//---

namespace CQChartsDrawPrivate {

// private
CQChartsGeom::Size calcHtmlTextSize(const QString &text, const QFont &font, int margin=0);

void drawScaledHtmlText(CQChartsPaintDevice *device, const CQChartsGeom::BBox &tbbox,
                        const QString &text, const CQChartsTextOptions &options);

void drawHtmlText(CQChartsPaintDevice *device, const CQChartsGeom::BBox &tbbox,
                  const QString &text, const CQChartsTextOptions &options);

}

#endif
