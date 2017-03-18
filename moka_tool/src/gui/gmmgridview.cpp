#include "gmmgridview.h"

#include <cmath>
#include <QDebug>
#include <QFont>
#include <QtGui>

#include "gui/graphicshexcell.h"

namespace gui {

namespace mmo = ::moka::model;

/**
 * Constructor
 *
 */
GMMGridView::GMMGridView(QWidget *parent) :
  QGraphicsView(parent),
  m_current_scene(NULL),
  m_grid_hex_side(50.0),
  m_grid_hex(m_grid_hex_side),
  m_grid_cols(0),
  m_grid_rows(0),
  m_ctrl_pressed(false),
  m_pan_active(false)
{
  // QWidget properties
  setFocusPolicy(Qt::WheelFocus);

  // QFrame properties
  setFrameShape(QFrame::NoFrame);
  setFrameShadow(QFrame::Plain);
  setLineWidth(0);

  // QGraphicsView properties
  setCacheMode(CacheBackground);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  m_current_scene = new QGraphicsScene(this);
  setScene(m_current_scene);

  return;
} // constructor

/**
 * Destructor
 *
 */
GMMGridView::~GMMGridView()
{
  delete m_current_scene;
  return;
}

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method addCellGrid
 *
 */
void GMMGridView::addCellGrid()
{
  util::Hexagon hex_grid(m_grid_hex_side);
  QPen grid_pen(Qt::black);
  QBrush grid_brush(Qt::NoBrush);

  for (uint r = 0; r < m_grid_rows; ++r)
    for (uint c = 0; c < m_grid_cols; ++c)
    {
      QGraphicsPolygonItem *grid_item =
          scene()->addPolygon(hex_grid.getQPolygonF(), grid_pen, grid_brush);
      centerOnCell(grid_item, r, c);
    } // for (r,c)

  return;
} // method addCellGrid

/**
 * Method addLabel
 *
 * Take from UNIT_INFO_MAP the label with name LABEL_NAME and add in each cell
 * the most frequent value and the frequency. If one cell has not such label
 * or has no values then no label is added on it.
 */
void GMMGridView::addLabel
(
    const std::vector< std::vector<util::UnitInfo> >& unit_info_map,
    const QString& label_name
)
{
  // Check unit info map size
  if (unit_info_map.size() == 0 || unit_info_map[0].size() == 0)
  {
    qDebug() << "GMMGridView::addLabel: error: empty unit info map.";
    return;
  }

  if (unit_info_map.size() != m_grid_rows ||
      unit_info_map[0].size() != m_grid_cols)
  {
    qDebug() << "GMMGridView::addLabel: error: wrong unit info map size.";
    return;
  }

  for (uint r = 0; r < m_grid_rows; ++r)
    for (uint c = 0; c < m_grid_cols; ++c)
    {
      QGraphicsTextItem *unit_index = scene()->addText("");
      unit_index->setHtml
          (
            QString() +
            "<center>" +
            "(" + QString::number(r) + "," + QString::number(c) + ")" +
            "</center>"
          );

      unit_index->setFont(QFont("Helvetica", 11));
      placeTextOnBottom(unit_index, r, c);

      size_t l = unit_info_map[r][c].searchLabel(label_name);

      if (l == unit_info_map[r][c].getLabelsSize())
        continue;

      if (unit_info_map[r][c].getLabel(l).getSize() == 0)
        continue;

      QGraphicsTextItem *label_val = scene()->addText("");
      label_val->setHtml
          (
            "<center>" +
            unit_info_map[r][c].getLabel(l).getVal(0) +
            " (" +
            QString::number(unit_info_map[r][c].getLabel(l).getFreq(0)) +
            ")</center>"
          );
      label_val->setFont(QFont("Helvetica", 12));
      placeTextOnTop(label_val, r, c);

//      QGraphicsTextItem *label_freq = scene()->addText("");
//      label_freq->setHtml
//          (
//            "<center> (" +
//            QString::number(unit_info_map[r][c].getLabel(l).getFreq(0)) +
//            ") </center>"
//          );
//      label_freq->setFont(QFont("Helvetica", 11));
//      placeTextOnBottom(label_freq, r, c);

    } // for (r,c)

  return;
} // method addLabel

/**
 * Method addUnitIndex
 *
 */
void GMMGridView::addUnitIndex()
{
  for (uint r = 0; r < m_grid_rows; ++r)
    for (uint c = 0; c < m_grid_cols; ++c)
    {
      QGraphicsTextItem *unit_index = scene()->addText("");
      unit_index->setHtml
          (
            QString() +
            "<center>" +
            "(" + QString::number(r) + "," + QString::number(c) + ")" +
            "</center>"
          );

      unit_index->setFont(QFont("Helvetica", 9));
      placeTextOnCenter(unit_index, r, c);
    } // for (r,c)

  return;
} // method addUnitIndex

/**
 * Method addUmatrix
 *
 */
void GMMGridView::addUmatrix(const std::vector< std::vector<float> >& umatrix)
{
  // Check umatrix size
  if (umatrix.size() == 0 || umatrix[0].size() == 0)
  {
    qDebug() << "GMMGridView::addUmatrix: error: empty umatrix.";
    return;
  }

  if (umatrix.size() != (2 * m_grid_rows - 1))
  {
    qDebug() << "GMMGridView::addUmatrix: error: wrong umatrix no rows.";
    return;
  }

  if (umatrix[0].size() != (2 * m_grid_cols - 1))
  {
    qDebug() << "GMMGridView::addUmatrix: error: wrong umatrix no columns.";
    return;
  }

  util::Hexagon uhex(m_grid_hex_side / 2.0);

  for (size_t r = 0; r < umatrix.size(); ++r)
    for (size_t c = 0; c < umatrix[r].size(); ++c)
    {
      float umat_gray = 255 * (umatrix[r][c] * 0.6 + 0.4);
      QColor uhex_color(umat_gray, umat_gray, umat_gray);
      QPen uhex_pen(uhex_color);
      QBrush uhex_brush(uhex_color, Qt::SolidPattern);

      if (r % 4 < 3)
        scene()->addPolygon(uhex.getQPolygonF(), uhex_pen, uhex_brush)->setPos
            (
              c * uhex.getWidth() + uhex.getB() + uhex.getB() * (r % 4), // x
              r * (uhex.getA() + uhex.getC()) + (2.0 * uhex.getA())      // y
            );
      else
        scene()->addPolygon(uhex.getQPolygonF(), uhex_pen, uhex_brush)->setPos
            (
              c * uhex.getWidth() + uhex.getWidth(),                  // x
              r * (uhex.getA() + uhex.getC()) + + (2.0 * uhex.getA()) // y
            );

    } // for (r,c)

  return;
} // method addUmatrix

/**
 * Method addWeights
 *
 */
void GMMGridView::addWeights(const mmo::GraphEsnSom::Matrix& readout_weights)
{
  // Some checks
  if (readout_weights.size() == 0)
  {
    qDebug() << "GMMGridView::addWeights: readout weights empty.";
    return;
  }

  if (readout_weights.n_rows > 1)
  {
    qDebug() << "GMMGridView::addWeights: too many readout units.";
    return;
  }

  if (readout_weights.n_cols != (m_grid_rows * m_grid_cols + 1))
  {
    qDebug() << "GMMGridView::addWeights: wrong number of weights.";
    return;
  }

  qreal bias = readout_weights.at(0, 0);
  std::vector<qreal> weights_norm(readout_weights.n_cols - 1);

  // Get absolute values and max abs value
  qreal max_weight = 0.0;
  for (size_t i = 0; i < weights_norm.size(); ++i)
  {
    weights_norm[i] = std::abs(readout_weights.at(0, i + 1));

    if (weights_norm[i] > max_weight)
      max_weight = weights_norm[i];

  } // for i

  // Normalize weights
  if (max_weight != 0.0)
    for (size_t i = 0; i < weights_norm.size(); ++i)
      weights_norm[i] = weights_norm[i] / max_weight;

  // Put the weights in the grid (note that the weights are in the vector by
  // rows, i.e. first the first row, then the second row and so on).
  for (uint r = 0, i = 0; r < m_grid_rows; ++r)
    for (uint c = 0; c < m_grid_cols; ++c, ++i)
    {
      // Make weight color
      QColor weight_color;
      if (bias >= 0)
      {
        if (readout_weights.at(0, i + 1) >= 0)
          weight_color.setNamedColor("#FECC8B"); // light orange
        else if (std::abs(readout_weights.at(0, i + 1)) < bias)
          weight_color.setNamedColor("#C2FEBD"); // light green
        else
          weight_color.setNamedColor("#BDFDFF"); // light cyan
      }
      else
      {
        if (readout_weights.at(0, i + 1) < 0)
          weight_color.setNamedColor("#BDFDFF"); // light cyan
        else if (readout_weights.at(0, i + 1) < std::abs(bias))
          weight_color.setNamedColor("#FECC8B"); // light orange
        else
          weight_color.setNamedColor("#FFA992"); // light red
      } // else-if

      // Pen and brush
      QPen wpen(weight_color);
      QBrush wbrush(weight_color, Qt::SolidPattern);

      // Weight hexagon
      util::Hexagon whex(std::sqrt(weights_norm[i] * 0.95) * m_grid_hex_side);

      // Add the weight to the scene
      centerOnCell(
          scene()->addPolygon(whex.getQPolygonF(), wpen, wbrush), r , c);

    } // for (r,c)

  return;
} // method addWeights

/**
 * Method clearView
 *
 * Clear the scene by deleting all its items. If you call this method you will
 * need to recall the method initView in order to re-initialize the scene.
 */
void GMMGridView::clearView()
{
  m_grid_cols = 0;
  m_grid_rows = 0;

  delete m_current_scene;
  m_current_scene = new QGraphicsScene(this);
  setScene(m_current_scene);

  return;
} // method clearView

/**
 * Method getGridCols
 *
 */
uint GMMGridView::getGridCols() const
{
  return m_grid_cols;
}

/**
 * Method getGridRows
 *
 */
uint GMMGridView::getGridRows() const
{
  return m_grid_rows;
}

/**
 * Method initView
 *
 * Init the scene area. To call before any "add__" method.
 */
void GMMGridView::initView(uint grid_rows, uint grid_cols)
{
  clearView();

  util::Hexagon hexagon(m_grid_hex_side);

  m_grid_rows = grid_rows;
  m_grid_cols = grid_cols;

  qreal grid_w = (grid_cols * hexagon.getWidth()) + hexagon.getB();
  qreal grid_h = grid_rows * (hexagon.getA() + hexagon.getC()) + hexagon.getA();
  if (grid_rows % 2 != 0)
    grid_h += (hexagon.getA() + hexagon.getC());

  scene()->setSceneRect
      (
        -m_grid_hex_side, // x
        -m_grid_hex_side, // y
        grid_w + 2 * m_grid_hex_side, // w
        grid_h + 2 * m_grid_hex_side  // h
      );

  fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);

