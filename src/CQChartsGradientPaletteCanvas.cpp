#include <CQChartsGradientPaletteCanvas.h>
#include <CQChartsUtil.h>

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

namespace Util {
  inline double norm(double x, double low, double high) {
    return (x - low)/(high - low);
  }

  inline double lerp(double value1, double value2, double amt) {
    return value1 + (value2 - value1)*amt;
  }

  inline double map(double value, double low1, double high1, double low2, double high2) {
    return lerp(low2, high2, norm(value, low1, high1));
  }

  inline double rgbToGray(double r, double g, double b) {
    return r*0.3 + g*0.59 + b*0.11;
  }
}

//------

CQChartsGradientPaletteCanvas::
CQChartsGradientPaletteCanvas(QWidget *parent, CQChartsGradientPalette *palette) :
 QFrame(parent), palette_(palette)
{
  init();
}

CQChartsGradientPaletteCanvas::
CQChartsGradientPaletteCanvas(CQChartsGradientPalette *palette, QWidget *parent) :
 QFrame(parent), palette_(palette)
{
  init();
}

CQChartsGradientPaletteCanvas::
~CQChartsGradientPaletteCanvas()
{
}

void
CQChartsGradientPaletteCanvas::
init()
{
  setObjectName("palette");

  setMouseTracking(true);

  //gradientPalette()->addDefinedColor(0, QColor(0,0,0));
  //gradientPalette()->addDefinedColor(1, QColor(255,255,255));

  tipText_ = new QLabel(this);

  tipText_->setAutoFillBackground(true);
}

void
CQChartsGradientPaletteCanvas::
setGradientPalette(CQChartsGradientPalette *palette)
{
  palette_ = palette;

  update();
}

void
CQChartsGradientPaletteCanvas::
enterEvent(QEvent *)
{
}

void
CQChartsGradientPaletteCanvas::
leaveEvent(QEvent *)
{
  hideTipText();
}

void
CQChartsGradientPaletteCanvas::
mousePressEvent(QMouseEvent *)
{
  pressed_ = true;
}

void
CQChartsGradientPaletteCanvas::
mouseMoveEvent(QMouseEvent *me)
{
  int x = me->pos().x();
  int y = me->pos().y();

  double wx, wy;

  pixelToWindow(x, y, wx, wy);

  if (wx < 0.0 || wx > 1.0) {
    hideTipText();
    return;
  }

  QColor bg = gradientPalette()->getColor(wx);
  QColor fg = CQChartsUtil::bwColor(bg);

  tipText_->setText(QString("%1,%2").arg(wx).arg(wy));

  QPalette palette = tipText_->palette();

  palette.setColor(tipText_->backgroundRole(), bg);
  palette.setColor(tipText_->foregroundRole(), fg);

  tipText_->setPalette(palette);

  showTipText();
}

void
CQChartsGradientPaletteCanvas::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
}

void
CQChartsGradientPaletteCanvas::
showTipText()
{
  tipText_->setVisible(true);

  tipText_->resize(tipText_->sizeHint());

  tipText_->move(width() - tipText_->width(), height() - tipText_->height());

  tipText_->raise();
}

void
CQChartsGradientPaletteCanvas::
hideTipText()
{
  tipText_->setVisible(false);
}


void
CQChartsGradientPaletteCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  painter.fillRect(rect(), Qt::white);

  //---

  drawAxis(&painter);

  //---

  // draw graph
  QPen redPen  (Qt::red  ); redPen  .setWidth(0);
  QPen greenPen(Qt::green); greenPen.setWidth(0);
  QPen bluePen (Qt::blue ); bluePen .setWidth(0);
  QPen blackPen(Qt::black); blackPen.setWidth(0);

  QPainterPath redPath, greenPath, bluePath, blackPath;

  double px1, py1, px2, py2;

  windowToPixel(0.0, 0.0, px1, py1);
  windowToPixel(1.0, 1.0, px2, py2);

  bool   first = true;
