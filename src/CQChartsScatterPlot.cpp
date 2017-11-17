#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsScatterPlotType::
CQChartsScatterPlotType()
{
  addParameters();
}

void
CQChartsScatterPlotType::
addParameters()
{
  addColumnParameter("x", "X", "xColumn", "", 0);
  addColumnParameter("y", "Y", "yColumn", "", 1);

  addColumnParameter("name"      , "Name"       , "nameColumn"      , "optional");
  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn", "optional");
  addColumnParameter("fontSize"  , "Font Size"  , "fontSizeColumn"  , "optional");
  addColumnParameter("color"     , "Color"      , "colorColumn"     , "optional");

  addBoolParameter("textLabels", "Text Labels", "textLabels", "optional");

  CQChartsPlotType::addParameters();
}

CQChartsPlot *
CQChartsScatterPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsScatterPlot(view, model);
}

//---

CQChartsScatterPlot::
CQChartsScatterPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("scatter"), model), dataLabel_(this)
{
  symbolSizeSet_.setMapMin(16);
  symbolSizeSet_.setMapMax(64);

  fontSizeSet_.setMapMin(16);
  fontSizeSet_.setMapMax(64);

  addAxes();

  addKey();

  addTitle();
}

void
CQChartsScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"       , "name"       );
  addProperty("columns", this, "xColumn"          , "x"          );
  addProperty("columns", this, "yColumn"          , "y"          );
  addProperty("columns", this, "symbolSizeColumn" , "symbolSize" );
  addProperty("columns", this, "fontSizeColumn"   , "fontSize"   );
  addProperty("columns", this, "colorColumn"      , "color"      );

  addProperty("symbol", this, "symbolBorderColor"   , "borderColor");
  addProperty("symbol", this, "symbolSize"          , "size"       );
  addProperty("symbol", this, "symbolSizeMapEnabled", "mapEnabled" );
  addProperty("symbol", this, "symbolSizeMapMin"    , "mapMin"     );
  addProperty("symbol", this, "symbolSizeMapMax"    , "mapMax"     );

  addProperty("color", this, "colorMapEnabled", "mapEnabled" );
  addProperty("color", this, "colorMapMin"    , "mapMin"     );
  addProperty("color", this, "colorMapMax"    , "mapMax"     );

  addProperty("font", this, "fontSize"          , "font"      );
  addProperty("font", this, "fontSizeMapEnabled", "mapEnabled");
  addProperty("font", this, "fontSizeMapMin"    , "mapMin"    );
  addProperty("font", this, "fontSizeMapMax"    , "mapMax"    );

  dataLabel_.addProperties("dataLabel");
}

void
CQChartsScatterPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = numRows();

  dataRange_.reset();

  for (int r = 0; r < nr; ++r) {
    QModelIndex xInd = model->index(r, xColumn());
    QModelIndex yInd = model->index(r, yColumn());

    //---

    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model, xInd, ok1);
    double y = CQChartsUtil::modelReal(model, yInd, ok2);

    if (! ok1) x = r;
    if (! ok2) y = r;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
      continue;

    dataRange_.updateRange(x, y);
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  QString xname = model->headerData(xColumn(), Qt::Horizontal).toString();
  QString yname = model->headerData(yColumn(), Qt::Horizontal).toString();

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  if (apply)
    applyDataRange();
}

int
CQChartsScatterPlot::
numRows() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return 0;

  return model->rowCount(QModelIndex());
}

int
CQChartsScatterPlot::
nameIndex(const QString &name) const
{
  int i = 0;

  for (const auto &nv : nameValues_) {
    if (nv.first == name)
      return i;

    ++i;
  }

  return 0;
}