  // Init the graphics cell grid
  for (uint r = 0; r < m_grid_rows; ++r)
    for (uint c = 0; c < m_grid_cols; ++c)
    {
      // Builds the graphics cell
      GraphicsHexCell *hex_cell = new GraphicsHexCell(r, c, m_grid_hex_side);
      centerOnCell(hex_cell, r, c);
      scene()->addItem(hex_cell);

      // Connect signals
      QObject::connect(
          hex_cell,
          SIGNAL(showUnitInfo(uint,uint)),
          this,
          SIGNAL(showUnitInfo(uint,uint)));

    } // for (r,c)

  return;
} // method initView

/**
 * Method scaleView
 *
 */
void GMMGridView::scaleView(qreal scale_factor)
{
  qreal factor =
      transform().scale(scale_factor, scale_factor)
      .mapRect(QRectF(0, 0, 1, 1)).width();

  if (factor < 0.01 || factor > 10)
    return;

  scale(scale_factor, scale_factor);

  return;
} // method scaleView

// =================
// PROTECTED METHODS
// =================

/**
 * Method keyPressEvent
 *
 */
void GMMGridView::keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Control:
    m_ctrl_pressed = true;
    QGraphicsView::keyPressEvent(event);
    break;

  case Qt::Key_Plus:
    scaleView(1.2);
    break;

  case Qt::Key_Minus:
    scaleView(1.0 / 1.2);
    break;

  case Qt::Key_Space:
    m_pan_active = true;
    m_pan_cursor_bak = cursor();
    setCursor(Qt::OpenHandCursor);
    m_pan_start_point = QCursor::pos();
    // QGraphicsView::keyPressEvent(event);
    break;

  default:
    QGraphicsView::keyPressEvent(event);
  } // switch

  return;
} // method keyPressEvent

