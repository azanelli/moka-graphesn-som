#include "gmmviewer.h"
#include "ui_gmmviewer.h"

#include "modelequationdialog.h"

namespace gui {

/**
 * Constructor
 *
 */
GMMViewer::GMMViewer(QWidget *parent) :
  QWidget(parent),
  m_ui(new Ui::GMMViewer),
  m_gmm_grid_view(new GMMGridView(NULL)),
  m_gmm(NULL),
  m_unit_info_map(NULL),
  m_is_showing_grid(false)
{
  // Init gui elements
  m_ui->setupUi(this);

  // Init connections
  initConnections();

  return;
} // constructor

/**
 * Destructor
 *
 */
GMMViewer::~GMMViewer()
{
  // m_gmm: is a bind (not to be deleted)
  // m_unit_info_map: is a bind (not to be deleted)
  delete m_gmm_grid_view;
  delete m_ui;
  return;
} // destructor

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method bindGMM
 *
 */
void GMMViewer::bindGMM(moka::model::GraphEsnSom const *gmm)
{
  m_gmm = gmm;
} // method bindGMM

/**
 * Method bindUnitInfoMap
 *
 */
void GMMViewer::bindUnitInfoMap(UnitInfoMap const *unit_info_map)
{
  m_unit_info_map = unit_info_map;
} // method bindUnitInfoMap

/**
 * Method removeGMM
 *
 */
void GMMViewer::removeGMM()
{
  m_gmm = NULL;
} // method removeGMM

/**
 * Method removeUnitInfoMap
 *
 */
void GMMViewer::removeUnitInfoMap()
{
  m_unit_info_map = NULL;
} // method removeUnitInfoMap

/**
 * Method setModelName
 *
 */
void GMMViewer::setModelName(const QString& name)
{
  m_model_name = name;
} // method setModelName

/**
 * Method setFocusOnGrid
 *
 */
void GMMViewer::setFocusOnGrid()
{
  m_gmm_grid_view->setFocus();
  return;
} // method setFocusOnGrid

/**
 * Method showGMM
 *
 */
void GMMViewer::showGMM()
{
  if (!m_gmm)
    return;

  // Init the grid view
  m_gmm_grid_view->initView(
      m_gmm->getSOM().getNoRows(), m_gmm->getSOM().getNoColumns());

  // Show the umatrix
  std::vector< std::vector<float> > umat;
  m_gmm->getSOM().umatrix(umat);
  m_gmm_grid_view->addUmatrix(umat);

  // Show readout weights
  m_gmm_grid_view->addWeights(m_gmm->getReadout().getReadoutMatrix());

  // Show the hexagonal grid
  m_gmm_grid_view->addCellGrid();

  if (!m_unit_info_map || m_unit_info_map->size() == 0 ||
      m_unit_info_map->size() != m_gmm_grid_view->getGridRows() ||
      m_unit_info_map->at(0).size() != m_gmm_grid_view->getGridCols())
    m_gmm_grid_view->addUnitIndex();
  else
    m_gmm_grid_view->addLabel(*m_unit_info_map, "Atom symbol");

  if (!m_is_showing_grid)
  {
    // Remove wait label
    m_ui->wdg_map_view_vlayout->removeWidget(m_ui->lbl_wait_map);
    m_ui->lbl_wait_map->setParent(NULL);

    // Set grid view
    m_gmm_grid_view->setParent(m_ui->wdg_map_view);
    m_ui->wdg_map_view_vlayout->addWidget(m_gmm_grid_view);
    m_gmm_grid_view->show();
    m_gmm_grid_view->setFocus();

    m_is_showing_grid = true;
  }

  // Set model name and info
  if (m_model_name.isEmpty())
    m_ui->lbl_model_name->setText("Model");
  else
    m_ui->lbl_model_name->setText("Model: " + m_model_name);

  m_ui->lbl_model_info->setText
      (
        "Map " + QString::number(m_gmm->getSOM().getNoRows()) +
        "x" + QString::number(m_gmm->getSOM().getNoColumns()) +
        " (" + QString::number(m_gmm->getSOM().getNoUnits()) + ") | " +
        "Bias " +
        QString::number(m_gmm->getReadout().getReadoutMatrix().at(0,0), 'g', 3)
      );

  return;
} // method showGMM

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method initConnections
 *
 * Initialize connections for this object.
 */
void GMMViewer::initConnections()
{
  // GMMViewer
  QObject::connect(
      m_ui->btn_close_map,
      SIGNAL(clicked()),
      this,
      SIGNAL(closeViewer()));

  // From GMMGridView
  QObject::connect(
      m_gmm_grid_view,
      SIGNAL(showUnitInfo(uint,uint)),
      this,
      SIGNAL(showUnitInfo(uint,uint)));

  return;
} // method initConnections

// =====================
// PRIVATE SLOTS METHODS
// =====================

/**
 * Slot on_btn_more_model_info_clicked
 *
 */
void GMMViewer::on_btn_more_model_info_clicked()
{
  ModelEquationDialog *model_eq_diag =
      new ModelEquationDialog(m_gmm->getModelEquation().c_str());

  model_eq_diag->setWindowTitle(m_model_name + " - Model equation");

  // model_eq_diag->setWindowFlags(Qt::Dialog);
  // model_eq_diag->setWindowFlags(Qt::Window);
  model_eq_diag->setAttribute(Qt::WA_DeleteOnClose);
  model_eq_diag->show();
  model_eq_diag->raise();
  model_eq_diag->activateWindow();

  return;
} // slot on_btn_more_model_info_clicked

/**
 * Slot on_btn_zoom_in_clicked
 *
 */
void GMMViewer::on_btn_zoom_in_clicked()
{
  m_gmm_grid_view->scaleView(1.2);
  return;
} // slot on_btn_zoom_in_clicked

/**
 * Slot on_btn_zoom_out_clicked
 *
 */
void GMMViewer::on_btn_zoom_out_clicked()
{
  m_gmm_grid_view->scaleView(1.0 / 1.2);
  return;
} // slot on_btn_zoom_out_clicked

} // namespace gui