//double r1 = 0.0, g1 = 0.0, b1 = 0.0, m1 = 0.0, x1 = 0.0;

  for (double x = px1; x <= px2; x += 1.0) {
    double wx, wy;

    pixelToWindow(x, 0, wx, wy);

    QColor c = gradientPalette()->getColor(wx);

    double x2 = wx;

    double r2 = 0.0, g2 = 0.0, b2 = 0.0, m2 = 0.0;

    if (gradientPalette()->colorModel() == CQChartsGradientPalette::ColorModel::HSV) {
      r2 = c.hueF       ();
      g2 = c.saturationF();
      b2 = c.valueF     ();
      m2 = c.valueF     ();
    }
    else {
      r2 = c.redF  ();
      g2 = c.greenF();
      b2 = c.blueF ();
      m2 = Util::rgbToGray(r2, g2, b2);
    }

    double px, py;

    if (first) {
      windowToPixel(x2, r2, px, py); redPath  .moveTo(px, py);
      windowToPixel(x2, g2, px, py); greenPath.moveTo(px, py);
      windowToPixel(x2, b2, px, py); bluePath .moveTo(px, py);
      windowToPixel(x2, m2, px, py); blackPath.moveTo(px, py);
    }
    else {
      windowToPixel(x2, r2, px, py); redPath  .lineTo(px, py);
      windowToPixel(x2, g2, px, py); greenPath.lineTo(px, py);
      windowToPixel(x2, b2, px, py); bluePath .lineTo(px, py);
      windowToPixel(x2, m2, px, py); blackPath.lineTo(px, py);
    }

//  x1 = x2; r1 = r2; g1 = g2; b1 = b2; m1 = m2;

    first = false;
  }

  painter.strokePath(redPath  , redPen  );
  painter.strokePath(greenPath, greenPen);
  painter.strokePath(bluePath , bluePen );
  painter.strokePath(blackPath, blackPen);

  //---

  // draw color bar
  double xp1 = 1.05;
  double xp2 = 1.15;

  double pxp1, pxp2;

  windowToPixel(xp1, 0.0, pxp1, py1);
  windowToPixel(xp2, 1.0, pxp2, py2);

  for (double y = py2; y <= py1; y += 1.0) {
    double wx, wy;

    pixelToWindow(0, y, wx, wy);

    QColor c = gradientPalette()->getColor(wy);

    QPen pen(c); pen.setWidth(0);

    painter.setPen(pen);

    painter.drawLine(QPointF(pxp1, y), QPointF(pxp2, y));
  }

  painter.setPen(blackPen);

  painter.drawLine(QPointF(pxp1, py1), QPointF(pxp2, py1));
  painter.drawLine(QPointF(pxp2, py1), QPointF(pxp2, py2));
  painter.drawLine(QPointF(pxp2, py2), QPointF(pxp1, py2));
  painter.drawLine(QPointF(pxp1, py2), QPointF(pxp1, py1));

  //---

  if (gradientPalette()->colorType() == CQChartsGradientPalette::ColorType::DEFINED) {
    for (const auto &c : gradientPalette()->colors()) {
      double x  = gradientPalette()->mapDefinedColorX(c.first);
      QColor c1 = c.second;

      if (gradientPalette()->colorModel() == CQChartsGradientPalette::ColorModel::HSV) {
        drawSymbol(&painter, x, c1.hueF       (), redPen  );
        drawSymbol(&painter, x, c1.saturationF(), greenPen);
        drawSymbol(&painter, x, c1.valueF     (), bluePen );
      }
      else {
        drawSymbol(&painter, x, c1.redF  (), redPen  );
        drawSymbol(&painter, x, c1.greenF(), greenPen);
        drawSymbol(&painter, x, c1.blueF (), bluePen );
      }
    }
  }
}

void
CQChartsGradientPaletteCanvas::
drawAxis(QPainter *painter)
{
  QPen blackPen(Qt::black); blackPen.setWidth(0);

  double px1, py1, px2, py2;

  windowToPixel(0.0, 0.0, px1, py1);
  windowToPixel(1.0, 1.0, px2, py2);

  drawLine(painter, 0, 0, 1, 0, blackPen);
  drawLine(painter, 0, 0, 0, 1, blackPen);

  painter->setPen(blackPen);

  painter->drawLine(QPointF(px1, py1), QPointF(px1, py1 + 4));
  painter->drawLine(QPointF(px2, py1), QPointF(px2, py1 + 4));

  painter->drawLine(QPointF(px1, py1), QPointF(px1 - 4, py1));
  painter->drawLine(QPointF(px1, py2), QPointF(px1 - 4, py2));

  QFontMetricsF fm(font());

  double tw  = fm.width("X.X");
  double dty = (fm.ascent() - fm.descent())/2;

  painter->drawText(QPointF(px1 - tw - 4, py1 + dty), "0.0");
  painter->drawText(QPointF(px1 - tw - 4, py2 + dty), "1.0");
}

void
CQChartsGradientPaletteCanvas::
drawLine(QPainter *painter, double x1, double y1, double x2, double y2, const QPen &pen)
{
  painter->setPen(pen);

  double px1, py1, px2, py2;

  windowToPixel(x1, y1, px1, py1);
  windowToPixel(x2, y2, px2, py2);

  painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));
}

void
CQChartsGradientPaletteCanvas::
drawSymbol(QPainter *painter, double x, double y, const QPen &pen)
{
  painter->setPen(pen);

  double px, py;

  windowToPixel(x, y, px, py);

  painter->drawLine(QPointF(px - 4, py), QPointF(px + 4, py));
  painter->drawLine(QPointF(px, py - 4), QPointF(px, py + 4));
}

void
CQChartsGradientPaletteCanvas::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  px = Util::map(wx, -margin_.left  , 1 + margin_.right, 0, width () - 1);
  py = Util::map(wy, -margin_.bottom, 1 + margin_.top  , height() - 1, 0);
}

void
CQChartsGradientPaletteCanvas::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  wx = Util::map(px, 0, width () - 1, -margin_.left  , 1 + margin_.right);
  wy = Util::map(py, height() - 1, 0, -margin_.bottom, 1 + margin_.top  );
}

QSize
CQChartsGradientPaletteCanvas::
sizeHint() const
{
  return QSize(600, 600);
}