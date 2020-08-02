#include <CQChartsRectEdit.h>
#include <CQChartsGeomBBoxEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsRectEdit::
CQChartsRectEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("rectEdit");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQChartsGeomBBoxEdit>("rect");

  edit_->setToolTip("Rectangle Coords (Left Bottom Right Top)");

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(regionChanged()), this, SLOT(editRegionSlot()));

  //---

  unitsEdit_ = CQUtil::makeWidget<CQChartsUnitsEdit>("units");

  layout->addWidget(unitsEdit_);

  //---

  setFocusProxy(edit_);

  connectSlots(true);
}

void
CQChartsRectEdit::
setRect(const CQChartsRect &rect)
{
  rect_ = rect;

  rectToWidgets();
}

void
CQChartsRectEdit::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  edit_->setPlot(plot);
}

void
CQChartsRectEdit::
editRegionSlot()
{
  setRegion(edit_->getValue());
}

void
CQChartsRectEdit::
setRegion(const CQChartsGeom::BBox &bbox)
{
  setRect(CQChartsRect(bbox, CQChartsUnits::PLOT));

  emit regionChanged();
}

void
CQChartsRectEdit::
editChanged()
{
  auto        bbox  = edit_->getValue();
  const auto &units = rect_.units();

  CQChartsRect rect(bbox, units);

  if (! rect.isValid())
    return;

  rect_ = rect;

  emit rectChanged();
}

void
CQChartsRectEdit::
unitsChanged()
{
  const auto &bbox  = rect_.bbox();
  auto        units = unitsEdit_->units();

  CQChartsRect rect(bbox, units);

  if (! rect.isValid())
    return;

  rect_ = rect;

  emit rectChanged();
}

void
CQChartsRectEdit::
rectToWidgets()
{
  connectSlots(false);

  const auto &bbox  = rect_.bbox();
  const auto &units = rect_.units();

  edit_->setValue(bbox);

  unitsEdit_->setUnits(units);

  connectSlots(true);
}

#if 0
void
CQChartsRectEdit::
widgetsToRect()
{
  auto bbox  = edit_->getValue();
  auto units = unitsEdit_->units();

  CQChartsRect rect(bbox, units);

  if (! rect.isValid())
    return;

  rect_ = rect;
}
#endif

void
CQChartsRectEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(edit_, SIGNAL(valueChanged()), SLOT(editChanged()));
  connectDisconnect(unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsRectPropertyViewType::
CQChartsRectPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsRectPropertyViewType::
getEditor() const
{
  return new CQChartsRectPropertyViewEditor;
}

bool
CQChartsRectPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsRectPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  //---

  bool ok;

  QString str = valueString(item, value, ok);

  QFont font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsRectPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsRectPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  CQChartsRect rect = value.value<CQChartsRect>();

  QString str;

  if (rect.isValid()) {
    str = rect.toString();
    ok  = true;
  }
  else {
    str = "Undefined";
    ok  = false;
  }

  return str;
}

//------

CQChartsRectPropertyViewEditor::
CQChartsRectPropertyViewEditor()
{
}

QWidget *
CQChartsRectPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsRectEdit(parent);

  return edit;
}

void
CQChartsRectPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsRectEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(rectChanged()), obj, method);
}

QVariant
CQChartsRectPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsRectEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->rect());
}

void
CQChartsRectPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsRectEdit *>(w);
  assert(edit);

  CQChartsRect rect = var.value<CQChartsRect>();

  edit->setRect(rect);
}
