#include "graphicshexcell.h"

#include <iostream>
#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

namespace gui {

/**
 * Constructor
 *
 * Builds an hexagonal cell with side of SIDE points.
 */
GraphicsHexCell::GraphicsHexCell(uint row, uint col, float side) :
  m_hex(side),
  m_row(row),
  m_col(col),
  m_on_enter_z_value_bak(0.0)
{
  setAcceptHoverEvents(true);
  setPolygon(m_hex.getQPolygonF());
  setPen(Qt::NoPen);
  setBrush(Qt::NoBrush);

  // On hover default pen
  m_on_hover_pen = pen();
  m_on_hover_pen.setStyle(Qt::SolidLine);
  m_on_hover_pen.setBrush(Qt::lightGray);

  return;
} // constructor

/**
 * Destructor
 *
 */
GraphicsHexCell::~GraphicsHexCell()
{ }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method getHexagon
 *
 */
const util::Hexagon& GraphicsHexCell::getHexagon() const
{
  return m_hex;
} // method getHexagon

/**
 * Method setOnHoverPen
 *
 */
void GraphicsHexCell::setOnHoverPen(const QPen& on_hover_pen)
{
  m_on_hover_pen = on_hover_pen;
  return;
} // method setOnHoverPen

// =================
// PROTECTED METHODS
// =================

/**
 * Method hoverEnterEvent
 *
 */
void GraphicsHexCell::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  // Change pen (to highlight the cell)
  m_on_enter_pen_bak = pen();
  setPen(m_on_hover_pen);

  m_on_enter_z_value_bak = zValue();
  setZValue(m_on_enter_z_value_bak + 0.5);

  QGraphicsItem::hoverEnterEvent(event);

  return;
} // method hoverEnterEvent

/**
 * Method hoverLeaveEvent
 *
 */
void GraphicsHexCell::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  // Restore "on enter" pen
  setPen(m_on_enter_pen_bak);

  // Restore z value
  setZValue(m_on_enter_z_value_bak);

  QGraphicsItem::hoverEnterEvent(event);

  return;
} // method hoverLeaveEvent

/**
 * Method mouseDoubleClickEvent
 *
 */
void GraphicsHexCell::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    emit showUnitInfo(m_row, m_col);
  return;
} // method mouseDoubleClickEvent

} // namespace gui
