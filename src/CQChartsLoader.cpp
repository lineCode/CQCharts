#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsFileType.h>
#include <CQChartsCsvFilterModel.h>
#include <CQChartsDataFilterModel.h>
#include <CQChartsExprDataFilterModel.h>
#include <CQChartsGnuDataFilterModel.h>
#include <CQChartsJsonFilterModel.h>
#include <CQChartsTsvFilterModel.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsExprModel.h>
#include <CQChartsColumnType.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <CQPerfMonitor.h>
#if defined(CQCharts_USE_TCL)
#include <CQTclUtil.h>
#endif
#include <CMathCorrelation.h>

CQChartsLoader::
CQChartsLoader(CQCharts *charts) :
 charts_(charts)
{
}

#if defined(CQCharts_USE_TCL)
void
CQChartsLoader::
setQtcl(CQTcl *qtcl)
{
  qtcl_ = qtcl;
}
#endif

QAbstractItemModel *
CQChartsLoader::
loadFile(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  CQPerfTrace trace("CQChartsLoader::loadFile");

  hierarchical = false;

  QAbstractItemModel *model = nullptr;

  if      (type == CQChartsFileType::CSV) {
    CQChartsCsvFilterModel *csv = loadCsv(filename, inputData);

    if (! csv) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return csv;
  }
  else if (type == CQChartsFileType::TSV) {
    CQChartsTsvFilterModel *tsv = loadTsv(filename, inputData);

    if (! tsv) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return tsv;
  }
  else if (type == CQChartsFileType::JSON) {
    CQChartsJsonFilterModel *json = loadJson(filename, inputData);

    if (! json) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    hierarchical = json->isHierarchical();

    return json;
  }
  else if (type == CQChartsFileType::DATA) {
    CQChartsGnuDataFilterModel *data = loadData(filename, inputData);

    if (! data) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return data;
  }
  else if (type == CQChartsFileType::EXPR) {
    model = createExprModel(inputData.numRows);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }
  }
  else if (type == CQChartsFileType::VARS) {
    model = createVarsModel(inputData);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }
  }
  else {
    charts_->errorMsg("Bad file type specified '" + fileTypeToString(type) + "'");
    return nullptr;
  }

  return model;
}

CQChartsCsvFilterModel *
CQChartsLoader::
loadCsv(const QString &filename, const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadCsv");

  CQChartsCsvFilterModel *csv = new CQChartsCsvFilterModel(charts_);

  csv->setCommentHeader    (inputData.commentHeader);
  csv->setFirstLineHeader  (inputData.firstLineHeader);
  csv->setFirstColumnHeader(inputData.firstColumnHeader);

  if (inputData.separator.length())
    csv->setSeparator(inputData.separator[0].toLatin1());

  if (! csv->load(filename)) {
    delete csv;
    return nullptr;
  }

  if (inputData.filter.length())
    csv->setSimpleFilter(inputData.filter);

  return csv;
}

CQChartsTsvFilterModel *
CQChartsLoader::
loadTsv(const QString &filename, const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadTsv");

  CQChartsTsvFilterModel *tsv = new CQChartsTsvFilterModel(charts_);

  tsv->setCommentHeader    (inputData.commentHeader);
  tsv->setFirstLineHeader  (inputData.firstLineHeader);
  tsv->setFirstColumnHeader(inputData.firstColumnHeader);

  if (! tsv->load(filename)) {
    delete tsv;
    return nullptr;
  }

  if (inputData.filter.length())
    tsv->setSimpleFilter(inputData.filter);

  return tsv;
}

CQChartsJsonFilterModel *
CQChartsLoader::
loadJson(const QString &filename, const CQChartsInputData &)
{
  CQPerfTrace trace("CQChartsLoader::loadJson");

  CQChartsJsonFilterModel *json = new CQChartsJsonFilterModel(charts_);

  if (! json->load(filename)) {
    delete json;
    return nullptr;
  }

  return json;
}

