#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsEditHandles.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QScrollBar>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionSlider>

CQChartsKey::
CQChartsKey(CQChartsView *view) :
 CQChartsBoxObj(view),
 CQChartsObjTextData      <CQChartsKey>(this),
 CQChartsObjHeaderTextData<CQChartsKey>(this)
{
  setObjectName("key");

  setStateColoring(false);

  setHeaderTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
}

CQChartsKey::
CQChartsKey(CQChartsPlot *plot) :
 CQChartsBoxObj(plot),
 CQChartsObjTextData      <CQChartsKey>(this),
 CQChartsObjHeaderTextData<CQChartsKey>(this)
{
  setObjectName("key");

  setFilled(false);

  setHeaderTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
}

QString
CQChartsKey::
calcId() const
{
  if      (view_)
    return view_->id() + "/key";
  else if (plot_)
    return plot_->id() + "/key";
  else
    return "key";
}

void
CQChartsKey::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() {
    if      (view_)
      view_->update();
    else if (plot_)
      plot_->queueDrawObjs();
  } );
}

CQChartsKey::
~CQChartsKey()
{
}

void
CQChartsKey::
setLocation(const CQChartsKeyLocation &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { updatePosition(); } );
}

//---

void
CQChartsKey::
draw(QPainter *) const
{
}

//------

CQChartsViewKey::
CQChartsViewKey(CQChartsView *view) :
 CQChartsKey(view)
{
}

CQChartsViewKey::
~CQChartsViewKey()
{
}

void
CQChartsViewKey::
updatePosition(bool /*queued*/)
{
  redraw();
}

void
CQChartsViewKey::
updateLayout()
{
  redraw();
}

void
CQChartsViewKey::
doLayout()
{
  QFont font = view()->viewFont(textFont());

  QFontMetricsF fm(font);

  double bs = fm.height() + 4.0;

  //----

  double x = 0.0, y = 0.0;
//double dx = 0.0, dy = 0.0;

  if      (location().onLeft   ()) { x =   0.0; /*dx =  1.0; */ }
  else if (location().onHCenter()) { x =  50.0; /*dx =  0.0; */ }
  else if (location().onRight  ()) { x = 100.0; /*dx = -1.0; */ }

  if      (location().onTop    ()) { y = 100.0; /*dy =  1.0; */ }
  else if (location().onVCenter()) { y =  50.0; /*dy =  0.0; */ }
  else if (location().onBottom ()) { y =   0.0; /*dy = -1.0; */ }

  //----

  double px, py;

  view_->windowToPixel(x, y, px, py);

  //px += dx*bs;
  //py += dy*bs;

  //---

  double pw = 0.0;
  double ph = 0.0;

  int n = view_->numPlots();

  for (int i = 0; i < n; ++i) {
    CQChartsPlot *plot = view_->plot(i);

    QString name = plot->keyText();

    double tw = fm.width(name) + bs + margin();

    pw = std::max(pw, tw);

    ph += bs;
  }

  size_ = QSizeF(pw + 2*margin(), ph + 2*margin()  + (n - 1)*2);

  double pxr = 0.0, pyr = 0.0;

  if      (location().onLeft   ()) pxr = px                   + margin();
  else if (location().onHCenter()) pxr = px - size_.width()/2;
  else if (location().onRight  ()) pxr = px - size_.width()   - margin();

  if      (location().onTop    ()) pyr = py                    + margin();
  else if (location().onVCenter()) pyr = py - size_.height()/2;
  else if (location().onBottom ()) pyr = py - size_.height()   - margin();

  position_ = QPointF(pxr, pyr);
}

void
CQChartsViewKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"   )->setDesc("Is visible");
  model->addProperty(path, this, "selected"  )->setDesc("Is selected");
  model->addProperty(path, this, "horizontal")->setDesc("Draw items horizontally");
  model->addProperty(path, this, "autoHide"  )->setDesc("Auto hide key when too large");
  model->addProperty(path, this, "clipped"   )->setDesc("Clip key to view");

  model->addProperty(path, this, "location")->setDesc("Key placement location");

  model->addProperty(path, this, "interactive"  )->setDesc("Key supports click");
  model->addProperty(path, this, "pressBehavior")->setDesc("Key click behavior");

  model->addProperty(path, this, "hiddenAlpha")->setDesc("Alpha for hidden items");

  //---

  // header text
  QString headerPath = path + "/header";

  model->addProperty(headerPath, this, "header"             , "text"     )->
                      setDesc("Header text");
  model->addProperty(headerPath, this, "headerTextColor"    , "color"    )->
                      setDesc("Header text color");
  model->addProperty(headerPath, this, "headerTextAlpha"    , "alpha"    )->
                      setDesc("Header text alpha");
  model->addProperty(headerPath, this, "headerTextFont"     , "font"     )->
                      setDesc("Header text font");
  model->addProperty(headerPath, this, "headerTextAngle"    , "angle"    )->
                      setDesc("Header text angle");
  model->addProperty(headerPath, this, "headerTextContrast" , "contrast" )->
                      setDesc("Header text contrast");
  model->addProperty(headerPath, this, "headerTextAlign"    , "align"    )->
                      setDesc("Header text align");
  model->addProperty(headerPath, this, "headerTextFormatted", "formatted")->
                      setDesc("Header text formatted to box");
  model->addProperty(headerPath, this, "headerTextScaled"   , "scaled"   )->
                      setDesc("Header text scaled to box");
  model->addProperty(headerPath, this, "headerTextHtml"     , "html"     )->
                      setDesc("Header text is html");

  //---

  // border, fill
  CQChartsBoxObj::addProperties(model, path);

  //---

  // key text
  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textColor", "color")->setDesc("Text color");
  model->addProperty(textPath, this, "textAlpha", "alpha")->setDesc("Text alpha");
  model->addProperty(textPath, this, "textFont" , "font" )->setDesc("Text font");
  model->addProperty(textPath, this, "textAlign", "align")->setDesc("Text align");
}

