#ifndef GUI_MOKAWINDOW_H
#define GUI_MOKAWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QString>
#include <QThread>
#include <moka/global.h>
#include <moka/model/graphesnsom.h>
#include <moka/util/parameters.h>

#include "gui/gmmviewer.h"
#include "thread/loadgmm.h"
#include "thread/loadunitinfo.h"
#include "util/unitinfo.h"

namespace gui {

namespace Ui
{
  class MokaWindow;
}

/**
 * Class MokaWindow
 *
 */
class MokaWindow : public QMainWindow
{
    Q_OBJECT
    
  public:
    explicit MokaWindow(QWidget *parent = 0);
    virtual ~MokaWindow();
    
  protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void closeEvent(QCloseEvent *event);

  private:
    // User interface components
    Ui::MokaWindow *m_ui;
    GMMViewer *m_gmm_viewer;

    // GMM
    ::moka::model::GraphEsnSom m_gmm;
    QMutex m_gmm_mutex;
    bool m_gmm_is_loaded;
    QString m_gmm_filename;

    // Unit info map
    std::vector< std::vector<util::UnitInfo> > m_unit_info_map;
    QMutex m_unit_info_map_mutex;
    bool m_unit_info_is_loaded;
    QString m_unit_info_filename;

    // Dataset
    ::moka::util::Parameters m_dataset_parameters;
    bool m_dataset_cofig_is_loaded;

    // Threads
    thread::LoadGMM m_th_load_gmm;
    thread::LoadUnitInfo m_th_load_unit_info;

    // Private methods
    void enableViewModel(bool enable, QThread const *from_thread = 0);
    void handleEvent_lbl_load_dataset_config(QEvent *ev);
    void handleEvent_lbl_load_model(QEvent *ev);
    void handleEvent_lbl_load_unit_info(QEvent *ev);
    void initConnections();
    void loadSettings();
    void removeUnitInfo();
    void saveSettings() const;
    void setWindowTitle(const QString& prefix = "");

  private slots:
    void closeGMMViewer();
    void loadModelEnd(QString filepath);
    void loadModelError(QString reason);
    void loadModelStart(QString filepath);
    void loadUnitInfoEnd(QString filepath);
    void loadUnitInfoError(QString reason);
    void loadUnitInfoStart(QString filepath);
    void on_act_view_model_triggered();
    void on_btn_open_molecules_dock_clicked();
    void on_btn_molecules_calculate_clicked();
    void on_btn_molecules_open_clicked();
    void on_dck_molecules_visibilityChanged(bool visible);
    void showGMMViewer();
    void showUnitInfo(uint row, uint col);

}; // class MokaWindow

} // namespace gui

#endif // GUI_MOKAWINDOW_H
