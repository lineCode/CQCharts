#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRotatedText.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsPiePlotType::
CQChartsPiePlotType()
{
  // name, desc, propName, attributes, default
  addColumnParameter("label"   , "Label"    , "labelColumn"   , "", 0);
  addColumnParameter("data"    , "Data"     , "dataColumn"    , "", 1);
  addColumnParameter("keyLabel", "Key Label", "keyLabelColumn", "optional");
}

CQChartsPlot *
CQChartsPiePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsPiePlot(view, model);
}

//---

CQChartsPiePlot::
CQChartsPiePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("pie"), model), textBox_(this)
{
  setLayerActive(Layer::FG, true);

  addKey();

  addTitle();
}

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty(""       , this, "donut"                      );
  addProperty(""       , this, "innerRadius"                );
  addProperty(""       , this, "labelRadius"                );
  addProperty(""       , this, "explodeSelected"            );
  addProperty(""       , this, "startAngle"                 );
  addProperty(""       , this, "rotatedText"                );
  addProperty("columns", this, "labelColumn"    , "label"   );
  addProperty("columns", this, "dataColumn"     , "data"    );
  addProperty("columns", this, "keyLabelColumn" , "keyLabel");

  addProperty("label", &textBox_, "visible");
  addProperty("label", &textBox_, "font"   );
  addProperty("label", &textBox_, "color"  );

  QString labelBoxPath = id() + "/label/box";

  textBox_.CQChartsBoxObj::addProperties(propertyView(), labelBoxPath);
}

void
CQChartsPiePlot::
updateRange()
{
  double r = 1.0;

  r = std::max(r, labelRadius());

  double xr = r;
  double yr = r;

  if (isEqualScale()) {
    double aspect = view()->aspect();

    if (aspect > 1.0)
      xr *= aspect;
    else
      yr *= 1.0/aspect;
  }

  dataRange_.reset();

  dataRange_.updateRange(-xr, -yr);
  dataRange_.updateRange( xr,  yr);

  applyDataRange();
}

void
CQChartsPiePlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //---

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  double xc = 0.0;
  double yc = 0.0;
  double r  = 0.90;

  setInnerRadius(0.6*r);

  //---

  double angle1 = startAngle();

  int n = model->rowCount(QModelIndex());

  //---

  double total = 0.0;

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    QModelIndex yind = model->index(i, dataColumn());

    bool ok;

    double value = CQChartsUtil::modelReal(model, yind, ok);

    if (! ok)
      value = i;

    if (CQChartsUtil::isNaN(value))
      continue;

    total += value;
  }

  //---

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    QModelIndex xind = model->index(i, labelColumn());
    QModelIndex yind = model->index(i, dataColumn ());

    bool ok1, ok2;

    QString name  = CQChartsUtil::modelString(model, xind, ok1);
    double  value = CQChartsUtil::modelReal  (model, yind, ok2);

    if (! ok2) value = i;

    if (CQChartsUtil::isNaN(value))
      continue;

    double angle  = 360.0*value/total;
    double angle2 = angle1 - angle;

    //---

    CBBox2D rect(xc - r, yc - r, xc + r, yc + r);

    CQChartsPieObj *obj = new CQChartsPieObj(this, rect, i, n);

    obj->setId(QString("%1:%2").arg(name).arg(columnStr(dataColumn(), value)));

    obj->setCenter(CPoint2D(xc, yc));
    obj->setRadius(r);
    obj->setAngle1(angle1);
    obj->setAngle2(angle2);
    obj->setName  (name);
    obj->setValue (value);

    addPlotObject(obj);

    //---

    angle1 = angle2;
  }

  //---

  keyObj_->clearItems();

  addKeyItems(keyObj_);
}

void
CQChartsPiePlot::
addKeyItems(CQChartsKey *key)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int labelColumn = keyLabelColumn();

  if (labelColumn < 0)
    labelColumn = this->labelColumn();

  int n = model->rowCount(QModelIndex());

  for (int i = 0; i < n; ++i) {
    QModelIndex xind = model->index(i, labelColumn);

    bool ok;

    QString name = CQChartsUtil::modelString(model, xind, ok);

    CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, i, n);
    CQChartsPieKeyText  *text  = new CQChartsPieKeyText (this, i, name);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsPiePlot::