void
CQChartsViewKey::
draw(QPainter *painter) const
{
  if (! isVisible())
    return;

  //---

  CQChartsViewKey *th = const_cast<CQChartsViewKey *>(this);

  th->doLayout();

  //---

  // pixel position & size (TODO: using view/units)
  double px = position_.x(); // left
  double py = position_.y(); // top

  double pw = size_.width ();
  double ph = size_.height();

  //vbbox_ = CQChartsGeom::BBox(px, py - ph, px + pw, py);

  //---

  double x1, y1, x2, y2;

  view_->pixelToWindow(px     , py     , x1, y1);
  view_->pixelToWindow(px + pw, py + ph, x2, y2);

  pbbox_ = CQChartsGeom::BBox(x1, y2, x2, y1);

  //---

  QRectF rect(px, py, pw, ph);

  //---

  CQChartsBoxObj::draw(painter, rect);

  //---

  view()->setPainterFont(painter, textFont());

  QFontMetricsF fm(painter->font());

  double px1 = px + margin();
  double py1 = py + margin();

  int n = view_->numPlots();

  double bs = fm.height() + 4.0;

  double dth = (bs - fm.height())/2;

  for (int i = 0; i < n; ++i) {
    double py2 = py1 + bs + 2;

    CQChartsPlot *plot = view_->plot(i);

    bool checked = plot->isVisible();

    //---

    drawCheckBox(painter, px1, (py1 + py2)/2.0 - bs/2.0, bs, checked);

    //---

    painter->setPen(interpTextColor(0, 1));

    QString name = plot->keyText();

    double px2 = px1 + bs + margin();

    //double tw = fm.width(name);

    CQChartsDrawUtil::drawSimpleText(painter, px2, py1 + fm.ascent() + dth, name);

    //---

    // save view key item (plot) rect
    double x1, y1, x2, y2;

    view_->pixelToWindow(px     , py1, x1, y1);
    view_->pixelToWindow(px + pw, py2, x2, y2);

    CQChartsGeom::BBox prect(x1, y2, x2, y1);

    prects_.push_back(prect);

    //---

    py1 = py2;
  }
}

void
CQChartsViewKey::
drawCheckBox(QPainter *painter, double x, double y, int bs, bool checked) const
{
  QImage cimage(QSize(bs, bs), QImage::Format_ARGB32);

  cimage.fill(QColor(0,0,0,0));

  //QRectF qrect(x, y, bs, bs);
  QRectF qrect(0, 0, bs, bs);

  QStylePainter spainter(&cimage, view_);

  spainter.setPen(interpTextColor(0, 1));

  QStyleOptionButton opt;

  opt.initFrom(view_);

  opt.rect = qrect.toRect();

  opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

  spainter.drawControl(QStyle::CE_CheckBox, opt);

  painter->drawImage(x, y, cimage);

  //painter->drawRect(qrect);
}

bool
CQChartsViewKey::
isInside(const CQChartsGeom::Point &w) const
{
  return pbbox_.inside(w);
}

void
CQChartsViewKey::
selectPress(const CQChartsGeom::Point &w, CQChartsSelMod selMod)
{
  int n = std::min(view_->numPlots(), int(prects_.size()));

  for (int i = 0; i < n; ++i) {
    if (! prects_[i].inside(w))
      continue;

    if      (pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW)
      doShow(i, selMod);
    else if (pressBehavior() == CQChartsKeyPressBehavior::Type::SELECT)
      doSelect(i, selMod);

    break;
  }

  redraw();
}

void
CQChartsViewKey::
doShow(int i, CQChartsSelMod selMod)
{
  CQChartsPlot *plot = view_->plot(i);

  if      (selMod == CQChartsSelMod::REPLACE) {
    CQChartsView::Plots plots;

    view_->getPlots(plots);

    for (auto &plot1 : plots)
      plot->setVisible(plot1 == plot);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setVisible(true);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setVisible(false);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setVisible(! plot->isVisible());
}

void
CQChartsViewKey::
doSelect(int, CQChartsSelMod)
{
}

void
CQChartsViewKey::
redraw(bool /*queued*/)
{
  view_->update();
}

//------

CQChartsPlotKey::
CQChartsPlotKey(CQChartsPlot *plot) :
 CQChartsKey(plot)
{
  autoHide_ = false;

  //---

  editHandles_ = new CQChartsEditHandles(plot, CQChartsEditHandles::Mode::MOVE);

  setBorder(true);

  clearItems();

  //---

  // create scroll bar
  scrollData_.hbar = new QScrollBar(Qt::Horizontal, plot->view());
  scrollData_.vbar = new QScrollBar(Qt::Vertical  , plot->view());

  scrollData_.hbar->hide();
  scrollData_.vbar->hide();

  connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
  connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));

  scrollData_.pixelBarSize = view()->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;
}

