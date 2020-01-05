#include <CQChartsOptRectEdit.h>

CQChartsOptRectEdit::
CQChartsOptRectEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "Rect")
{
}

void
CQChartsOptRectEdit::
setValue(const CQChartsOptRect &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptRectEdit::
setTextValue(const QString &str)
{
  CQChartsOptRect value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptRectEdit::
resetValue()
{
  value_ = CQChartsOptRect();
}

bool
CQChartsOptRectEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptRectEdit::
valueToText()
{
  const CQChartsRect &l = value_.rect();

  return l.toString();
}

QVariant
CQChartsOptRectEdit::
toVariant() const
{
  return QVariant::fromValue(value());
}

void
CQChartsOptRectEdit::
fromVariant(const QVariant &var)
{
  CQChartsOptRect value = var.value<CQChartsOptRect>();

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptRectPropertyViewType::
getEditor() const
{
  return new CQChartsOptRectPropertyViewEditor;
}

QString
CQChartsOptRectPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  CQChartsOptRect optRect = value.value<CQChartsOptRect>();

  QString str;

  if (optRect.isSet()) {
    str = optRect.toString();
    ok  = true;
  }
  else {
    str = "Undefined";
    ok  = false;
  }

  return str;
}

//------

QWidget *
CQChartsOptRectPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptRectEdit(parent);
}