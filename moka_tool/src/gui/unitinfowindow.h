#ifndef GUI_UNITINFOWINDOW_H
#define GUI_UNITINFOWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

#include "util/unitinfo.h"

namespace gui {

namespace Ui
{
  class UnitInfoWindow;
}

/**
 * Class UnitInfoWindow
 *
 * Show a UnitInfo object in a new window.
 */
class UnitInfoWindow : public QMainWindow
{
    Q_OBJECT
    
  public:
    explicit UnitInfoWindow(
        const util::UnitInfo& unit_info,
        QWidget *parent = 0);
    virtual ~UnitInfoWindow();
    
  private:
    Ui::UnitInfoWindow *m_ui;
    bool m_few_elements;

    void addLabelView(
        const util::UnitInfo::Label& label,
        uint label_number
        );
    void adjustLabelTableWidget(QTableWidget *table) const;
    void paintUnit(
        uint freq_pos,
        uint freq_neg,
        QString label_top,
        QString label_bottom);
    void showUnitInfo(const util::UnitInfo& uinf);

}; // class UnitInfoWindow

} // namespace gui

#endif // GUI_UNITINFOWINDOW_H