handleResize()
{
  dataRange_.reset();

  CQChartsPlot::handleResize();
}

void
CQChartsPiePlot::
draw(QPainter *p)
{
  initObjs();

  //---

//contentsBBox_ = CBBox2D(-1, -1, 1, 1);

  drawParts(p);
}

//------

CQChartsPieObj::
CQChartsPieObj(CQChartsPiePlot *plot, const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), i_(i), n_(n)
{
}

bool
CQChartsPieObj::
inside(const CPoint2D &p) const
{
  double r = p.distanceTo(center());

  double ir = 0.0;

  if (plot_->isDonut())
    ir = plot_->innerRadius()*radius();

  if (r < ir || r > radius())
    return false;

  //---

  // check angle
  double a = CQChartsUtil::Rad2Deg(atan2(p.y - center().y, p.x - center().x));

  while (a < 0) a += 360.0;

  double a1 = angle1(); while (a1 < 0) a1 += 360.0;
  double a2 = angle2(); while (a2 < 0) a2 += 360.0;

  if (a1 < a2) {
    // crosses zero
    if (a >= 0 && a <= a1)
      return true;

    if (a <= 360 && a >= a2)
      return true;
  }
  else {
    if (a >= a2 && a <= a1)
      return true;
  }

  return false;
}

void
CQChartsPieObj::
draw(QPainter *p, const CQChartsPlot::Layer &layer)
{
  CPoint2D c  = center();
  double   r  = radius();
  double   a1 = angle1();
  double   a2 = angle2();

  //---

  bool exploded = isExploded();

  if (isSelected() && plot_->isExplodeSelected())
    exploded = true;

  if (exploded) {
    double angle = CQChartsUtil::Deg2Rad((a1 + a2)/2.0);

    double dx = 0.1*radius()*cos(angle);
    double dy = 0.1*radius()*sin(angle);

    c.x += dx;
    c.y += dy;
  }

  //---

  //double ir = plot_->innerRadius()*radius();

  CPoint2D pc;

  plot_->windowToPixel(c, pc);

  //---

  CBBox2D bbox(c.x - r, c.y - r, c.x + r, c.y + r);

  CBBox2D pbbox;

  plot_->windowToPixel(bbox, pbbox);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    QPainterPath path;

    if (plot_->isDonut()) {
      double ir = plot_->innerRadius();

      CBBox2D bbox1(c.x - ir, c.y - ir, c.x + ir, c.y + ir);

      CBBox2D pbbox1;

      plot_->windowToPixel(bbox1, pbbox1);

      //---

      double ra1 = a1*M_PI/180.0;
      double ra2 = a2*M_PI/180.0;

      double x1 = c.x + ir*cos(ra1);
      double y1 = c.y + ir*sin(ra1);
      double x2 = c.x +  r*cos(ra1);
      double y2 = c.y +  r*sin(ra1);

      double x3 = c.x + ir*cos(ra2);
      double y3 = c.y + ir*sin(ra2);
      double x4 = c.x +  r*cos(ra2);
      double y4 = c.y +  r*sin(ra2);

      double px1, py1, px2, py2, px3, py3, px4, py4;

      plot_->windowToPixel(x1, y1, px1, py1);
      plot_->windowToPixel(x2, y2, px2, py2);
      plot_->windowToPixel(x3, y3, px3, py3);
      plot_->windowToPixel(x4, y4, px4, py4);

      path.moveTo(px1, py1);
      path.lineTo(px2, py2);

      path.arcTo(CQChartsUtil::toQRect(pbbox), a1, a2 - a1);

      path.lineTo(px4, py4);
      path.lineTo(px3, py3);

      path.arcTo(CQChartsUtil::toQRect(pbbox1), a2, a1 - a2);
    }
    else {
      double a21 = a2 - a1;

      if (std::abs(a21) < 360.0) {
        path.moveTo(QPointF(pc.x, pc.y));

        path.arcTo(CQChartsUtil::toQRect(pbbox), a1, a2 - a1);
      }
      else {
        path.addEllipse(CQChartsUtil::toQRect(pbbox));
      }
    }

    path.closeSubpath();

    //---

    QColor bg = plot_->objectColor(this, i_, n_);
    QColor fg = plot_->textColor(bg);

    p->setBrush(bg);
    p->setPen  (fg);

    p->drawPath(path);
  }

  //---

  if (layer == CQChartsPlot::Layer::FG && name() != "") {
    double a21 = a2 - a1;

    // if full circle always draw text at center
    if (CQChartsUtil::realEq(std::abs(a21), 360.0)) {
      plot_->textBox().draw(p, CQChartsUtil::toQPoint(pc), name(), 0.0);
    }
    // draw on arc center line
    else {
      double ir = plot_->innerRadius();
      double lr = plot_->labelRadius();

      double ta = (a1 + a2)/2.0;

      double tangle = CQChartsUtil::Deg2Rad(ta);

      double lr1;

      if (plot_->isDonut())
        lr1 = ir + lr*(radius() - ir);
      else
        lr1 = lr*radius();

      if (lr1 < 0.01)
        lr1 = 0.01;

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = c.x + lr1*tc;
      double ty = c.y + lr1*ts;

      double ptx, pty;

      plot_->windowToPixel(tx, ty, ptx, pty);

      //---

      double        dx    = 0.0;
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      if (lr1 > radius()) {
        double lx1 = c.x + radius()*tc;
        double ly1 = c.y + radius()*ts;
        double lx2 = c.x + lr1*tc;
        double ly2 = c.y + lr1*ts;

        double lpx1, lpy1, lpx2, lpy2;

        plot_->windowToPixel(lx1, ly1, lpx1, lpy1);
        plot_->windowToPixel(lx2, ly2, lpx2, lpy2);

        int tickSize = 16;

        if (tc >= 0) {
          dx    = tickSize;
          align = Qt::AlignLeft | Qt::AlignVCenter;
        }
        else {
          dx    = -tickSize;
          align = Qt::AlignRight | Qt::AlignVCenter;
        }

        QColor bg = plot_->objectColor(this, i_, n_);

        p->setPen(bg);

        p->drawLine(lpx1, lpy1, lpx2     , lpy2);
        p->drawLine(lpx2, lpy2, lpx2 + dx, lpy2);
      }

      //---

      QPointF pt = QPointF(ptx + dx, pty);

      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      plot_->textBox().draw(p, pt, name(), angle, align);

      CBBox2D tbbox;

      plot_->pixelToWindow(CQChartsUtil::fromQRect(plot_->textBox().rect()), tbbox);

      plot_->setContentsBBox(plot_->contentsBBox() + tbbox);
    }
  }
}

