#include <CQChartsModelWidgets.h>
#include <CQChartsModelList.h>
#include <CQChartsModelControl.h>
#include <CQCharts.h>

#include <QVBoxLayout>

CQChartsModelWidgets::
CQChartsModelWidgets(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setObjectName("modelWidgets");

  QVBoxLayout *layout = new QVBoxLayout(this);

  // create models list
  modelList_ = new CQChartsModelList(charts_);

  layout->addWidget(modelList_);

  //---

  // create current model control
  modelControl_ = new CQChartsModelControl(charts_);

  layout->addWidget(modelControl_);

  //---

  modelList_   ->setModelControl(modelControl_);
  modelControl_->setModelList   (modelList_);
}

void
CQChartsModelWidgets::
addModelData(CQChartsModelData *modelData)
{
  modelList_->addModelData(modelData);
}