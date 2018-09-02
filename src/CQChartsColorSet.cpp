#include <CQChartsColorSet.h>
#include <CQChartsColor.h>
#include <CQChartsVariant.h>

CQChartsColorSet::
CQChartsColorSet(CQChartsPlot *plot) :
 CQChartsValueSet(plot)
{
}

bool
CQChartsColorSet::
icolor(int i, CQChartsColor &color)
{
  if (empty())
    return false;

  if (! hasInd(i))
    return false;

  // color can be actual color value (string) or value used to map into palette
  // (map enabled or disabled)
  if      (type() == Type::STRING) {
    QVariant colorVar = value(i);

    // TODO: force all color columns with names to use color type
    QColor c(colorVar.toString());

    if (c.isValid()) {
      color = c;

      return true;
    }

    // interped color must have at least 2 unique string values
    if (snum() <= 1)
      return false;
  }
  else if (type() == Type::COLOR) {
    bool ok;

    CQChartsColor c = CQChartsVariant::toColor(value(i), ok);

    if (c.isValid()) {
      color = c;

      return true;
    }

    return false;
  }

  //---

  double value = imap(i);

  CQChartsColor c = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, value);

  color = c;

  return true;
}