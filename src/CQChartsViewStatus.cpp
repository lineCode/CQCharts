#include <CQChartsViewStatus.h>
#include <CQChartsWindow.h>
#include <QPainter>

CQChartsViewStatus::
CQChartsViewStatus(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("status");

  setAutoFillBackground(true);
}

void
CQChartsViewStatus::
setText(const QString &s)
{
  text_ = s;

  update();
}

void
CQChartsViewStatus::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), palette().color(QPalette::Window));

  QFontMetricsF fm(font());

  p.drawText(2, 2 + fm.ascent(), text());
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
}
