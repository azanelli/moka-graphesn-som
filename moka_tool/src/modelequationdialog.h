#ifndef MODELEQUATIONDIALOG_H
#define MODELEQUATIONDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ModelEquationDialog;
}

class ModelEquationDialog : public QDialog
{
    Q_OBJECT
    
  public:
    explicit ModelEquationDialog(QString eqation, QWidget *parent = 0);
    ~ModelEquationDialog();
    
  private:
    Ui::ModelEquationDialog *ui;
};

#endif // MODELEQUATIONDIALOG_H