CQChartsPlotKey::
~CQChartsPlotKey()
{
  delete editHandles_;

  for (auto &item : items_)
    delete item;
}

//---

void
CQChartsPlotKey::
hscrollSlot(int v)
{
  scrollData_.hpos = v;

  redraw(/*queued*/ false);
}

void
CQChartsPlotKey::
vscrollSlot(int v)
{
  scrollData_.vpos = v;

  redraw(/*queued*/ false);
}

void
CQChartsPlotKey::
redraw(bool queued)
{
  if (queued) {
    plot_->queueDrawBackground();
    plot_->queueDrawForeground();
  }
  else {
    plot_->invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  }
}

void
CQChartsPlotKey::
updateKeyItems()
{
  plot_->resetKeyItems();

  redraw();
}

void
CQChartsPlotKey::
updateLayout()
{
  invalidateLayout();

  redraw();
}

void
CQChartsPlotKey::
updatePosition(bool queued)
{
  plot_->updateKeyPosition();

  redraw(queued);
}

void
CQChartsPlotKey::
updateLocation(const CQChartsGeom::BBox &bbox)
{
  // calc key size
  QSizeF ks = calcSize();

  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

  double kx { 0.0 }, ky { 0.0 };

  if      (location().onLeft()) {
    if (isInsideX())
      kx = bbox.getXMin() + xm;
    else
      kx = bbox.getXMin() - ks.width() - xm;
  }
  else if (location().onHCenter()) {
    kx = bbox.getXMid() - ks.width()/2;
  }
  else if (location().onRight()) {
    if (isInsideX())
      kx = bbox.getXMax() - ks.width() - xm;
    else
      kx = bbox.getXMax() + xm;
  }

  if      (location().onTop()) {
    if (isInsideY())
      ky = bbox.getYMax() - ym;
    else
      ky = bbox.getYMax() + ks.height() + ym;
  }
  else if (location().onVCenter()) {
    ky = bbox.getYMid() - ks.height()/2;
  }
  else if (location().onBottom()) {
    if (isInsideY())
      ky = bbox.getYMin() + ks.height() + ym;
    else {
      ky = bbox.getYMin() - ym;

      CQChartsAxis *xAxis = plot_->xAxis();

      if (xAxis && xAxis->side() == CQChartsAxisSide::Type::BOTTOM_LEFT && xAxis->bbox().isSet())
        ky -= xAxis->bbox().getHeight();
    }
  }

  QPointF kp(kx, ky);

  CQChartsKeyLocation::Type locationType = this->location().type();

  if      (locationType == CQChartsKeyLocation::Type::ABS_POSITION) {
    kp = absPlotPosition();
  }
  else if (locationType == CQChartsKeyLocation::Type::ABS_RECT) {
    kp = absPlotRect().bottomLeft();
  }

  setPosition(kp);
}

void
CQChartsPlotKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"   )->setDesc("Is visible");
  model->addProperty(path, this, "selected"  )->setDesc("Is selected");
  model->addProperty(path, this, "horizontal")->setDesc("Draw items horizontally");
  model->addProperty(path, this, "flipped"   )->setDesc("Draw name value flipped");
  model->addProperty(path, this, "autoHide"  )->setDesc("Auto hide key when too large");
  model->addProperty(path, this, "clipped"   )->setDesc("Clip key to plot");

  model->addProperty(path, this, "above"      )->setDesc("Draw key above plot");
  model->addProperty(path, this, "insideX"    )->setDesc("Key placed inside plot in x direction");
  model->addProperty(path, this, "insideY"    )->setDesc("Key placed inside plot in y direction");
  model->addProperty(path, this, "location"   )->setDesc("Key placement location");
  model->addProperty(path, this, "absPosition")->setDesc("Key placement absolute position");
  model->addProperty(path, this, "absRect"    )->setDesc("Key placement absolute rectangle");

  model->addProperty(path, this, "interactive"  )->setDesc("Key supports click");
  model->addProperty(path, this, "pressBehavior")->setDesc("Key click behavior");

  model->addProperty(path, this, "hiddenAlpha" )->setDesc("Alpha for hidden items");
  model->addProperty(path, this, "maxRows"     )->setDesc("Max rows for key");
  model->addProperty(path, this, "spacing"     )->setDesc("Spacing between rows");
  model->addProperty(path, this, "scrollWidth" )->setDesc("Scrolled fixed width");
  model->addProperty(path, this, "scrollHeight")->setDesc("Scrolled fixed height");

  //---

  // header text
  QString headerPath = path + "/header";

  model->addProperty(headerPath, this, "header"             , "text"     )->
                      setDesc("Header text");
  model->addProperty(headerPath, this, "headerTextColor"    , "color"    )->
                      setDesc("Header text color");
  model->addProperty(headerPath, this, "headerTextAlpha"    , "alpha"    )->
                      setDesc("Header text alpha");
  model->addProperty(headerPath, this, "headerTextFont"     , "font"     )->
                      setDesc("Header text font");
  model->addProperty(headerPath, this, "headerTextAngle"    , "angle"    )->
                      setDesc("Header text angle");
  model->addProperty(headerPath, this, "headerTextContrast" , "contrast" )->
                      setDesc("Header text contrast");
  model->addProperty(headerPath, this, "headerTextAlign"    , "align"    )->
                      setDesc("Header text align");
  model->addProperty(headerPath, this, "headerTextFormatted", "formatted")->
                      setDesc("Header text formatted to box");
  model->addProperty(headerPath, this, "headerTextScaled"   , "scaled"   )->
                      setDesc("Header text scaled to box");
  model->addProperty(headerPath, this, "headerTextHtml"     , "html"     )->
                      setDesc("eader text is html");

  //---

  // border, fill
  CQChartsBoxObj::addProperties(model, path);

  //---

  // key text
  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textColor", "color")->setDesc("Text color");
  model->addProperty(textPath, this, "textAlpha", "alpha")->setDesc("Text alpha");
  model->addProperty(textPath, this, "textFont" , "font" )->setDesc("Text font");
  model->addProperty(textPath, this, "textAlign", "align")->setDesc("Text align");
}