/**
 * Method keyReleaseEvent
 *
 */
void GMMGridView::keyReleaseEvent(QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Control:
    m_ctrl_pressed = false;
    QGraphicsView::keyReleaseEvent(event);
    break;

  case Qt::Key_Space:
    m_pan_active = false;
    setCursor(m_pan_cursor_bak);
    // QGraphicsView::keyReleaseEvent(event);
    break;

  default:
    QGraphicsView::keyReleaseEvent(event);
    break;
  } // switch

  return;
} // method keyReleaseEvent

/**
 * Method mouseMoveEvent
 *
 */
void GMMGridView::mouseMoveEvent(QMouseEvent *event)
{
  if (m_pan_active)
  {
    horizontalScrollBar()->setValue
        (
          horizontalScrollBar()->value() -
          (event->globalX() - m_pan_start_point.x())
        );

    verticalScrollBar()->setValue
        (
          verticalScrollBar()->value() -
          (event->globalY() - m_pan_start_point.y())
        );

    m_pan_start_point.setX(event->globalX());
    m_pan_start_point.setY(event->globalY());
  }

  QGraphicsView::mouseMoveEvent(event);

  return;
} // method mouseMoveEvent

/**
 * Method wheelEvent
 *
 */
void GMMGridView::wheelEvent(QWheelEvent *event)
{
  if (m_ctrl_pressed)
    scaleView(pow(2.0, event->delta() / 240.0));
  else
    QGraphicsView::wheelEvent(event);
  return;
} // method wheelEvent

