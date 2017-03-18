#include "meleculereseultsdialog.h"
#include "ui_meleculereseultsdialog.h"

MeleculeReseultsDialog::MeleculeReseultsDialog
(
    const QString& molecule_id,
    QWidget *parent
) :
  QDialog(parent),
  ui(new Ui::MeleculeReseultsDialog)
{
  ui->setupUi(this);

  ui->lbl_molecule_id->setText(molecule_id);

  // Table
  QTableWidget *table = ui->tbl_results;

  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  table->setSelectionMode(QAbstractItemView::NoSelection);
  // table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  // table->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);

  table->verticalHeader()->setVisible(false);
  table->horizontalHeader()->setVisible(true);

  // Header
  table->setHorizontalHeaderItem(0, new QTableWidgetItem("Vertex"));
  table->setHorizontalHeaderItem(1, new QTableWidgetItem("Atom symbol"));
  table->setHorizontalHeaderItem(2, new QTableWidgetItem("Atom round (radius 1)"));
  table->setHorizontalHeaderItem(3, new QTableWidgetItem("Atom round (radius 2)"));
  table->setHorizontalHeaderItem(4, new QTableWidgetItem("Atom round (radius 3)"));
  table->setHorizontalHeaderItem(5, new QTableWidgetItem("Unit"));
  table->setHorizontalHeaderItem(6, new QTableWidgetItem("Weight"));

  table->setRowCount(9);

  // Row 0
  table->setItem(0, 0, new QTableWidgetItem("1")); // vertex
  table->setItem(0, 1, new QTableWidgetItem("c")); // atom symbol
  table->setItem(0, 2, new QTableWidgetItem("CC(=C)N")); // atom round 1
  table->setItem(0, 3, new QTableWidgetItem("C[NH]C(=C(N)C)C(=C)C")); // atom round 2
  table->setItem(0, 4, new QTableWidgetItem("CC=C(c1[nH]c(nc1C(=C)C)C)C=C")); // atom round 3
  table->setItem(0, 5, new QTableWidgetItem("(2,4)")); // unit
  table->setItem(0, 6, new QTableWidgetItem("0.2839")); // weight

  // Row 1
  table->setItem(1, 0, new QTableWidgetItem("2")); // vertex
  table->setItem(1, 1, new QTableWidgetItem("c")); // atom symbol
  table->setItem(1, 2, new QTableWidgetItem("CC(=C)N")); // atom round 1
  table->setItem(1, 3, new QTableWidgetItem("C[NH]C(=C(N)C)C(=C)C")); // atom round 2
  table->setItem(1, 4, new QTableWidgetItem("CC=C(c1nc([nH]c1C(=C)C)C)C=C")); // atom round 3
  table->setItem(1, 5, new QTableWidgetItem("(2,4)")); // unit
  table->setItem(1, 6, new QTableWidgetItem("0.2839")); // weight

  // Row 2
  table->setItem(2, 0, new QTableWidgetItem("3")); // vertex
  table->setItem(2, 1, new QTableWidgetItem("c")); // atom symbol
  table->setItem(2, 2, new QTableWidgetItem("CC(=C)N")); // atom round 1
  table->setItem(2, 3, new QTableWidgetItem("C[CH]=C(C(=C)N)[CH]=C")); // atom round 2
  table->setItem(2, 4, new QTableWidgetItem("CNC(=C(N)C)c1ccccc1")); // atom round 3
  table->setItem(2, 5, new QTableWidgetItem("(2,7)")); // unit
  table->setItem(2, 6, new QTableWidgetItem("0.0")); // weight

  // Row 3
  table->setItem(3, 0, new QTableWidgetItem("4")); // vertex
  table->setItem(3, 1, new QTableWidgetItem("n")); // atom symbol
  table->setItem(3, 2, new QTableWidgetItem("C[NH]C")); // atom round 1
  table->setItem(3, 3, new QTableWidgetItem("CC(=C)[NH]C(=N)C")); // atom round 2
  table->setItem(3, 4, new QTableWidgetItem("CC(=C)c1nc(c([nH]1)C(=C)C)C")); // atom round 3
  table->setItem(3, 5, new QTableWidgetItem("(19,26)")); // unit
  table->setItem(3, 6, new QTableWidgetItem("-0.0124")); // weight

  // Row 4
  table->setItem(4, 0, new QTableWidgetItem("5")); // vertex
  table->setItem(4, 1, new QTableWidgetItem("c")); // atom symbol
  table->setItem(4, 2, new QTableWidgetItem("C[CH]=C")); // atom round 1
  table->setItem(4, 3, new QTableWidgetItem("C[CH]=[CH]C(=C)C")); // atom round 2
  table->setItem(4, 4, new QTableWidgetItem("NC(=C)c1ccccc1")); // atom round 3
  table->setItem(4, 5, new QTableWidgetItem("(7,3)")); // unit
  table->setItem(4, 6, new QTableWidgetItem("0.0")); // weight

  // Row 5
  table->setItem(5, 0, new QTableWidgetItem("6")); // vertex
  table->setItem(5, 1, new QTableWidgetItem("O")); // atom symbol
  table->setItem(5, 2, new QTableWidgetItem("C[OH]")); // atom round 1
  table->setItem(5, 3, new QTableWidgetItem("CC(=C)[OH]")); // atom round 2
  table->setItem(5, 4, new QTableWidgetItem("CC(=C(C(=C)N)O)C")); // atom round 3
  table->setItem(5, 5, new QTableWidgetItem("(1,11)")); // unit
  table->setItem(5, 6, new QTableWidgetItem("0.2839")); // weight

  // Row 6
  table->setItem(6, 0, new QTableWidgetItem("7")); // vertex
  table->setItem(6, 1, new QTableWidgetItem("H")); // atom symbol
  table->setItem(6, 2, new QTableWidgetItem("[CH]")); // atom round 1
  table->setItem(6, 3, new QTableWidgetItem("C[CH]=C")); // atom round 2
  table->setItem(6, 4, new QTableWidgetItem("CC=CC(=C)C")); // atom round 3
  table->setItem(6, 5, new QTableWidgetItem("(21,12)")); // unit
  table->setItem(6, 6, new QTableWidgetItem("0.2839")); // weight

  // Row 7
  table->setItem(7, 0, new QTableWidgetItem("8")); // vertex
  table->setItem(7, 1, new QTableWidgetItem("C")); // atom symbol
  table->setItem(7, 2, new QTableWidgetItem("C[CH2]C")); // atom round 1
  table->setItem(7, 3, new QTableWidgetItem("C[CH2][CH2][CH3]")); // atom round 2
  table->setItem(7, 4, new QTableWidgetItem("CCCCN")); // atom round 3
  table->setItem(7, 5, new QTableWidgetItem("(5,6)")); // unit
  table->setItem(7, 6, new QTableWidgetItem("0.2839")); // weight

  // Row 8
  table->setItem(8, 0, new QTableWidgetItem("9")); // vertex
  table->setItem(8, 1, new QTableWidgetItem("O")); // atom symbol
  table->setItem(8, 2, new QTableWidgetItem("C=O")); // atom round 1
  table->setItem(8, 3, new QTableWidgetItem("CC(=O)O")); // atom round 2
  table->setItem(8, 4, new QTableWidgetItem("COC(=O)C(O)(C)C")); // atom round 3
  table->setItem(8, 5, new QTableWidgetItem("(3,21)")); // unit
  table->setItem(8, 6, new QTableWidgetItem("0.0")); // weight

  // Finish
  table->resizeColumnsToContents();
  table->scrollToTop();

  return;
}

MeleculeReseultsDialog::~MeleculeReseultsDialog()
{
  delete ui;
}