CQChartsGnuDataFilterModel *
CQChartsLoader::
loadData(const QString &filename, const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadData");

  CQChartsGnuDataFilterModel *data = new CQChartsGnuDataFilterModel(charts_);

  data->setCommentHeader    (inputData.commentHeader);
  data->setFirstLineHeader  (inputData.firstLineHeader);
  data->setFirstColumnHeader(inputData.firstColumnHeader);

  if (! data->load(filename)) {
    delete data;
    return nullptr;
  }

  if (inputData.filter.length())
    data->setSimpleFilter(inputData.filter);

  return data;
}

QAbstractItemModel *
CQChartsLoader::
createExprModel(int n)
{
  CQPerfTrace trace("CQChartsLoader::createExprModel");

  int nc = 1;
  int nr = n;

  CQChartsExprDataFilterModel *data = new CQChartsExprDataFilterModel(charts_, nc, nr);

  return data;
}

QAbstractItemModel *
CQChartsLoader::
createVarsModel(const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::createVarsModel");

#if defined(CQCharts_USE_TCL)
  using ColumnValues = std::vector<QVariant>;
  using VarColumns   = std::vector<ColumnValues>;

  VarColumns varColumns;

  int nv = inputData.vars.size();

  int nr = -1;

  if (nv == 1) {
    QString varName = inputData.vars[0].toString();

    ColumnValues columnValues;

    if (qtcl_)
      columnValues = qtcl_->getListVar(varName);

    if (! inputData.transpose) {
      int nc = columnValues.size();

      for (int c = 0; c < nc; ++c) {
        ColumnValues columnValues1;

        if (columnValues[c].type() == QVariant::List) {
          QList<QVariant> rowVars = columnValues[c].toList();

          for (int i = 0; i < rowVars.length(); ++i)
            columnValues1.push_back(rowVars[i]);
        }
        else {
          columnValues1.push_back(columnValues[c]);
        }

        int nv1 = columnValues1.size();

        if (nr < 0)
          nr = nv1;
        else
          nr = std::min(nr, nv1);

        varColumns.push_back(columnValues1);
      }
    }
    else {
      using IndColumnValues = std::map<int,ColumnValues>;

      IndColumnValues indColumnValues;

      nr = columnValues.size();

      for (int r = 0; r < nr; ++r) {
        if (columnValues[r].type() == QVariant::List) {
          QList<QVariant> columnVars = columnValues[r].toList();

          for (int c = 0; c < columnVars.length(); ++c)
            indColumnValues[c].push_back(columnVars[c]);
        }
        else {
          indColumnValues[0].push_back(columnValues[r]);
        }
      }

      nr = 0;

      for (auto &i : indColumnValues) {
        ColumnValues &columnValues = i.second;

        nr = std::max(nr, int(columnValues.size()));
      }

      for (auto &i : indColumnValues) {
        ColumnValues &columnValues = i.second;

        while (int(columnValues.size()) < nr)
          columnValues.push_back("");
      }

      for (auto &i : indColumnValues) {
        ColumnValues &columnValues = i.second;

        varColumns.push_back(columnValues);
      }
    }
  }
  else {
    for (int i = 0; i < nv; ++i) {
      QString varName = inputData.vars[i].toString();

      ColumnValues columnValues;

      if (qtcl_)
        columnValues = qtcl_->getListVar(varName);

      int nv1 = columnValues.size();

      if (nr < 0)
        nr = nv1;
      else
        nr = std::min(nr, nv1);

      varColumns.push_back(columnValues);
    }
  }

  if (nr < 0)
    return nullptr;

  //---

  int ic = 0;
  int ir = 0;

  if (inputData.firstColumnHeader) ++ic;
  if (inputData.firstLineHeader  ) ++ir;

  //---

  int nc = varColumns.size();

  CQChartsDataFilterModel *model = new CQChartsDataFilterModel(charts_, nc - ic, nr - ir);

  CQDataModel *dataModel = model->dataModel();

  QModelIndex parent;

  if (inputData.firstColumnHeader) {
    const ColumnValues &columnValues = varColumns[0];

    for (int r = ir; r < nr; ++r)
      dataModel->setHeaderData(r - ir, Qt::Vertical, columnValues[r]);
  }

  if (inputData.firstLineHeader) {
    for (int c = ic; c < nc; ++c) {
      const ColumnValues &columnValues = varColumns[c];

      dataModel->setHeaderData(c - ic, Qt::Horizontal, columnValues[0]);
    }
  }

  for (int c = ic; c < nc; ++c) {
    const ColumnValues &columnValues = varColumns[c];

    for (int r = ir; r < nr; ++r) {
      QModelIndex ind = dataModel->index(r - ir, c - ic, parent);

      dataModel->setData(ind, columnValues[r]);
    }
  }

  return model;
#else
  int nc = inputData.vars.size();
  int nr = 100;

  CQChartsExprDataFilterModel *model = new CQChartsExprDataFilterModel(charts_, nc, nr);

  return model;
#endif
}