// ==============
// PRIVATE METHOD
// ==============

/**
 * Method cellCenter
 *
 */
QPointF GMMGridView::cellCenter(uint row, uint col)
{
  return QPointF
      (
        (2 * col + 1) * m_grid_hex.getB() + ((row % 2) * m_grid_hex.getB()),
        (row) * (m_grid_hex.getC() + m_grid_hex.getA()) + m_grid_hex.getC()
      );
} // method cellCenter

/**
 * Method cellLeftCorner
 *
 */
QPointF GMMGridView::cellLeftCorner(uint row, uint col)
{
  QPointF point = cellCenter(row, col);
  point.setX(point.x() - m_grid_hex_side);
  point.setY(point.y() - m_grid_hex_side);
  return point;
} // cellLeftCorner

/**
 * Method centerOnCell
 *
 * Given a QGraphicsItem set its position (using the method setPos and the
 * method boundingRect to get its sizes) so that it is centered on the cell
 * (ROW, COL).
 */
void GMMGridView::centerOnCell(QGraphicsItem *gitem, uint row, uint col)
{
  if (!gitem)
    return;

  QPointF cell_center = cellCenter(row, col);
  gitem->setPos
      (
        cell_center.x() - (gitem->boundingRect().width() / 2.0),  // x
        cell_center.y() - (gitem->boundingRect().height() / 2.0)  // y
      );

  return;
} // method centerOnCell

/**
 * Method placeTextOnBottom
 *
 * Given a QGraphicsTextItem set its position so that it is on the bottom part
 * of the cell (ROW, COL).
 */
void GMMGridView::placeTextOnBottom
(
    QGraphicsTextItem *gtext,
    uint row,
    uint col
)
{
  if (!gtext)
    return;

  QPointF cell_center = cellCenter(row, col);
  gtext->setTextWidth(m_grid_hex.getWidth() * 0.85);
  gtext->setPos
      (
        cell_center.x() - (gtext->textWidth() / 2.0),  // x
        cell_center.y() + 2.0  // y
      );

  return;
} // method placeTextOnBottom

/**
 * Method placeTextOnCenter
 *
 * Given a QGraphicsTextItem set its position so that it is on the center of
 * the cell (ROW, COL).
 */
void GMMGridView::placeTextOnCenter
(
    QGraphicsTextItem *gtext,
    uint row,
    uint col
)
{
  if (!gtext)
    return;
  gtext->setTextWidth(m_grid_hex.getWidth() * 0.85);
  centerOnCell(gtext, row, col);
  return;
} // method placeTextOnCenter

/**
 * Method placeTextOnTop
 *
 * Given a QGraphicsTextItem set its position so that it is on the top part of
 * the cell (ROW, COL).
 */
void GMMGridView::placeTextOnTop(QGraphicsTextItem *gtext, uint row, uint col)
{
  if (!gtext)
    return;

  QPointF cell_center = cellCenter(row, col);
  gtext->setTextWidth(m_grid_hex.getWidth() * 0.85);
  gtext->setPos
      (
        cell_center.x() - (gtext->textWidth() / 2.0),  // x
        cell_center.y() - (m_grid_hex.getC() * 2.0 / 3.0)  // y
      );

  return;
} // method placeTextOnTop

} // namespace gui