void
CQChartsPlotKey::
invalidateLayout()
{
  needsLayout_ = true;
}

void
CQChartsPlotKey::
clearItems()
{
  for (auto &item : items_)
    delete item;

  items_.clear();

  invalidateLayout();

  maxRow_ = 0;
  maxCol_ = 0;
}

void
CQChartsPlotKey::
addItem(CQChartsKeyItem *item, int row, int col, int nrows, int ncols)
{
  item->setKey(this);

  item->setRow(row);
  item->setCol(col);

  item->setRowSpan(nrows);
  item->setColSpan(ncols);

  items_.push_back(item);

  invalidateLayout();

  maxRow_ = std::max(maxRow_, row + nrows);
  maxCol_ = std::max(maxCol_, col + ncols);
}

void
CQChartsPlotKey::
doLayout()
{
  if (! needsLayout_)
    return;

  needsLayout_ = false;

  //---

  // get items in each cell and dimension of grid
  using ColItems    = std::map<int,Items>;
  using RowColItems = std::map<int,ColItems>;

  RowColItems rowColItems;

  numRows_ = 0;
  numCols_ = 0;

  for (const auto &item : items_) {
    numRows_ = std::max(numRows_, item->row() + item->rowSpan());
    numCols_ = std::max(numCols_, item->col() + item->colSpan());
  }

  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    rowColItems[item->row()][col].push_back(item);
  }

  //---

  // get max number of rows
  int numRows = numRows_;

  // limit rows if height (and this scrolled) not defined
  if (! scrollData_.height.isSet())
    numRows = std::min(numRows, maxRows());

  //---

  // get size of each cell
  rowColCell_.clear();

  for (int r = 0; r < numRows; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      const Items &items = rowColItems[r][c];

      for (const auto &item : items) {
        QSizeF size = item->size();

        double width  = size.width ()/item->colSpan();
        double height = size.height()/item->rowSpan();

        rowColCell_[r][c].width  = std::max(rowColCell_[r][c].width , width );
        rowColCell_[r][c].height = std::max(rowColCell_[r][c].height, height);
      }
    }
  }

  //---

  // get spacing and margin in plot coords
  xs_ = plot_->pixelToWindowWidth (spacing());
  ys_ = plot_->pixelToWindowHeight(spacing());

  xm_ = plot_->pixelToWindowWidth (margin());
  ym_ = plot_->pixelToWindowHeight(margin());

  //---

  // get size of each row and column
  rowHeights_.clear();
  colWidths_ .clear();

  for (int r = 0; r < numRows; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      rowHeights_[r] = std::max(rowHeights_[r], rowColCell_[r][c].height);
      colWidths_ [c] = std::max(colWidths_ [c], rowColCell_[r][c].width );
    }
  }

  //----

  // get header text size
  layoutData_.headerWidth  = 0;
  layoutData_.headerHeight = 0;

  if (headerStr().length()) {
    // set text options
    CQChartsTextOptions textOptions;

    textOptions.html = isHeaderTextHtml();

    // get font
    QFont font = view()->plotFont(plot_, headerTextFont());

    // get text size
    QSizeF tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    layoutData_.headerWidth  = plot_->pixelToWindowWidth (tsize.width ()) + 2*xs_;
    layoutData_.headerHeight = plot_->pixelToWindowHeight(tsize.height()) + 2*ys_;
  }

  //---

  // update cell positions and sizes
  double y = -ym_;

  y -= layoutData_.headerHeight;

  for (int r = 0; r < numRows; ++r) {
    double x = xm_;

    double rh = rowHeights_[r] + 2*ys_;

    for (int c = 0; c < numCols_; ++c) {
      double cw = colWidths_[c] + 2*xs_;

      Cell &cell = rowColCell_[r][c];

      cell.x      = x;
      cell.y      = y;
      cell.width  = cw;
      cell.height = rh;

      x += cell.width;
    }

    y -= rh; // T->B
  }

  //----

  // calc full size (unscrolled)
  double w = 0, h = 0;

  for (int c = 0; c < numCols_; ++c) {
    Cell &cell = rowColCell_[0][c];

    w += cell.width;
  }

  w += 2*xm_;

  for (int r = 0; r < numRows; ++r) {
    Cell &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += 2*ym_ + layoutData_.headerHeight;

  w = std::max(w, layoutData_.headerWidth);

  layoutData_.fullSize = QSizeF(w, h);

  //---

  double vbw = plot_->pixelToWindowWidth (scrollData_.pixelBarSize);
  double hbh = plot_->pixelToWindowHeight(scrollData_.pixelBarSize);

  //---

  // check if key size exceeds plot pixel size (auto hide if needed)
  CQChartsGeom::BBox plotPixelRect = plot_->calcPlotPixelRect();

  double maxPixelWidth  = plotPixelRect.getWidth ()*0.8;
  double maxPixelHeight = plotPixelRect.getHeight()*0.8;

  double pixelWidth  = plot_->windowToPixelWidth (layoutData_.fullSize.width ());
  double pixelHeight = plot_->windowToPixelHeight(layoutData_.fullSize.height());

  layoutData_.pixelWidthExceeded  = (pixelWidth  > maxPixelWidth );
  layoutData_.pixelHeightExceeded = (pixelHeight > maxPixelHeight);

  //---

  // calc if horizontally, vertically scrolled and scroll area sizes
  layoutData_.hscrolled       = false;
  layoutData_.scrollAreaWidth = w;

  if      (scrollData_.width.isSet()) {
    double sw = plot_->lengthPlotWidth(scrollData_.width.length());

    layoutData_.hscrolled       = (w > sw);
    layoutData_.scrollAreaWidth = sw;
  }
  else if (layoutData_.pixelWidthExceeded) {
    layoutData_.hscrolled       = true;
    layoutData_.scrollAreaWidth = plot_->pixelToWindowWidth(maxPixelWidth);
  }

  layoutData_.vscrolled        = false;
  layoutData_.scrollAreaHeight = h - layoutData_.headerHeight;

  if      (scrollData_.height.isSet()) {
    double sh = plot_->lengthPlotHeight(scrollData_.height.length());

    layoutData_.vscrolled        = (h > sh + layoutData_.headerHeight);
    layoutData_.scrollAreaHeight = sh;
  }
  else if (layoutData_.pixelHeightExceeded) {
    layoutData_.vscrolled        = true;
    layoutData_.scrollAreaHeight = plot_->pixelToWindowHeight(maxPixelHeight) -
                                   layoutData_.headerHeight;
  }

  //---

  // adjust size for displayed scroll bar sizes
  layoutData_.vbarWidth  = 0.0;
  layoutData_.hbarHeight = 0.0;

  if (layoutData_.vscrolled)
    layoutData_.vbarWidth = vbw;

  if (layoutData_.hscrolled)
    layoutData_.hbarHeight = hbh;

  //---

  w = layoutData_.scrollAreaWidth  + layoutData_.vbarWidth;
  h = layoutData_.scrollAreaHeight + layoutData_.headerHeight + layoutData_.hbarHeight;

  //---

  layoutData_.size = QSizeF(w, h);
}

