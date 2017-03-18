#ifndef GUI_GMMVIEWER_H
#define GUI_GMMVIEWER_H

#include <vector>
#include <QString>
#include <QWidget>
#include <moka/model/graphesnsom.h>

#include "gui/gmmgridview.h"
#include "util/unitinfo.h"

namespace gui {

namespace Ui
{
  class GMMViewer;
}

/**
 * Class GMMViewer
 *
 */
class GMMViewer : public QWidget
{
    Q_OBJECT
    
  public:
    typedef std::vector< std::vector<util::UnitInfo> > UnitInfoMap;

    explicit GMMViewer(QWidget *parent = 0);
    ~GMMViewer();

    void bindGMM(moka::model::GraphEsnSom const *gmm);
    void bindUnitInfoMap(UnitInfoMap const *unit_info_map);
    void removeGMM();
    void removeUnitInfoMap();
    void setModelName(const QString& name);
    void setFocusOnGrid();
    void showGMM();

  signals:
    void closeViewer();
    void showUnitInfo(uint row, uint col);

  private:
    // User interface elements
    Ui::GMMViewer *m_ui;
    GMMGridView *m_gmm_grid_view;

    // GMM elements
    moka::model::GraphEsnSom const *m_gmm;
    QString m_model_name;
    UnitInfoMap const *m_unit_info_map;

    // Status members
    bool m_is_showing_grid;

    // Private method
    void initConnections();

  private slots:
    void on_btn_more_model_info_clicked();
    void on_btn_zoom_in_clicked();
    void on_btn_zoom_out_clicked();

}; // class GMMViewer

} // namespace gui

#endif // GUI_GMMVIEWER_H