QAbstractItemModel *
CQChartsLoader::
createCorrelationModel(QAbstractItemModel *model, bool flip)
{
  CQPerfTrace trace("CQChartsLoader::createCorrelationModel");

  int nr = model->rowCount   ();
  int nc = model->columnCount();

  int nv = (! flip ? nc : nr);

  using ColumnValues = std::vector<CMathCorrelation::Values>;
  using ColumnNames  = std::vector<QString>;

  ColumnValues columnValues;
  ColumnNames  columnNames;

  columnValues.resize(nv);
  columnNames .resize(nv);

  if (! flip) {
    for (int c = 0; c < nc; ++c) {
      bool ok;

      columnNames[c] = CQChartsUtil::modelHeaderString(model, c, Qt::Horizontal, ok);

      CMathCorrelation::Values &values = columnValues[c];

      values.resize(nr);

      for (int r = 0; r < nr; ++r) {
        QModelIndex ind = model->index(r, c, QModelIndex());

        bool ok;

        double v = CQChartsUtil::modelReal(model, ind, ok);

        values[r] = v;
      }
    }
  }
  else {
    for (int r = 0; r < nr; ++r) {
      bool ok;

      columnNames[r] = CQChartsUtil::modelHeaderString(model, r, Qt::Vertical, ok);

      CMathCorrelation::Values &values = columnValues[r];

      values.resize(nc);

      for (int c = 0; c < nc; ++c) {
        QModelIndex ind = model->index(r, c, QModelIndex());

        bool ok;

        double v = CQChartsUtil::modelReal(model, ind, ok);

        values[c] = v;
      }
    }
  }

  //---

  CQChartsDataFilterModel *dataModel = new CQChartsDataFilterModel(charts_, nv, nv);

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQDataModel *model1 = dataModel->dataModel();

  for (int c = 0; c < nv; ++c)
    (void) columnTypeMgr->setModelColumnType(model1, c, CQBaseModel::Type::REAL);

  for (int c = 0; c < nv; ++c) {
    CQChartsUtil::setModelHeaderValue(model1, c, Qt::Horizontal, columnNames[c], Qt::DisplayRole);
    CQChartsUtil::setModelHeaderValue(model1, c, Qt::Vertical  , columnNames[c], Qt::DisplayRole);

    CQChartsUtil::setModelValue(model1, c, c, 1.0);
  }

  for (int c1 = 0; c1 < nv; ++c1) {
    for (int c2 = c1; c2 < nv; ++c2) {
      double corr = CMathCorrelation::calc(columnValues[c1], columnValues[c2]);

      CQChartsUtil::setModelValue(model1, c1, c2, corr);
      CQChartsUtil::setModelValue(model1, c2, c1, corr);
    }
  }

  return dataModel;
}