QPointF
CQChartsPlotKey::
absPlotPosition() const
{
  return plot_->viewToWindow(absPosition());
}

void
CQChartsPlotKey::
setAbsPlotPosition(const QPointF &p)
{
  setAbsPosition(plot_->windowToView(p));
}

QRectF
CQChartsPlotKey::
absPlotRect() const
{
  return plot_->viewToWindow(absRect());
}

void
CQChartsPlotKey::
setAbsPlotRect(const QRectF &r)
{
  setAbsRect(plot_->windowToView(r));
}

QSizeF
CQChartsPlotKey::
calcSize()
{
  doLayout();

  return layoutData_.size;
}

bool
CQChartsPlotKey::
contains(const CQChartsGeom::Point &p) const
{
  if (! isVisible() || isEmpty())
    return false;

  return bbox().inside(p);
}

CQChartsKeyItem *
CQChartsPlotKey::
getItemAt(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return nullptr;

  for (auto &item : items_) {
    if (item->bbox().inside(p))
      return item;
  }

  return nullptr;
}

bool
CQChartsPlotKey::
isEmpty() const
{
  return items_.empty();
}

//------

bool
CQChartsPlotKey::
selectMove(const CQChartsGeom::Point &w)
{
  bool changed = false;

  if (contains(w)) {
    CQChartsKeyItem *item = getItemAt(w);

    bool handled = false;

    if (item) {
      changed = setInside(item);

      handled = item->selectMove(w);
    }

    if (changed)
      redraw();

    if (handled)
      return true;
  }

  changed = setInside(nullptr);

  if (changed)
    redraw();

  return false;
}

//------

bool
CQChartsPlotKey::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_->setDragPos(p);

  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    location_ = CQChartsKeyLocation::Type::ABS_POSITION;

    setAbsPlotPosition(position_);
  }

  return true;
}

bool
CQChartsPlotKey::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point& dragPos  = editHandles_->dragPos();
  const CQChartsResizeSide&  dragSide = editHandles_->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (dragSide == CQChartsResizeSide::MOVE) {
    location_ = CQChartsKeyLocation::Type::ABS_POSITION;

    setAbsPlotPosition(absPlotPosition() + QPointF(dx, dy));
  }
  else {
    location_ = CQChartsKeyLocation::Type::ABS_RECT;

    editHandles_->updateBBox(dx, dy);

    wbbox_ = editHandles_->bbox();

    setAbsPlotRect(wbbox_.qrect());

    CQChartsLength width (wbbox_.getWidth ()                           , CQChartsUnits::PLOT);
    CQChartsLength height(wbbox_.getHeight() - layoutData_.headerHeight, CQChartsUnits::PLOT);

    setScrollWidth (CQChartsOptLength(width ));
    setScrollHeight(CQChartsOptLength(height));

    invalidateLayout();
  }

  editHandles_->setDragPos(p);

  updatePosition(/*queued*/false);

  return true;
}

