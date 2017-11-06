#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>
#include <QTimer>

namespace {

int numColors = 20;

int s_colorId = -1;

int nextColorId() {
  ++s_colorId;

  if (s_colorId >= numColors)
    s_colorId = 0;

  return s_colorId;
}

}

//------

CQChartsHierBubblePlotType::
CQChartsHierBubblePlotType()
{
  addColumnParameter("name" , "Name"  , "nameColumn" , "", 0);
  addColumnParameter("value", "Value" , "valueColumn", "", 1);

  addStringParameter("separator", "Separator", "separator", "optional", "/");
}

CQChartsPlot *
CQChartsHierBubblePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsHierBubblePlot(view, model);
}

//------

CQChartsHierBubblePlot::
CQChartsHierBubblePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("bubble"), model)
{
  setMargins(1, 1, 1, 1);

  addTitle();
}

void
CQChartsHierBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "separator" );
  addProperty("", this, "fontHeight");
}

void
CQChartsHierBubblePlot::
setCurrentRoot(CQChartsHierBubbleHierNode *hier)
{
  currentRoot_ = hier;
}

void
CQChartsHierBubblePlot::
updateCurrentRoot()
{
  placeNodes(currentRoot_);

  updateObjs();
}

void
CQChartsHierBubblePlot::
updateRange(bool apply)
{
  dataRange_.reset();

  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  if (apply)
    applyDataRange();
}

void
CQChartsHierBubblePlot::
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

  if (! root_)
    initNodes();

  //---

  initNodeObjs(currentRoot_, nullptr, 0);
}

void
CQChartsHierBubblePlot::
initNodeObjs(CQChartsHierBubbleHierNode *hier, CQChartsHierBubbleHierObj *parentObj, int depth)
{
  CQChartsHierBubbleHierObj *hierObj = 0;

  if (hier != root_) {
    double r = hier->radius();

    CBBox2D rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    hierObj = new CQChartsHierBubbleHierObj(this, hier, parentObj, rect, hier->depth(), maxDepth());

    hierObj->setId(QString("%1:%2").arg(hier->name()).arg(hier->size()));

    addPlotObject(hierObj);
  }

  //---

  for (auto hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1);
  }

  //---

  for (auto node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CBBox2D rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    CQChartsHierBubbleObj *obj =
      new CQChartsHierBubbleObj(this, node, parentObj, rect, node->depth(), maxDepth());

    obj->setId(QString("%1:%2").arg(node->name()).arg(node->size()));

    addPlotObject(obj);
  }
}

void
CQChartsHierBubblePlot::
initNodes()
{
  root_ = new CQChartsHierBubbleHierNode(0, "<root>");

  root_->setDepth(0);

  currentRoot_ = root_;

  //---

  if (isHier())
    loadChildren(root_);
  else
    loadFlat();

  //---

  placeNodes(currentRoot_);
}

void
CQChartsHierBubblePlot::
placeNodes(CQChartsHierBubbleHierNode *hier)
{
  initNodes(hier);

  //---

  hier->packNodes();

  offset_ = CPoint2D(hier->x(), hier->y());
  scale_  = 1.0/hier->radius();

  //---

  hier->setX((hier->x() - offset_.x)*scale_);
  hier->setY((hier->y() - offset_.y)*scale_);

  hier->setRadius(1.0);

  transformNodes(hier);
}

void
CQChartsHierBubblePlot::
initNodes(CQChartsHierBubbleHierNode *hier)
{
  for (auto hierNode : hier->getChildren()) {
    hierNode->initRadius();

    initNodes(hierNode);
  }

  //---

  for (auto node : hier->getNodes())
    node->initRadius();
}

void
CQChartsHierBubblePlot::
transformNodes(CQChartsHierBubbleHierNode *hier)
{
  for (auto hierNode : hier->getChildren()) {
    hierNode->setX((hierNode->x() - offset_.x)*scale_);
    hierNode->setY((hierNode->y() - offset_.y)*scale_);

    hierNode->setRadius(hierNode->radius()*scale_);

    transformNodes(hierNode);
  }

  //---

  for (auto node : hier->getNodes()) {
    node->setX((node->x() - offset_.x)*scale_);
    node->setY((node->y() - offset_.y)*scale_);

    node->setRadius(node->radius()*scale_);
  }
}

bool
CQChartsHierBubblePlot::
isHier() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return true;

  int nc = model->rowCount();

  for (int i = 0; i < nc; ++i) {
    QModelIndex index1 = model->index(i, 0);

    if (model->rowCount(index1) > 0)
      return true;
  }

  return false;
}

