#ifndef GUI_GRIDVIEW_H
#define GUI_GRIDVIEW_H

#include <vector>
#include <QCursor>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPoint>
#include <QPointF>
#include <moka/model/graphesnsom.h>
#include <moka/ml/linearreadout.h>

#include "util/hexagon.h"
#include "util/unitinfo.h"

namespace gui {

/**
 * Class GMMGridView
 *
 * A QGraphicsView to visualizing an hexagonal grid for a GMM SOM.
 */
class GMMGridView : public QGraphicsView
{
    Q_OBJECT

  public:
    explicit GMMGridView(QWidget *parent = 0);
    virtual ~GMMGridView();

    virtual void addCellGrid();
    // virtual void addMarkers();
    virtual void addLabel(
        const std::vector< std::vector<util::UnitInfo> >& unit_info_map,
        const QString& label_name);
    virtual void addUmatrix(const std::vector< std::vector<float> >& umatrix);
    virtual void addUnitIndex();
    virtual void addWeights(
        const moka::model::GraphEsnSom::Matrix& readout_weights);
    virtual void clearView();
    virtual uint getGridCols() const;
    virtual uint getGridRows() const;
    virtual void initView(uint grid_rows, uint grid_cols);
    virtual void scaleView(qreal scaleFactor);

  signals:
    void showUnitInfo(uint row, uint col);

  protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

  private:
    // Graphics elements
    QGraphicsScene *m_current_scene;

    // Grid parameters
    const qreal m_grid_hex_side;
    const util::Hexagon m_grid_hex;
    uint m_grid_cols, m_grid_rows;

    // Event parameters
    bool m_ctrl_pressed;
    bool m_pan_active;
    QCursor m_pan_cursor_bak;
    QPoint m_pan_start_point;

    // Private method
    QPointF cellCenter(uint row, uint col);
    QPointF cellLeftCorner(uint row, uint col);
    void centerOnCell(QGraphicsItem *gitem, uint row, uint col);
    void placeTextOnBottom(QGraphicsTextItem *gtext, uint row, uint col);
    void placeTextOnCenter(QGraphicsTextItem *gtext, uint row, uint col);
    void placeTextOnTop(QGraphicsTextItem *gtext, uint row, uint col);

}; // class GMMGridView

} // namespace gui

#endif // GUI_GRIDVIEW_H