bool
CQChartsPlotKey::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_->selectInside(p);
}

bool
CQChartsPlotKey::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

void
CQChartsPlotKey::
editMoveBy(const QPointF &f)
{
  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    location_ = CQChartsKeyLocation::Type::ABS_POSITION;

    setAbsPlotPosition(position_ + f);
  }

  updatePosition();
}

//------

bool
CQChartsPlotKey::
tipText(const CQChartsGeom::Point &p, QString &tip) const
{
  bool rc = false;

  CQChartsKeyItem *item = getItemAt(p);

  if (item) {
    QString tip1;

    if (item->tipText(p, tip1)) {
      if (! tip.length())
        tip += "\n";

      tip += tip1;

      rc = true;
    }
  }

  return rc;
}

//------

bool
CQChartsPlotKey::
setInside(CQChartsKeyItem *item)
{
  bool changed = false;

  for (auto &item1 : items_) {
    if (item1 == item) {
      if (! item1->isInside()) {
        item1->setInside(true);

        changed = true;
      }
    }
    else {
      if (item1->isInside()) {
        item1->setInside(false);

        changed = true;
      }
    }
  }

  return changed;
}

void
CQChartsPlotKey::
setFlipped(bool b)
{
  if (b == flipped_)
    return;

  flipped_ = b;

  needsLayout_ = true;

  redraw();
}

//------