void
CQChartsHierBubblePlot::
loadChildren(CQChartsHierBubbleHierNode *hier, const QModelIndex &index, int depth)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  int colorId = -1;

  int nr = model->rowCount(index);

  for (int r = 0; r < nr; ++r) {
    QModelIndex index1 = model->index(r, nameColumn (), index);
    QModelIndex index2 = model->index(r, valueColumn(), index);

    //---

    bool ok;

    QString name = CQChartsUtil::modelString(model, index1, ok);

    //---

    if (model->rowCount(index1) > 0) {
      CQChartsHierBubbleHierNode *hier1 = new CQChartsHierBubbleHierNode(hier, name);

      hier1->setDepth(depth);

      loadChildren(hier1, index1, depth + 1);
    }
    else {
      if (colorId < 0)
        colorId = nextColorId();

      bool ok;

      int size = CQChartsUtil::modelInteger(model, index2, ok);

      if (! ok) size = 1;

      CQChartsHierBubbleNode *node =
        new CQChartsHierBubbleNode(hier, name, size, colorId, r, index);

      node->setDepth(depth);

      hier->addNode(node);
    }
  }
}

void
CQChartsHierBubblePlot::
loadFlat()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    QModelIndex index1 = model->index(r, nameColumn ());
    QModelIndex index2 = model->index(r, valueColumn());

    bool ok1, ok2;

    QString name = CQChartsUtil::modelString (model, index1, ok1);
    int     size = CQChartsUtil::modelInteger(model, index2, ok2);

    QStringList strs;

    if (separator().length())
      strs = name.split(separator(), QString::SkipEmptyParts);
    else
      strs << name;

    if (! strs.length())
      continue;

    //--

    int depth = strs.length();

    maxDepth_ = std::max(maxDepth_, depth + 1);

    CQChartsHierBubbleHierNode *parent = root_;

    for (int j = 0; j < strs.length() - 1; ++j) {
      CQChartsHierBubbleHierNode *child = childHierNode(parent, strs[j]);

      if (! child) {
        child = new CQChartsHierBubbleHierNode(parent, strs[j]);

        child->setDepth(depth);
      }

      parent = child;
    }

    CQChartsHierBubbleNode *node = childNode(parent, strs[strs.length() - 1]);

    if (! node) {
      node = new CQChartsHierBubbleNode(parent, strs[strs.length() - 1], size, depth,
                                        r, QModelIndex());

      node->setDepth(depth);

      parent->addNode(node);
    }
  }
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
childHierNode(CQChartsHierBubbleHierNode *parent, const QString &name) const
{
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
childNode(CQChartsHierBubbleHierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

void
CQChartsHierBubblePlot::
zoomFull()
{
  setCurrentRoot(root_);

  updateCurrentRoot();
}

void
CQChartsHierBubblePlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsHierBubblePlot::
drawForeground(QPainter *p)
{
  drawBounds(p, currentRoot_);
}

void
CQChartsHierBubblePlot::
drawBounds(QPainter *p, CQChartsHierBubbleHierNode *hier)
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  double px1, py1, px2, py2;

  windowToPixel(xc - r, yc + r, px1, py1);
  windowToPixel(xc + r, yc - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen(QColor(0,0,0));

  p->setBrush(Qt::NoBrush);

  p->drawPath(path);

  //---

  for (auto hierNode : hier->getChildren()) {
    drawBounds(p, hierNode);
  }
}

QColor
CQChartsHierBubblePlot::
nodeColor(CQChartsHierBubbleNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/numColors, c);
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                          CQChartsHierBubbleHierObj *hierObj, const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), hier_(hier), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsHierBubbleHierObj::
inside(const CPoint2D &p) const
{
  if (CQChartsUtil::PointPointDistance(p, CPoint2D(hier_->x(), hier_->y())) < hier_->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleHierObj::
clickZoom(const CPoint2D &)
{
#if 0
  CQChartsHierBubbleHierNode *parent1 = hier_->parent();
  CQChartsHierBubbleHierNode *parent2 = (parent1 ? parent1->parent() : nullptr);

  while (parent2 && parent2 != plot_->currentRoot()) {
    parent1 = parent2;
    parent2 = (parent1 ? parent1->parent() : nullptr);
  }

  if (parent1) {
    plot_->setCurrentRoot(parent1);

    QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
  }
#else
  plot_->setCurrentRoot(hier_);

  QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
#endif
}

void
CQChartsHierBubbleHierObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  double f = (1.0*(i_ + 1))/(n_ + 1);

//QColor c = plot_->objectStateColor(this, plot_->hierColor(hier_));
  QColor c = plot_->interpPaletteColor(f);

  //QColor tc = Qt::black;
  QColor tc = CQChartsUtil::bwColor(c);

  //QColor c1;
  //c1.setHsvF(c.hueF(), c.saturationF()*f, c.valueF());

  double r = hier_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x() - r, hier_->y() + r, px1, py1);
  plot_->windowToPixel(hier_->x() + r, hier_->y() - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen(tc);

  if (isInside())
    p->setBrush(plot_->insideColor(c));
  else
    p->setBrush(c);

  p->drawPath(path);
}

//------

CQChartsHierBubbleObj::
CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                      CQChartsHierBubbleHierObj *hierObj, const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsHierBubbleObj::
inside(const CPoint2D &p) const
{
  if (CQChartsUtil::PointPointDistance(p, CPoint2D(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleObj::
clickZoom(const CPoint2D &)
{
#if 0
  CQChartsHierBubbleHierNode *parent1 = node_->parent();
  CQChartsHierBubbleHierNode *parent2 = (parent1 ? parent1->parent() : nullptr);

  while (parent2 && parent2 != plot_->currentRoot()) {
    parent1 = parent2;
    parent2 = (parent1 ? parent1->parent() : nullptr);
  }

  if (parent1) {
    plot_->setCurrentRoot(parent1);

    QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
  }
#else
  CQChartsHierBubbleHierNode *parent = node_->parent();

  plot_->setCurrentRoot(parent);

  QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
#endif
}

void
CQChartsHierBubbleObj::
mousePress(const CPoint2D &)
{
  plot_->beginSelect();

  plot_->addSelectIndex(node_->row(), plot_->nameColumn (), node_->ind());
  plot_->addSelectIndex(node_->row(), plot_->valueColumn(), node_->ind());

  plot_->endSelect();
}

void
CQChartsHierBubbleObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  p->setFont(font);

  //---

  QFontMetricsF fm(p->font());

  QColor c = plot_->interpPaletteColor((1.0*(i_ + 1))/(n_ + 1));
//QColor c = plot_->objectStateColor(this, plot_->nodeColor(node_));

  QColor tc = plot_->textColor(c);

  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen(tc);

  if (isInside())
    p->setBrush(plot_->insideColor(c));
  else
    p->setBrush(c);

  p->drawPath(path);

  //---

  p->setPen(tc);

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  int len = node_->name().size();

  for (int i = len; i >= 1; --i) {
    QString name1 = node_->name().mid(0, i);

    double tw = fm.width(name1);

    if (tw > 2*(px2 - px1)) continue;

    p->drawText(px1 - tw/2, py1 + fm.descent(), name1);

    break;
  }
}

//------

CQChartsHierBubbleHierNode::
CQChartsHierBubbleHierNode(CQChartsHierBubbleHierNode *parent, const QString &name) :
 CQChartsHierBubbleNode(parent, name, 0.0, -1, -1, QModelIndex()), parent_(parent)
{
  if (parent_)
    parent_->children_.push_back(this);
}

void
CQChartsHierBubbleHierNode::
packNodes()
{
  pack_.reset();

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
    (*pn)->resetPosition();

  //---

  // pack child hier nodes first
  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
    (*p)->packNodes();

  //---

  // make single list of nodes to pack
  Nodes packNodes;

  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
    packNodes.push_back(*p);

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
    packNodes.push_back(*pn);

  // sort nodes
  std::sort(packNodes.begin(), packNodes.end(), CQChartsHierBubbleNodeCmp());

  // pack nodes
  for (Nodes::const_iterator pp = packNodes.begin(); pp != packNodes.end(); ++pp)
    pack_.addNode(*pp);

  //---

  // get bounding circle
  double xc { 0.0 }, yc { 0.0 }, r { 1.0 };

  pack_.boundingCircle(xc, yc, r);

  // set center and radius
  x_ = xc;
  y_ = yc;

  setRadius(r);

  //setRadius(std::max(std::max(fabs(xmin), xmax), std::max(fabs(ymin), ymax)));
}

void
CQChartsHierBubbleHierNode::
addNode(CQChartsHierBubbleNode *node)
{
  nodes_.push_back(node);

  size_ += node->size();

  CQChartsHierBubbleHierNode *parent = parent_;

  while (parent) {
    parent->size_ += node->size();

    parent = parent->parent_;
  }
}

void
CQChartsHierBubbleHierNode::
setPosition(double x, double y)
{
  double dx = x - this->x();
  double dy = y - this->y();

  CQChartsHierBubbleNode::setPosition(x, y);

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
    CQChartsHierBubbleNode *node = *pn;

    node->setPosition(node->x() + dx, node->y() + dy);
  }

  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p) {
    CQChartsHierBubbleHierNode *hierNode = *p;

    hierNode->setPosition(hierNode->x() + dx, hierNode->y() + dy);
  }
}
