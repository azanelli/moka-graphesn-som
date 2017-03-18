#ifndef MELECULERESEULTSDIALOG_H
#define MELECULERESEULTSDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class MeleculeReseultsDialog;
}

class MeleculeReseultsDialog : public QDialog
{
    Q_OBJECT
    
  public:
    explicit MeleculeReseultsDialog(
        const QString& molecule_id,
        QWidget *parent = 0);
    ~MeleculeReseultsDialog();
    
  private:
    Ui::MeleculeReseultsDialog *ui;
};

#endif // MELECULERESEULTSDIALOG_H