//------

CQChartsPieKeyColor::
CQChartsPieKeyColor(CQChartsPiePlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsPieKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

QColor
CQChartsPieKeyColor::
fillColor() const
{
  QColor c = CQChartsKeyColorBox::fillColor();

  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}

//------

CQChartsPieKeyText::
CQChartsPieKeyText(CQChartsPiePlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsPieKeyText::
textColor() const
{
  QColor c = CQChartsKeyText::textColor();

  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}

//------

CQChartsPieTextObj::
CQChartsPieTextObj(CQChartsPiePlot *plot) :
 plot_(plot)
{
}

void
CQChartsPieTextObj::
redrawBoxObj()
{
  plot_->update();
}

void
CQChartsPieTextObj::
draw(QPainter *p, const QPointF &c, const QString &text, double angle,
     Qt::Alignment align) const
{
  p->save();

  QFontMetrics fm(font());

  int tw = fm.width(text);

  double tw1 = tw + 2*margin();
  double th1 = fm.height() + 2*margin();

  double cx = c.x();
  double cy = c.y() - th1/2;
  double cd = 0.0;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
    cd  = -margin();
  }
  else {
    cd  = margin();
  }

  rect_ = QRectF(cx, cy, tw1, th1);

  //---

  p->setFont(font());
  p->setPen(color());

  if (CQChartsUtil::isZero(angle)) {
    CQChartsBoxObj::draw(p, rect_);
  }
  else {
    QRectF                bbox;
    CQRotatedText::Points points;

    CQRotatedText::bboxData(c.x(), c.y(), text, font(), angle, margin(),
                            bbox, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(p, poly);
  }

  p->setPen(color());

  CQRotatedText::drawRotatedText(p, c.x() + cd, c.y(), text, angle,
                                 align, /*alignBBox*/ true);

  p->restore();
}