void
CQChartsPlotKey::
draw(QPainter *painter) const
{
  if (! isVisible() || isEmpty()) {
    scrollData_.hbar->hide();
    scrollData_.vbar->hide();
    return;
  }

  //---

  CQChartsPlotKey *th = const_cast<CQChartsPlotKey *>(this);

  th->doLayout();

  //---

  CQChartsKeyLocation::Type locationType = this->location().type();

  //---

  // calc plot bounding box (full size)
  double x = position_.x(); // left
  double y = position_.y(); // top
  double w = layoutData_.size.width ();
  double h = layoutData_.size.height();

  if (locationType ==CQChartsKeyLocation::Type::ABS_RECT) {
    w = wbbox_.getWidth ();
    h = wbbox_.getHeight();
  }

  //---

  // calc key drawing area (not including scrollbars)
  double sw = layoutData_.scrollAreaWidth;
  double sh = layoutData_.scrollAreaHeight;

  CQChartsGeom::BBox sbbox(x, y - sh, x + sw, y);

  //---

  // set displayed bbox
  if (locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    wbbox_ = CQChartsGeom::BBox(x, y - h, x + w, y);
  }

  //---

  // calc pixel bounding box
  double px1, py1, px2, py2;

  plot_->windowToPixel(x    , y    , px1, py1);
  plot_->windowToPixel(x + w, y - h, px2, py2);

  QRectF pixelRect(px1, py1, px2 - px1, py2 - py1);

  //---

  // check if key size exceeds plot pixel size (auto hide if needed)
  if (isAutoHide()) {
    if (layoutData_.pixelWidthExceeded || layoutData_.pixelHeightExceeded) {
      scrollData_.hbar->hide();
      scrollData_.vbar->hide();
      return;
    }
  }

  // set clip rect to plot pixel rect
  CQChartsGeom::BBox plotPixelRect = plot_->calcPlotPixelRect();

  bool   clipped  = false;
  QRectF clipRect = CQChartsUtil::toQRect(plotPixelRect);

  //---

  // get plot data rect
  CQChartsGeom::BBox dataPixelRect = plot_->calcDataPixelRect();

  QRectF dataRect = CQChartsUtil::toQRect(dataPixelRect);

  //---

  painter->save();

  //---

  // place scroll bar
  sx_ = 0.0;
  sy_ = 0.0;

  double phh = plot_->windowToPixelHeight(layoutData_.headerHeight);

  double vspw = 0.0;
  double hsph = 0.0;

  if (layoutData_.vscrolled)
    vspw = scrollData_.pixelBarSize;

  if (layoutData_.hscrolled)
    hsph = scrollData_.pixelBarSize;

  if (layoutData_.vscrolled) {
    scrollData_.vbar->show();

    scrollData_.vbar->move(px2 - scrollData_.pixelBarSize - 1, py1 + phh);
    scrollData_.vbar->resize(scrollData_.pixelBarSize - 2, py2 - py1 - phh - hsph - 1);

    //---

    // count number of rows in height
    int    scrollRows   = 0;
    double scrollHeight = sh - 2*ym_ - layoutData_.hbarHeight;

    for (int i = 0; i < numRows_; ++i) {
      if (scrollHeight <= 0)
        break;

      ++scrollRows;

      int rh = rowHeights_[i] + 2*ys_;

      scrollHeight -= rh;
    }

    //---

    // update scroll bar
    if (scrollData_.vbar->pageStep() != scrollRows)
      scrollData_.vbar->setPageStep(scrollRows);

    int smax = std::max(numRows_ - scrollRows, 1);

    if (scrollData_.vbar->maximum() != smax)
      scrollData_.vbar->setRange(0, smax);

    if (scrollData_.vbar->value() != scrollData_.vpos)
      scrollData_.vbar->setValue(scrollData_.vpos);

    for (int i = 0; i < scrollData_.vpos; ++i)
      sy_ += rowHeights_[i] + 2*ys_;
  }
  else {
    if (scrollData_.vbar)
      scrollData_.vbar->hide();
  }

  //---

  if (layoutData_.hscrolled) {
    scrollData_.hbar->show();

    scrollData_.hbar->move(px1 + 1, py2 - scrollData_.pixelBarSize - 1);
    scrollData_.hbar->resize(px2 - px1 - vspw, scrollData_.pixelBarSize - 2);

    //---

    // update scroll bar
    int pageStep  = plot_->windowToPixelWidth(sw - layoutData_.vbarWidth);
    int fullWidth = plot_->windowToPixelWidth(layoutData_.fullSize.width());

    if (scrollData_.hbar->pageStep() != pageStep)
      scrollData_.hbar->setPageStep(pageStep);

    int smax = std::max(fullWidth - pageStep, 1);

    if (scrollData_.hbar->maximum() != smax)
      scrollData_.hbar->setRange(0, smax);

    if (scrollData_.hbar->value() != scrollData_.hpos)
      scrollData_.hbar->setValue(scrollData_.hpos);

    sx_ = plot_->pixelToWindowWidth(scrollData_.hpos);
  }
  else {
    if (scrollData_.hbar)
      scrollData_.hbar->hide();
  }

  //---

  if (layoutData_.vscrolled || layoutData_.hscrolled) {
    clipped  = true;
    clipRect = QRectF(px1, py1 + phh, px2 - px1 - vspw, py2 - py1 - phh - hsph);
  }
  else {
    if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
        locationType != CQChartsKeyLocation::Type::ABS_RECT) {
      if (isInsideX()) {
        clipRect.setLeft (dataRect.left ());
        clipRect.setRight(dataRect.right());
      }

      if (isInsideY()) {
        clipRect.setTop   (dataRect.top   ());
        clipRect.setBottom(dataRect.bottom());
      }
    }

    clipped = isClipped();
  }

  //---

  // draw box (background)
  CQChartsBoxObj::draw(painter, pixelRect);

  //---

  // draw header
  if (headerStr().length()) {
    // set text options
    CQChartsTextOptions textOptions;

    textOptions.contrast  = isHeaderTextContrast();
    textOptions.formatted = isHeaderTextFormatted();
    textOptions.scaled    = isHeaderTextScaled();
    textOptions.html      = isHeaderTextHtml();
    textOptions.align     = headerTextAlign();

    textOptions = plot_->adjustTextOptions(textOptions);

    //---

    // get font
    QFont font = view()->plotFont(plot_, headerTextFont());

    painter->setFont(font);

    //---

    // get key top left, width (pixels), margins
    QPointF p = plot_->windowToPixel(QPointF(x, y)); // top left

    double pw = plot_->windowToPixelWidth(sw);

  //double xm = margin();
    double ym = margin();

    //---

    // calc text rect
    QSizeF tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    double tw = pw;
    double th = tsize.height() + 2*ym;

    QRectF trect(p.x(), p.y(), tw, th);

    //---

    // set text pen
    QPen   tpen;
    QBrush tbrush;

    QColor tc = interpHeaderTextColor(0, 1);

    plot_->setPen(tpen, true, tc, headerTextAlpha());

    plot_->updateObjPenBrushState(this, tpen, tbrush);

    //---

    // draw text
    painter->setPen(tpen);

    CQChartsDrawUtil::drawTextInBox(painter, trect, headerStr(), textOptions);
  }

  //---

  if (clipped)
    painter->setClipRect(clipRect);

  //---

  // draw items
  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    Cell &cell = rowColCell_[item->row()][col];

    double x1 = cell.x - sx_;
    double y1 = cell.y + sy_;
    double w1 = cell.width;
    double h1 = cell.height;

    for (int c = 1; c < item->colSpan(); ++c) {
      Cell &cell1 = rowColCell_[item->row()][col + c];

      w1 += cell1.width;
    }

    for (int r = 1; r < item->rowSpan(); ++r) {
      Cell &cell1 = rowColCell_[item->row() + r][col];

      h1 += cell1.height;
    }

    CQChartsGeom::BBox bbox(x1 + x, y1 + y - h1, x1 + x + w1, y1 + y);

    item->setBBox(bbox);

    if (wbbox_.overlaps(bbox)) {
      item->draw(painter, bbox);

      if (plot_->showBoxes())
        plot_->drawWindowColorBox(painter, bbox);
    }
  }

  //---

  // draw box
  if (plot_->showBoxes()) {
    plot_->drawWindowColorBox(painter, wbbox_);

    CQChartsGeom::BBox headerBox(x, y - layoutData_.headerHeight, x + sw, y);

    plot_->drawWindowColorBox(painter, headerBox);
  }

  //---

  painter->restore();
}

