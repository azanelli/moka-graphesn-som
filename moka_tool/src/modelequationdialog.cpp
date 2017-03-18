#include "modelequationdialog.h"
#include "ui_modelequationdialog.h"

ModelEquationDialog::ModelEquationDialog
(
    QString model_equation,
    QWidget *parent
) :
  QDialog(parent),
  ui(new Ui::ModelEquationDialog)
{
  ui->setupUi(this);
  ui->txt_model_eq_area->insertPlainText(model_equation);
}

ModelEquationDialog::~ModelEquationDialog()
{
  delete ui;
}
