#include "unitinfowindow.h"
#include "ui_unitinfowindow.h"

#include <vector>
#include <QColor>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QTableWidgetItem>
#include <QTextItem>
#include <QWidget>

#include "util/hexagon.h"

namespace gui {

/**
 * Constructor
 */
UnitInfoWindow::UnitInfoWindow
(
    const util::UnitInfo& unit_info,
    QWidget *parent
) :
  QMainWindow(parent),
  m_ui(new Ui::UnitInfoWindow),
  m_few_elements(true)
{
  m_ui->setupUi(this);
  showUnitInfo(unit_info);

  if (m_few_elements)
  {
    setMaximumWidth(std::max(this->width(), 270));
    setMaximumHeight(180);
  }

  return;
} // constructor

/**
 * Destructor
 */
UnitInfoWindow::~UnitInfoWindow()
{
  delete m_ui;
  return;
} // destructor

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method addLabelView
 *
 * Add the passed label in the user interface. The LABEL_NUMBER must start
 * from 1 for the first label to add and go on.
 */
void UnitInfoWindow::addLabelView
(
    const util::UnitInfo::Label& label,
    uint label_number
)
{
  QString label_name = label.getName();
  label_name.replace(QString("_"), QString(" "));
  if (label_name.isEmpty())
    label_name = "Unknown";

  QWidget *widget = new QWidget(m_ui->tbx_other_labels);

  QHBoxLayout *widget_hlayout = new QHBoxLayout(widget);
  widget->setLayout(widget_hlayout);
  widget->layout()->setContentsMargins(0, 0, 0, 0);

  QTableWidget *table = new QTableWidget(label.getSize(), 2, widget);
  widget->layout()->addWidget(table);
  for (size_t v = 0; v < label.getSize(); ++v)
  {
    table->setItem(
          v, 0, new QTableWidgetItem(label.getVal(v)));
    table->setItem(
          v, 1, new QTableWidgetItem(QString::number(label.getFreq(v))));
  } // for v
  table->setHorizontalHeaderItem(0, new QTableWidgetItem("Value"));
  table->setHorizontalHeaderItem(1, new QTableWidgetItem("Frequency"));
  adjustLabelTableWidget(table);

  // The first label is inserted in the already existing page
  if (label_number == 1)
  {
    m_ui->tbx_other_labels->setItemText(0, label_name);
    m_ui->wdg_label_1_page_layout->addWidget(widget);
  }
  else
    m_ui->tbx_other_labels->addItem(widget, label_name);

  return;
} // method addLabelView

/**
 * Method adjustLabelTableWidget
 *
 */
void UnitInfoWindow::adjustLabelTableWidget(QTableWidget *table) const
{
  if (table->columnCount() < 2)
  {
    qDebug() << "UnitInfoWindow::adjustLabelTableWidget: warning: try to "
             << "adjust a table with less than 2 columns.";
    return;
  }

  table->setFrameShape(QFrame::Box);
  table->setFrameShadow(QFrame::Plain);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  table->setSelectionMode(QAbstractItemView::NoSelection);

  table->verticalHeader()->setVisible(false);
  table->horizontalHeader()->setVisible(true);
  table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  table->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);

  table->scrollToTop();

  return;
} // method adjustLabelTableWidget

/**
 * Method paintUnit
 *
 */
void UnitInfoWindow::paintUnit
(
    uint freq_pos,
    uint freq_neg,
    QString label_top,
    QString label_bottom
)
{
  // Hexagon
  util::Hexagon hex(35);

  // Histograms
  float hist_h = 30.0, hist_w = 10.0;
  float hist_pos_h = 1.0, hist_neg_h = 1.0;
  if (freq_pos != 0 || freq_neg != 0)
  {
    hist_pos_h = hist_h * (freq_pos / float(freq_pos + freq_neg)) + 1.0;
    hist_neg_h = hist_h * (freq_neg / float(freq_pos + freq_neg)) + 1.0;
  }
  QRectF hist_pos_rect
      (
        hex.getMx() - hist_w * (1.0 / 4.0) - hist_w,     // left
        hex.getMy() + hist_h * (1.0 / 3.0) - hist_pos_h, // top
        hist_w,                                          // width
        hist_pos_h                                       // height
      );
  QRectF hist_neg_rect
      (
        hex.getMx() + hist_w * (1.0 / 4.0),              // left
        hex.getMy() + hist_h * (1.0 / 3.0) - hist_neg_h, // top
        hist_w,                                          // width
        hist_neg_h                                       // height
      );

  // Labels
  QString label_top_text = label_top;
  QString label_bottom_text =
      (label_bottom.isEmpty() ? "" : "(" + label_bottom + ")");
  QRectF label_top_rect
      (
        hex.getMx() - hex.getB(), // left
        (1.0 / 3.0) * hex.getC(), // top
        hex.getWidth(),           // width
        (2.0 / 3.0) * hex.getC()  // height
      );
  QRectF label_bottom_rect
      (
        hex.getMx() - hex.getB(), // left
        hex.getC() + 3.0,         // top
        hex.getWidth(),           // width
        (2.0 / 3.0) * hex.getC()  // height
      );

  // Pixmap
  QPixmap pixmap(hex.getWidth() + 2.0, hex.getHeight() + 2.0);
  pixmap.fill(QColor("transparent"));

  // Start to paint
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::SolidLine);
  painter.setBrush(QBrush(QColor("#E4E4E4"))); // light gray
  painter.drawPolygon(hex.getQPolygonF());
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(QColor("#FECC8B"))); // light orange
  painter.drawRect(hist_pos_rect);
  painter.setBrush(QBrush(QColor("#BDFDFF"))); // light cyan
  painter.drawRect(hist_neg_rect);
  painter.setPen(Qt::SolidLine);
  painter.setFont(QFont("Helvetica", 8));
  painter.drawText(label_top_rect, Qt::AlignCenter, label_top_text);
  painter.drawText(label_bottom_rect, Qt::AlignCenter, label_bottom_text);

  m_ui->lbl_unit_pixmap->setPixmap(pixmap);

  return;
} // method paintUnit

