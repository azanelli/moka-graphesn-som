#ifndef GUI_GRAPHICSHEXCELL_H
#define GUI_GRAPHICSHEXCELL_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QPen>

#include "util/hexagon.h"

namespace gui {

/**
 * Class GraphicsHexCell
 *
 * Represent an hexagonal cell for the graphical hexagonal grid. See the class
 * Hexagon for details on such polygon (e.g. measures and coordinates).
 */
class GraphicsHexCell : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

  public:
    GraphicsHexCell(uint row, uint col, float side);
    virtual ~GraphicsHexCell();

    const util::Hexagon& getHexagon() const;
    void setOnHoverPen(const QPen& on_hover_pen);

  signals:
    void showUnitInfo(uint row, uint col);

  protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

  private:
    util::Hexagon m_hex;
    uint m_row, m_col;
    QPen m_on_hover_pen;
    QPen m_on_enter_pen_bak;
    qreal m_on_enter_z_value_bak;

}; // GraphicsHexCell

} // namespace gui

#endif // GUI_GRAPHICSHEXCELL_H