void
CQChartsScatterPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  // init name values
  if (nameValues_.empty()) {
    QAbstractItemModel *model = this->model();

    if (! model)
      return;

    int nr = numRows();

    if (symbolSizeColumn() >= 0) {
      bool ok;

      for (int r = 0; r < nr; ++r) {
        QModelIndex symbolSizeInd = model->index(r, symbolSizeColumn());

        symbolSizeSet_.addValue(CQChartsUtil::modelValue(model, symbolSizeInd, ok));
      }
    }

    if (fontSizeColumn() >= 0) {
      bool ok;

      for (int r = 0; r < nr; ++r) {
        QModelIndex fontSizeInd = model->index(r, fontSizeColumn());

        fontSizeSet_.addValue(CQChartsUtil::modelValue(model, fontSizeInd, ok));
      }
    }

    if (colorColumn() >= 0) {
      bool ok;

      for (int r = 0; r < nr; ++r) {
        QModelIndex colorInd = model->index(r, colorColumn());

        colorSet_.addValue(CQChartsUtil::modelValue(model, colorInd, ok));
      }
    }

    for (int r = 0; r < nr; ++r) {
      QModelIndex nameInd = model->index(r, nameColumn());

      //---

      bool ok;

      QString name = CQChartsUtil::modelString(model, nameInd, ok);

      //---

      QModelIndex xInd = model->index(r, xColumn());
      QModelIndex yInd = model->index(r, yColumn());

      QModelIndex xInd1 = normalizeIndex(xInd);

      //---

      bool ok1, ok2;

      double x = CQChartsUtil::modelReal(model, xInd, ok1);
      double y = CQChartsUtil::modelReal(model, yInd, ok2);

      if (! ok1) x = r;
      if (! ok2) y = r;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        continue;

      //---

      QModelIndex symbolSizeInd = model->index(r, symbolSizeColumn());
      QModelIndex fontSizeInd   = model->index(r, fontSizeColumn  ());
      QModelIndex colorInd      = model->index(r, colorColumn     ());

      bool ok3;

      // get symbol size label (needed if not string ?)
      QString symbolSizeStr = CQChartsUtil::modelString(model, symbolSizeInd, ok3);

      // get font size label (needed if not string ?)
      QString fontSizeStr = CQChartsUtil::modelString(model, fontSizeInd, ok3);

      // get color label (needed if not string ?)
      QString colorStr = CQChartsUtil::modelString(model, colorInd, ok3);

      //---

      nameValues_[name].emplace_back(x, y, r, xInd1, symbolSizeStr, fontSizeStr, colorStr);
    }
  }

  //---

  QAbstractItemModel *model = this->model();

  QString xname, yname, symbolSizeName, fontSizeName, colorName;

  if (model) {
    xname          = model->headerData(xColumn         (), Qt::Horizontal).toString();
    yname          = model->headerData(yColumn         (), Qt::Horizontal).toString();
    symbolSizeName = model->headerData(symbolSizeColumn(), Qt::Horizontal).toString();
    fontSizeName   = model->headerData(fontSizeColumn  (), Qt::Horizontal).toString();
    colorName      = model->headerData(colorColumn     (), Qt::Horizontal).toString();
  }

  if (xname          == "") xname          = "x";
  if (yname          == "") yname          = "y";
  if (symbolSizeName == "") symbolSizeName = "symbolSize";
  if (fontSizeName   == "") fontSizeName   = "fontSize";
  if (colorName      == "") colorName      = "color";

  //double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  //double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int nv = nameValues_.size();

  int r = 0;

  for (const auto &nameValues : nameValues_) {
    bool hidden = isSetHidden(r);

    if (! hidden) {
      const QString &name   = nameValues.first;
      const Values  &values = nameValues.second;

      int nv1 = values.size();

      for (int j = 0; j < nv1; ++j) {
        const Point &valuePoint = values[j];

        const QPointF &p = valuePoint.p;

        double symbolSize = this->symbolSize();

        if (symbolSizeColumn() >= 0)
          symbolSize = symbolSizeSet_.imap(valuePoint.i);

        OptReal fontSize, color;

        if (fontSizeColumn() >= 0)
          fontSize = fontSizeSet_.imap(valuePoint.i);

        if (colorColumn() >= 0)
          color = colorSet_.imap(valuePoint.i);

        double sw = pixelToWindowWidth (symbolSize);
        double sh = pixelToWindowHeight(symbolSize);

        CQChartsGeom::BBox bbox(p.x() - sw, p.y() - sh, p.x() + sw, p.y() + sh);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, bbox, p, symbolSize, fontSize, color, r, nv);

        //---

        QString id = name;

        id += QString("\n  %1\t%2").arg(xname).arg(p.x());
        id += QString("\n  %1\t%2").arg(yname).arg(p.y());

        if (valuePoint.symbolSizeStr != "")
          id += QString("\n  %1\t%2").arg(symbolSizeName).arg(valuePoint.symbolSizeStr);

        if (valuePoint.fontSizeStr != "")
          id += QString("\n  %1\t%2").arg(fontSizeName).arg(valuePoint.fontSizeStr);

        if (valuePoint.colorStr != "")
          id += QString("\n  %1\t%2").arg(colorName).arg(valuePoint.colorStr);

        pointObj->setId(id);

        //---

        pointObj->setName(name);

        pointObj->setInd(valuePoint.ind);

        addPlotObject(pointObj);
      }
    }

    ++r;
  }

  //---

  resetKeyItems();
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsKey *key)
{
  int nv = nameValues_.size();

  int i = 0;

  for (const auto &nameValue: nameValues_) {
    const QString &name = nameValue.first;

    CQChartsScatterKeyColor *color = new CQChartsScatterKeyColor(this, i, nv);
    CQChartsKeyText         *text  = new CQChartsKeyText        (this, name);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);

    ++i;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsScatterPlot::
drawDataLabel(QPainter *p, const QRectF &qrect, const QString &str, double fontSize)
{
  if (fontSize > 0) {
    QFont font = dataLabel_.font();

    QFont font1 = font;

    font1.setPointSizeF(fontSize);

    dataLabel_.setFont(font1);

    dataLabel_.draw(p, qrect, str);

    dataLabel_.setFont(font);
  }
  else {
    dataLabel_.draw(p, qrect, str);
  }
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CQChartsGeom::BBox &rect, const QPointF &p,
                        double symbolSize, const OptReal &fontSize, const OptReal &color,
                        int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), p_(p), symbolSize_(symbolSize), fontSize_(fontSize),
 color_(color), i_(i), n_(n)
{
}

bool
CQChartsScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  double s = symbolSize_; // TODO: ensure not a crazy number

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
mousePress(const CQChartsGeom::Point &)
{
  plot_->beginSelect();

  plot_->addSelectIndex(ind_.row(), plot_->xColumn());
  plot_->addSelectIndex(ind_.row(), plot_->yColumn());

  plot_->endSelect();
}

bool
CQChartsScatterPointObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsScatterPointObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  double s = symbolSize_; // TODO: ensure not a crazy number

  QColor color;

  if (color_)
    color = plot_->interpPaletteColor(*color_, Qt::blue);
  else
    color = plot_->paletteColor(i_, n_, Qt::blue);

  QBrush brush(color);
  QPen   pen  (plot_->symbolBorderColor());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  p->setPen  (pen);
  p->setBrush(brush);

  QRectF erect(px - s, py - s, 2*s, 2*s);

  p->drawEllipse(erect);

  if (plot_->isTextLabels()) {
    plot_->drawDataLabel(p, erect, name_, (fontSize_ ? *fontSize_ : -1));
  }
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsScatterKeyColor::
mousePress(const CQChartsGeom::Point &)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsScatterKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}