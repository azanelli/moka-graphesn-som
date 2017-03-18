#ifndef OB_SVGPAINTER_H
#define OB_SVGPAINTER_H

#include <iostream>
#include <openbabel/babelconfig.h>
#include <openbabel_ext/depict/painter.h>

namespace OpenBabel
{

class OBDEPICT SVGPainter : public OBPainter
{
  public:

    SVGPainter();

    SVGPainter
    (
        std::ostream& ofs,
        bool withViewBox = false,
        double width = 0.0,
        double height = 0.0,
        double x = 0.0,
        double y = 0.0
    ) :
      m_ofs(ofs),
      m_withViewBox(withViewBox),
      m_width(width),
      m_height(height),
      m_x(x),
      m_y(y),
      m_Pencolor("black"),
      m_Fillcolor("white"),
      m_PenWidth(1),
      m_fontPointSize(16)
    { }

    ~SVGPainter()
    {
      m_ofs << "</svg>\n";
      if(m_withViewBox)
        m_ofs << "</g>\n";
      return;
    }

    //! @name OBPainter methods

    void NewCanvas(double width, double height)
    {

      // Using withViewBox to supress xml header and xmlns attributes.
      // May need another way.
      if (!m_withViewBox)
        m_ofs << "<?xml version=\"1.0\"?>\n";

      if (m_withViewBox)
        m_ofs << "<g transform=\"translate(" << m_x << "," << m_y << ")\">\n";

      m_ofs << "<svg ";
      if (!m_withViewBox)
        m_ofs << "xmlns=\"http://www.w3.org/2000/svg\"\n"
                 "xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
                 "xmlns:cml=\"http://www.xml-cml.org/schema\" ";
      if (m_withViewBox)
        m_ofs << "width=\"" << m_width << "\" height=\"" << m_height << "\" "
              << "x=\"0\" y=\"0\" "
              << "viewBox=\"0 0 " << width << ' ' << height << "\"\n";
      else
        m_ofs << "width=\"" << width << "\" height=\"" << height << "\" "
              << "x=\"0\" y=\"0\" ";

      // Bond color and width are the initial m_Pencolor and m_PenWidth
      m_ofs << "font-family=\"" << m_fontFamily << "\" stroke="
            << MakeRGB(m_Pencolor)
            << "stroke-width=\"" << m_PenWidth << "\"  stroke-linecap=\"round\""
            << ">\n";
      // Background color for single molecule. Handled by outer svg when table.
      if (!m_withViewBox)
        m_ofs << "<rect x=\"0%\" y=\"0%\" width=\"100%\" height=\"100%\" "
              << "stroke-width=\"0\" fill="
              << MakeRGB(m_Fillcolor) << " />\n";
      m_OrigBondcolor = m_Pencolor;
      return;
    } // method NewCanvas

    bool IsGood() const
    {
      return true;
    }

    void SetFontFamily(const std::string &fontFamily)
    {
      m_fontFamily = fontFamily;
    }

    void SetFontSize(int pointSize)
    {
      m_fontPointSize = pointSize;
    }

    void SetFillColor(const OBColor &color)
    {
      m_Fillcolor = color; //value when NewCanvas called used for background
    }

    void SetPenColor(const OBColor &color)
    {
      m_Pencolor = color; //value when NewCanvas called used for bonds
    }

    void SetPenWidth(double width)
    {
      m_PenWidth = width; //value when NewCanvas called used for bonds
    }

    void DrawLine(double x1, double y1, double x2, double y2)
    {
      std::streamsize oldprec = m_ofs.precision(1);
      m_ofs << std::fixed << "<line x1=\"" << x1 << "\" y1=\""
            << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"";
      // TODO: Bring this line back once Pybel is fine with this
      // if(m_Pencolor!=m_OrigBondcolor)
      m_ofs << " stroke=" << MakeRGB(m_Pencolor);
      m_ofs << " stroke-width=\"" << m_PenWidth << "\"";
      m_ofs << "/>\n";
      m_ofs.precision(oldprec);
    }

    void DrawPolygon(const std::vector< std::pair<double, double> > &points)
    {
      m_ofs << "<polygon points=\"";
        std::vector<std::pair<double,double> >::const_iterator i;
      for (i = points.begin(); i != points.end(); ++i)
        m_ofs << i->first << ' ' << i->second << ' ';
      m_ofs << "\"";
      m_ofs << " stroke-width=\"" << m_PenWidth << "\"";
      m_ofs << " fill=" << MakeRGB(m_Pencolor);
      m_ofs << " stroke=" << MakeRGB(m_Pencolor);
      m_ofs << "/>\n";
    }

    void DrawCircle(double x, double y, double r, const OBColor& color = OBColor("black"))
    {
      m_ofs << "<circle cx=\"" << x << "\" cy=\"" << y << "\""
            << " r=\"" << r << "\""
            << " fill=" << MakeRGB(color)
            << " stroke=" << MakeRGB(color)
            << "/>\n";
    }

    void DrawText
    (
        double x,
        double y,
        const std::string &text,
        double width = 1.0
    )
    {
      m_ofs << "<text x=\"" << x << "\" y=\"" << y << "\""
        << " fill=" << MakeRGB(m_Pencolor) << " stroke="
        << MakeRGB(m_Pencolor) << "stroke-width=\"" << width <<"\" "
        << "font-size=\"" << m_fontPointSize << "\" >"
        << text << "</text>\n";
    }

    OBFontMetrics GetFontMetrics(const std::string &text)
    {
      OBFontMetrics metrics;
      metrics.fontSize = m_fontPointSize;
      metrics.ascent   = m_fontPointSize;
      // Offset from baseline of bottom of text
      metrics.descent  = m_fontPointSize * -0.23;
      // Distance between successive lines of text
      metrics.height   = m_fontPointSize *  1.26;
      metrics.width = 0.0;
      for (std::string::size_type i=0; i < text.size(); ++i)
        metrics.width += m_fontPointSize * (isalpha(text[i]) ? 0.75 : 0.5);
      return metrics;
    }

    //! @name CairoPainter specific
    //@{
    void WriteImage(const std::string&) { }
    //@}

  private:
    std::string MakeRGB(OBColor color)
    {
      std::stringstream ss;
      ss << "\"rgb(" << (int)(255*color.red) << ',' << (int)(255*color.green)
         << ',' << (int)(255*color.blue) << ")\" ";
      return ss.str();
    }

  private:
    std::ostream& m_ofs;
    bool m_withViewBox;
    double m_width, m_height, m_x, m_y;
    OBColor m_Pencolor;
    OBColor m_OrigBondcolor;
    OBColor m_Fillcolor;
    double m_PenWidth;
    int m_fontPointSize;
    std::string m_fontFamily;

}; // class SVGPainter

} // namespace OpenBabel

#endif

//! \file svgpainter.h
//! \brief Generate 2D depictions in the SVG vector graphics format.
