#ifndef UTIL_HEXAGON_H
#define UTIL_HEXAGON_H

#include <QPointF>
#include <QPolygonF>
#include <cmath>

namespace util {

/**
 * Class Hexagon
 *
 * Represent an hexagon figure. The hexagon is a polygon with the following
 * 6 points:
 *
 *         1
 *      /  |  \
 *    /    |    \
 *   6-----+-----2
 *   |     |     |
 *   |     M     |
 *   |     |     |
 *   5-----+-----3
 *    \    |    /
 *      \  |  /
 *         4
 *
 * The hexagon measures are the following:
 *
 *         ^
 *      /  |  \  C
 *    /    A    \
 *   +-----+--B--+
 *   |     |     |
 *   |     |     |
 *   |     |     |
 *   +-----+-----+
 *    \    |    /
 *      \  |  /
 *         v
 *
 * where C is the side measure that must be passed in the constructor. Other
 * measures are computed as follows:
 *   - A = 0.5 *C
 *   - B = sin(60) * C
 *
 * The height is 2*C, the width is 2*B, then the center of the hexagon is at
 * point (B, C).
 *
 * The point coordinates are taken by the following coordinate system:
 *
 *     |                   x
 *   --+-----1--------------->
 *     |  /  |  \
 *     |/    |    \
 *     6-----+-----2
 *     |     |     |
 *     |     M     |
 *     |     |     |
 *     5-----+-----3
 *     |\    |    /
 *     |  \  |  /
 *     |     4
 *  y  |
 *     v
 *
 */
class Hexagon
{
  public:
    //! Constructor
    Hexagon(qreal side) :
      m_C(side),
      m_A(0.5 * m_C),
      m_B(m_C * std::sqrt(3.0) / 2.0)
    { }

    //! Destructor
    ~Hexagon()
    { }

    //! The x coordinate of the point 1
    qreal get1x() const { return 0.0; }

    //! The y coordinate of the point 1
    qreal get1y() const { return m_A + m_C; }

    //! The x coordinate of the point 2
    qreal get2x() const { return 0.0; }

    //! The y coordinate of the point 2
    qreal get2y() const { return m_A; }

    //! The x coordinate of the point 3
    qreal get3x() const { return m_B; }

    //! The y coordinate of the point 3
    qreal get3y() const { return 0.0; }

    //! The x coordinate of the point 4
    qreal get4x() const { return 2.0 * m_B; }

    //! The y coordinate of the point 4
    qreal get4y() const { return m_A; }

    //! The x coordinate of the point 5
    qreal get5x() const { return 2.0 * m_B; }

    //! The y coordinate of the point 5
    qreal get5y() const { return m_A + m_C; }

    //! The x coordinate of the point 6
    qreal get6x() const { return m_B; }

    //! The y coordinate of the point 6
    qreal get6y() const { return 2 * m_C; }

    //! A size
    qreal getA() const { return m_A; }

    //! B size
    qreal getB() const { return m_B; }

    //! C size
    qreal getC() const { return m_C; }

    //! Height
    qreal getHeight() const { return 2.0 * m_C; }

    //! The x coordinate of the center point M
    qreal getMx() const { return m_B; }

    //! The y coordinate of the center point M
    qreal getMy() const { return m_C; }

    //! A QPolygonF representing this hexagon
    QPolygonF getQPolygonF() const
    {
      QPolygonF polygon;
      polygon
          << QPointF(get1x(), get1y())
          << QPointF(get2x(), get2y())
          << QPointF(get3x(), get3y())
          << QPointF(get4x(), get4y())
          << QPointF(get5x(), get5y())
          << QPointF(get6x(), get6y())
          << QPointF(get1x(), get1y());
      return polygon;
    }

    //! Side size (C)
    qreal getSide() const { return m_C; }

    //! Width
    qreal getWidth() const { return 2.0 * m_B; }

  private:
    qreal m_C, m_A, m_B;

}; // class Hexagon

} // namespace util

#endif // UTIL_HEXAGON_H