void
CQChartsPlotKey::
drawEditHandles(QPainter *painter) const
{
  assert(plot_->view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  CQChartsPlotKey *th = const_cast<CQChartsPlotKey *>(this);

  if (scrollData_.height.isSet())
    th->editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
  else
    th->editHandles_->setMode(CQChartsEditHandles::Mode::MOVE);

  th->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
}

QColor
CQChartsPlotKey::
interpBgColor() const
{
  if (isFilled())
    return interpFillColor(0, 1);

  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    if      (isInsideX() && isInsideY()) {
      if (plot_->isDataFilled())
        return plot_->interpDataFillColor(0, 1);
    }
    else if (isInsideX()) {
      if (locationType == CQChartsKeyLocation::Type::CENTER_LEFT ||
          locationType == CQChartsKeyLocation::Type::CENTER_CENTER ||
          locationType == CQChartsKeyLocation::Type::CENTER_RIGHT) {
        if (plot_->isDataFilled())
          return plot_->interpDataFillColor(0, 1);
      }
    }
    else if (isInsideY()) {
      if (locationType == CQChartsKeyLocation::Type::TOP_CENTER ||
          locationType == CQChartsKeyLocation::Type::CENTER_CENTER ||
          locationType == CQChartsKeyLocation::Type::BOTTOM_CENTER) {
        if (plot_->isDataFilled())
          return plot_->interpDataFillColor(0, 1);
      }
    }
  }

  if (plot_->isPlotFilled())
    return plot_->interpPlotFillColor(0, 1);

  return plot_->interpThemeColor(0);
}

//------

CQChartsKeyItem::
CQChartsKeyItem(CQChartsPlotKey *key, int i, int n) :
 key_(key), i_(i), n_(n)
{
}

bool
CQChartsKeyItem::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
{
  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW)
      doShow(selMod);
    else if (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SELECT)
      doSelect(selMod);
  }

  return true;
}

bool
CQChartsKeyItem::
selectMove(const CQChartsGeom::Point &)
{
  return isClickable();
}

void
CQChartsKeyItem::
doShow(CQChartsSelMod selMod)
{
  CQChartsPlot *plot = key_->plot();

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int i = 0; i < n_; ++i)
      plot->setSetHidden(i, i != i_);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setSetHidden(i_, false);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setSetHidden(i_, true);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->queueUpdateObjs();
}

void
CQChartsKeyItem::
doSelect(CQChartsSelMod)
{
}

bool
CQChartsKeyItem::
tipText(const CQChartsGeom::Point &, QString &) const
{
  return false;
}

//------

CQChartsKeyText::
CQChartsKeyText(CQChartsPlot *plot, const QString &text, int i, int n) :
 CQChartsKeyItem(plot->key(), i, n), plot_(plot), text_(text)
{
}

QSizeF
CQChartsKeyText::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.width(text_);
  double h = fm.height();

  double ww = plot->pixelToWindowWidth (w + 4);
  double wh = plot->pixelToWindowHeight(h + 4);

  return QSizeF(ww, wh);
}

QColor
CQChartsKeyText::
interpTextColor(int i, int n) const
{
  return key_->interpTextColor(i, n);
}

void
CQChartsKeyText::
draw(QPainter *painter, const CQChartsGeom::BBox &rect) const
{
  CQChartsPlot *plot = key_->plot();

  plot->view()->setPlotPainterFont(plot, painter, key_->textFont());

  QFontMetricsF fm(painter->font());

  QColor tc = interpTextColor(0, 1);

  painter->setPen(tc);

  double px1, px2, py;

  plot->windowToPixel(rect.getXMin(), rect.getYMin(), px1, py);
  plot->windowToPixel(rect.getXMax(), rect.getYMin(), px2, py);

  if (px1 > px2)
    std::swap(px1, px2);

  double px = px1 + 2;

  if (key_->textAlign() & Qt::AlignRight)
    px = px2 - 2 - fm.width(text_);

  QPointF tp;

  if (! plot->isInvertY())
    tp = QPointF(px, py - fm.descent() - 2);
  else
    tp = QPointF(px, py + fm.ascent() + 2);

  CQChartsDrawUtil::drawSimpleText(painter, tp, text_);
}

//------

CQChartsKeyColorBox::
CQChartsKeyColorBox(CQChartsPlot *plot, int i, int n) :
 CQChartsKeyItem(plot->key(), i, n), plot_(plot)
{
  setClickable(true);
}

QColor
CQChartsKeyColorBox::
interpBorderColor(int i, int n) const
{
  return plot_->charts()->interpColor(borderColor(), i, n);
}

QSizeF
CQChartsKeyColorBox::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double h = fm.height();

  double ww = plot->pixelToWindowWidth (h + 2);
  double wh = plot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

void
CQChartsKeyColorBox::
draw(QPainter *painter, const CQChartsGeom::BBox &rect) const
{
  CQChartsPlot *plot = key_->plot();

  CQChartsGeom::BBox prect;

  plot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  QColor bc    = interpBorderColor(0, 1);
  QBrush brush = fillBrush();

  if (isInside())
    brush.setColor(plot->insideColor(brush.color()));

  painter->setPen  (bc);
  painter->setBrush(brush);

  double cxs = plot->lengthPixelWidth (cornerRadius());
  double cys = plot->lengthPixelHeight(cornerRadius());

  CQChartsRoundedPolygon::draw(painter, prect1, cxs, cys);
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  CQChartsPlot *plot = key_->plot();

  QColor c = plot->interpPaletteColor(i_, n_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}