/**
 * Method showUnitInfo
 *
 */
void UnitInfoWindow::showUnitInfo(const util::UnitInfo& uinf)
{
  const std::vector<util::UnitInfo::Label> &labels = uinf.getLabels();
  size_t output_i = 0, atom_sym_i = 0;
  uint freq_pos = 0, freq_neg = 0;
  QString atom_sym_val, atom_sym_freq;

  // Unit (row, col)
  m_ui->lbl_unit_value->setText
      (
        "(" + QString::number(uinf.getRow()) +
        ", " + QString::number(uinf.getCol()) + ")"
      );

  // Weight
  m_ui->lbl_weight_value->setText(QString::number(uinf.getWeight(), 'g', 6));

  // Outputs
  for (output_i = 0; output_i < labels.size(); ++output_i)
    if (labels[output_i].getName().toUpper() == "OUTPUT 1")
      break;
  if (output_i == labels.size() || labels[output_i].getSize() > 2 ||
      labels[output_i].getSize() <= 0)
  {
    m_ui->lbl_output_pos_value->setText("0");
    m_ui->lbl_output_neg_value->setText("0");
  }
  else
  {
    double output_val_0 = labels[output_i].getVal(0).toDouble();
    if (labels[output_i].getSize() == 1)
    {
      if (output_val_0 > 0)
        freq_pos = labels[output_i].getFreq(0);
      else
        freq_neg = labels[output_i].getFreq(0);
    }
    else // labels[i].getSize() == 2
    {
      if (output_val_0 > 0)
      {
        freq_pos = labels[output_i].getFreq(0);
        freq_neg = labels[output_i].getFreq(1);
      }
      else
      {
        freq_pos = labels[output_i].getFreq(1);
        freq_neg = labels[output_i].getFreq(0);
      }
    } // if/else (labels[i].getSize() == 1)
    double perc_freq_pos = (freq_pos * 100.0) / double(freq_pos + freq_neg);
    double perc_freq_neg = (freq_neg * 100.0) / double(freq_pos + freq_neg);
    m_ui->lbl_output_pos_value->setText
        (
          QString::number(freq_pos) + " (" +
          QString::number(perc_freq_pos, 'f', 2) + "%)"
        );
    m_ui->lbl_output_neg_value->setText
        (
          QString::number(freq_neg) + " (" +
          QString::number(perc_freq_neg, 'f', 2) + "%)"
        );
  } // if/else (output_i == labels.size() ...)

  // Atom symbol
  for (atom_sym_i = 0; atom_sym_i < labels.size(); ++atom_sym_i)
    if (labels[atom_sym_i].getName().toUpper() == "ATOM SYMBOL")
      break;
  if (atom_sym_i == labels.size() || labels[atom_sym_i].getSize() == 0)
  {
    atom_sym_val = "";
    atom_sym_freq = "";
    m_ui->lbl_atom_symbol->setVisible(false);
    m_ui->tbl_atom_symbol->setVisible(false);
  }
  else
  {
    m_few_elements = false;

    m_ui->tbl_atom_symbol->setRowCount(labels[atom_sym_i].getSize());
    m_ui->tbl_atom_symbol->setColumnCount(2);

    for (size_t v = 0; v < labels[atom_sym_i].getSize(); ++v)
    {
      atom_sym_val = labels[atom_sym_i].getVal(v);
      atom_sym_freq = QString::number(labels[atom_sym_i].getFreq(v));
      m_ui->tbl_atom_symbol->setItem(
            v, 0, new QTableWidgetItem(atom_sym_val));
      m_ui->tbl_atom_symbol->setItem(
            v, 1, new QTableWidgetItem(atom_sym_freq));
    } // for v

    m_ui->tbl_atom_symbol->setHorizontalHeaderItem(
          0, new QTableWidgetItem("Value"));
    m_ui->tbl_atom_symbol->setHorizontalHeaderItem(
          1, new QTableWidgetItem("Frequency"));

    adjustLabelTableWidget(m_ui->tbl_atom_symbol);

    int table_rows = m_ui->tbl_atom_symbol->rowCount();
    if (table_rows > 0)
      m_ui->tbl_atom_symbol->setFixedHeight(
          30 + m_ui->tbl_atom_symbol->rowHeight(0) * table_rows);
    else // table_rows == 0
      m_ui->tbl_atom_symbol->setFixedHeight(50);

  } // if/else (atom_sym_i == labels.size() || ...)

  // Other labels
  uint other_labels_count = 0;
  for (size_t l = 0; l < labels.size(); ++l)
  {
    if (l == output_i || l == atom_sym_i || labels[l].getSize() == 0)
      continue;

    addLabelView(labels[l], ++other_labels_count);
    m_few_elements = false;

  } // for l

  if (other_labels_count == 0)
    m_ui->wdg_other_labels->setVisible(false);

  // Unit picture
  paintUnit(freq_pos, freq_neg, atom_sym_val, atom_sym_freq);

  return;
} // method showUnitInfo

} // namespace gui
