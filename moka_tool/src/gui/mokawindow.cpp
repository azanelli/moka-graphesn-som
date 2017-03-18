#include "mokawindow.h"
#include "ui_mokawindow.h"

#include <iostream>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QtGui/QApplication>
#include <QSettings>

#include "gui/meleculereseultsdialog.h"
#include "gui/unitinfowindow.h"

namespace gui {

/**
 * Constructor
 *
 */
MokaWindow::MokaWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MokaWindow),
  m_gmm_is_loaded(false),
  m_unit_info_is_loaded(false),
  m_dataset_cofig_is_loaded(false)
{
  // Init gui elements
  m_ui->setupUi(this);
  m_gmm_viewer = new GMMViewer(NULL);

  // Window title
  setWindowTitle("");

  // Install event filters
  m_ui->lbl_load_model->installEventFilter(this);
  m_ui->lbl_load_unit_info->installEventFilter(this);
  m_ui->lbl_load_dataset_config->installEventFilter(this);

  // Load saved settings
  loadSettings();

  // Init connections
  initConnections();

  // Init gui elements (after load settings)
  if (m_ui->dck_molecules->isHidden())
    m_ui->wdg_open_molecules_dock->setVisible(true);
  else
    m_ui->wdg_open_molecules_dock->setVisible(false);

  // Focus on load widget
  m_ui->wdg_load->setFocus();

  // Hide load dataset info button (tmp code)
  m_ui->lbl_load_dataset_config->setVisible(false);

  return;
} // constructor

/**
 * Destructor
 *
 */
MokaWindow::~MokaWindow()
{
  delete m_ui;
  delete m_gmm_viewer;
  return;
} // destructor

// =================
// PROTECTED METHODS
// =================

/**
 * Method eventFilter
 *
 */
bool MokaWindow::eventFilter(QObject *obj, QEvent *ev)
{
  if (obj == m_ui->lbl_load_model)
    handleEvent_lbl_load_model(ev);

  if (obj == m_ui->lbl_load_unit_info)
    handleEvent_lbl_load_unit_info(ev);

  if (obj == m_ui->lbl_load_dataset_config)
    handleEvent_lbl_load_dataset_config(ev);

  return QMainWindow::eventFilter(obj, ev);
} // method eventFilter

/**
 * Method closeEvent
 *
 */
void MokaWindow::closeEvent(QCloseEvent *)
{
  saveSettings();
  QApplication::quit();
  return;
} // method closeEvent

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method enableViewModel
 *
 * Enable (or disable) the view model action (menu, button, etc.) looking if
 * other dangerous thread are running. If ENABLE is false the thread pointer
 * is ignored and the view model action is always disabled. If FROM_THREAD is
 * null all the threads are checked.
 */
void MokaWindow::enableViewModel(bool enable, QThread const *from_thread)
{
  // Disable request
  if (!enable)
  {
    m_ui->act_view_model->setEnabled(false);
    m_ui->btn_view_model->setEnabled(false);
    m_ui->lbl_load_model->setFocus();
    return;
  }

  // Enable request
  if (from_thread == NULL)
    if (m_th_load_gmm.isRunning() || m_th_load_unit_info.isRunning())
      return;

  if (from_thread == (&m_th_load_gmm) && m_th_load_unit_info.isRunning())
    return;

  if (from_thread == (&m_th_load_unit_info) && m_th_load_gmm.isRunning())
    return;

  if (m_gmm_is_loaded)
  {
    m_ui->act_view_model->setEnabled(true);
    m_ui->btn_view_model->setEnabled(true);
    m_ui->btn_view_model->setFocus();
  }

  return;
} // method enableViewModel

/**
 * Method handleEvent_lbl_load_dataset_config
 *
 */
void MokaWindow::handleEvent_lbl_load_dataset_config(QEvent *ev)
{
  if (ev->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *mev = static_cast<QMouseEvent*>(ev);

    if (mev->button() == Qt::LeftButton && m_ui->lbl_load_model->isEnabled())
    {
      QString filename = QFileDialog::getOpenFileName(
          this, "Select a dataset configuration file", QDir::homePath());

      if (!filename.isEmpty())
      {
        // TODO: change behaviour here
        m_dataset_cofig_is_loaded = true;
        m_ui->lbl_load_dataset_config->setStyleSheet(
            "color: #AFD8B0; "
            "border: 3px solid #AFD8B0; ");
        m_ui->lbl_load_dataset_config->setText(
            "Dataset Configuration File Loaded: some_file.ini");
        m_ui->btn_molecules_open->setEnabled(true);
      } // if (!filename.isEmpty())

    } // if Qt::LeftButton
  } // if MouseButtonPress

  return;
} // method handleEvent_lbl_load_dataset_config

/**
 * Method handleEvent_lbl_load_model
 *
 */
void MokaWindow::handleEvent_lbl_load_model(QEvent *ev)
{
  if (ev->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *mev = static_cast<QMouseEvent*>(ev);

    if (mev->button() == Qt::LeftButton && m_ui->lbl_load_model->isEnabled())
    {
      if (m_th_load_unit_info.isRunning())
        statusBar()->showMessage("Wait loading unit info", 5000);
      else
      {
        // Show a file dialog window
        QString model_filename = QFileDialog::getOpenFileName(
            this, "Select a model file", QDir::homePath());

        // Start the loading thread
        if (!model_filename.isEmpty())
        {
          m_th_load_gmm.init(&m_gmm, &m_gmm_mutex, model_filename);
          m_th_load_gmm.start();
        }
      } // else (!m_th_load_unit_info.isRunning())

    } // if Qt::LeftButton
  } // if MouseButtonPress

  return;
} // method handleEvent_lbl_load_model

/**
 * Method handleEvent_lbl_load_unit_info
 *
 */
void MokaWindow::handleEvent_lbl_load_unit_info(QEvent *ev)
{
  if (ev->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *mev = static_cast<QMouseEvent*>(ev);

    if (mev->button() == Qt::LeftButton &&
        m_ui->lbl_load_unit_info->isEnabled())
    {
      if (!m_gmm_is_loaded)
        statusBar()->showMessage("You should first load a model", 5000);
      else
      {
        QString start_dir = m_gmm_filename.isEmpty() ?
              QDir::homePath() : QFileInfo(m_gmm_filename).absolutePath();

        // Show a file dialog window
        QString unit_info_filename = QFileDialog::getOpenFileName(
              this, "Select a unit info file", start_dir);

        // Start the loading thread
        if (!unit_info_filename.isEmpty())
        {
          m_th_load_unit_info.init(
                &m_unit_info_map, &m_unit_info_map_mutex, unit_info_filename);
          m_th_load_unit_info.start();
        }

      } // else (!m_gmm_is_loaded)

    } // if Qt::LeftButton
  } // if MouseButtonPress

  return;
} // method handleEvent_lbl_load_model

/**
 * Method initConnections
 *
 * Initializes the connections for this object.
 */
void MokaWindow::initConnections()
{
  // MokaWindow
  QObject::connect(
      m_gmm_viewer,
      SIGNAL(closeViewer()),
      this,
      SLOT(closeGMMViewer()));

  // From GMMViewer
  QObject::connect(
      m_gmm_viewer,
      SIGNAL(showUnitInfo(uint, uint)),
      this,
      SLOT(showUnitInfo(uint,uint)));

  // From thread m_th_load_gmm
  QObject::connect(
      &m_th_load_gmm,
      SIGNAL(loadEnd(QString)),
      this,
      SLOT(loadModelEnd(QString)));
  QObject::connect(
      &m_th_load_gmm,
      SIGNAL(loadError(QString)),
      this,
      SLOT(loadModelError(QString)));
  QObject::connect(
      &m_th_load_gmm,
      SIGNAL(loadStart(QString)),
      this,
      SLOT(loadModelStart(QString)));

  // From thread m_th_load_unit_info
  QObject::connect(
      &m_th_load_unit_info,
      SIGNAL(loadEnd(QString)),
      this,
      SLOT(loadUnitInfoEnd(QString)));
  QObject::connect(
      &m_th_load_unit_info,
      SIGNAL(loadError(QString)),
      this,
      SLOT(loadUnitInfoError(QString)));
  QObject::connect(
      &m_th_load_unit_info,
      SIGNAL(loadStart(QString)),
      this,
      SLOT(loadUnitInfoStart(QString)));

  return;
} // method initConnections

/**
 * Method loadSettings
 *
 */
void MokaWindow::loadSettings()
{
  QSettings settings;
  settings.beginGroup("gui/mainwindow");
  this->restoreState(settings.value("window_state").toByteArray());
  this->resize
      (
        settings.value("size", QSize(800, QApplication::desktop()->height()))
        .toSize()
      );
  this->move(settings.value("pos", QPoint(0, 0)).toPoint());
  settings.endGroup();
  return;
} // method loadSettings

/**
 * Method removeUnitInfo
 *
 */
void MokaWindow::removeUnitInfo()
{
  if (!m_unit_info_map_mutex.tryLock())
    return;

  // Class members
  m_unit_info_map.clear();
  m_unit_info_is_loaded = false;
  m_unit_info_filename.clear();

  // GMMViewer
  m_gmm_viewer->removeUnitInfoMap();

  // User interface
  m_ui->lbl_load_unit_info->setEnabled(true);
  m_ui->lbl_load_unit_info->setStyleSheet("");
  m_ui->lbl_load_unit_info->setText("Load Unit Info");

  m_unit_info_map_mutex.unlock();
  return;
} // method removeUnitInfo

/**
 * Method saveSettings
 *
 */
void MokaWindow::saveSettings() const
{
  QSettings settings;
  settings.beginGroup("gui/mainwindow");
  settings.setValue(
      "window_state",
      this->saveState());
  settings.setValue("size", this->size());
  settings.setValue("pos", this->pos());
  settings.endGroup();
  return;
} // method saveSettings

/**
 * Method setWindowTitle
 *
 */
void MokaWindow::setWindowTitle(const QString& prefix)
{
  if (prefix.isEmpty())
    QMainWindow::setWindowTitle(
         QCoreApplication::applicationName());
  else
    QMainWindow::setWindowTitle(
        prefix + " - " + QCoreApplication::applicationName());
  return;
} // method setWindowTitle

// =============
// PRIVATE SLOTS
// =============

/**
 * Slot closeGMMViewer
 *
 */
void MokaWindow::closeGMMViewer()
{
  // Change some elements
  m_ui->act_view_model->setEnabled(true);

  // Remove the gmm viewer
  m_ui->wdg_main_widget_hlayout->removeWidget(m_gmm_viewer);
  m_gmm_viewer->setParent(NULL);

  // Set the load widget
  m_ui->wdg_load->setParent(m_ui->wdg_main_widget);
  m_ui->wdg_main_widget_hlayout->addWidget(m_ui->wdg_load);
  m_ui->wdg_load->show();
  m_ui->wdg_load->setFocus();

  return;
} // method closeGMMViewer

/**
 * Slot loadModelEnd
 *
 */
void MokaWindow::loadModelEnd(QString filepath)
{
  QString filename = QFileInfo(filepath).fileName();

  statusBar()->showMessage("Model succesfully loaded from " + filepath, 5000);
  m_gmm_is_loaded = true;
  m_gmm_filename = filepath;
  m_ui->lbl_load_model->setEnabled(true);
  m_ui->lbl_load_model->setStyleSheet(
      "color: #AFD8B0; "
      "border: 3px solid #AFD8B0; ");
  m_ui->lbl_load_model->setText("Model Loaded: " + filename);
  setWindowTitle(filename);

  // Load the GMM in the viewer
  m_gmm_viewer->bindGMM(&m_gmm);
  m_gmm_viewer->setModelName(filename);
  m_gmm_viewer->showGMM();

  enableViewModel(true, &m_th_load_gmm);

  return;
} // slot loadModelEnd

/**
 * Slot loadModelError
 *
 */
void MokaWindow::loadModelError(QString reason)
{
  statusBar()->showMessage("Loading model fail: " + reason, 10000);
  m_ui->lbl_load_model->setEnabled(true);
  m_ui->lbl_load_model->setText("Load Model");
  setWindowTitle("");
  enableViewModel(false);
  return;
} // slot loadModelError

/**
 * Slot loadModelStart
 *
 */
void MokaWindow::loadModelStart(QString filepath)
{
  enableViewModel(false);
  statusBar()->showMessage("Loading model from " + filepath, 5000);
  removeUnitInfo();
  m_gmm_filename.clear();
  m_gmm_is_loaded = false;
  m_ui->lbl_load_model->setEnabled(false);
  m_ui->lbl_load_model->setStyleSheet("");

  QString filename = QFileInfo(filepath).fileName();
  m_ui->lbl_load_model->setText("Loading model from " + filename);

  return;
} // slot loadModelStart

/**
 * Slot loadUnitInfoEnd
 *
 */
void MokaWindow::loadUnitInfoEnd(QString filepath)
{
  QString filename = QFileInfo(filepath).fileName();

  statusBar()->showMessage(
        "Unit info succesfully loaded from " + filepath, 5000);

  m_unit_info_is_loaded = true;
  m_unit_info_filename = filepath;
  m_ui->lbl_load_unit_info->setEnabled(true);
  m_ui->lbl_load_unit_info->setStyleSheet(
      "color: #AFD8B0; "
      "border: 3px solid #AFD8B0; ");
  m_ui->lbl_load_unit_info->setText("Unit Info Loaded: " + filename);

  // Bind the new unit info map in the GMMViewer and show the map with infos
  m_gmm_viewer->bindUnitInfoMap(&m_unit_info_map);
  // TODO: add in the GMMViewer removeLabel/addLabel methods to avoid to
  // re-show the entire map.
  m_gmm_viewer->showGMM();

  enableViewModel(true, &m_th_load_unit_info);

  return;
} // slot loadUnitInfoEnd

/**
 * Slot loadUnitInfoError
 *
 */
void MokaWindow::loadUnitInfoError(QString reason)
{
  statusBar()->showMessage("Loading unit info fail: " + reason, 10000);

  m_ui->lbl_load_unit_info->setEnabled(true);
  m_ui->lbl_load_unit_info->setStyleSheet("");
  m_ui->lbl_load_unit_info->setText("Load Unit Info");

  if (m_gmm_is_loaded)
    enableViewModel(true, &m_th_load_unit_info);

  return;
} // slot loadUnitInfoError

/**
 * Slot loadUnitInfoStart
 *
 */
void MokaWindow::loadUnitInfoStart(QString filepath)
{
  enableViewModel(false);
  statusBar()->showMessage("Loading unit info from " + filepath, 5000);

  // Remove the old unit info map binded from the GMMViewer
  m_gmm_viewer->removeUnitInfoMap();

  m_unit_info_filename.clear();
  m_unit_info_is_loaded = false;
  m_ui->lbl_load_unit_info->setEnabled(false);
  m_ui->lbl_load_unit_info->setStyleSheet("");

  QString filename = QFileInfo(filepath).fileName();
  m_ui->lbl_load_unit_info->setText("Loading unit info from " + filename);

  return;
} // slot loadUnitInfoStart

/**
 * Slot on_act_view_model_triggered
 *
 */
void MokaWindow::on_act_view_model_triggered()
{
  showGMMViewer();
  return;
} // slot on_act_view_model_triggered

/**
 * Slot on_btn_open_molecules_dock_clicked
 *
 */
void MokaWindow::on_btn_open_molecules_dock_clicked()
{
  if (m_ui->dck_molecules->isHidden())
    m_ui->dck_molecules->setVisible(true);
  m_ui->wdg_open_molecules_dock->setVisible(false);
  return;
} // slot on_btn_open_molecules_dock_clicked

/**
 * Slot on_btn_open_molecules_dock_clicked
 *
 */
void MokaWindow::on_btn_molecules_calculate_clicked()
{
  // TODO: change behaviour of this method

  if (!m_gmm_is_loaded)
    return;

  QList<QListWidgetItem*> selected = m_ui->lst_molecules_list->selectedItems();
  if (selected.isEmpty())
    return;

  MeleculeReseultsDialog *window =
      new MeleculeReseultsDialog(selected.at(0)->text(), this);

  window->setWindowTitle(selected.at(0)->text() + " - " + windowTitle());
  window->setAttribute(Qt::WA_DeleteOnClose);
  window->show();
  window->raise();
  window->activateWindow();

  return;
} // slot on_btn_molecules_calculate_clicked

/**
 * Slot on_btn_molecules_open_clicked
 *
 */
void MokaWindow::on_btn_molecules_open_clicked()
{
  // TODO: change behaviour of this method

  QString filename = QFileDialog::getOpenFileName(
      this, "Select an SDF/MOL file", QDir::homePath());

  if (!filename.isEmpty())
  {
    m_ui->btn_molecules_calculate->setEnabled(true);

    for (uint i = 0; i < 12; ++i)
    {
      m_ui->lst_molecules_list->addItem(
            "000" + QString::number(::moka::Global::getRandInt(10, 99)));
    }

  } // if (!filename.isEmpty())

  return;
} // slot on_btn_molecules_open_clicked

/**
 * Slot on_btn_open_molecules_dock_clicked
 *
 */
void MokaWindow::on_dck_molecules_visibilityChanged(bool visible)
{
  if (!visible)
    m_ui->wdg_open_molecules_dock->setVisible(true);
  return;
} // slot on_dck_molecules_visibilityChanged

/**
 * Slot showGMMViewer
 *
 */
void MokaWindow::showGMMViewer()
{
  // Change some elements
  m_ui->act_view_model->setEnabled(false);

  // Remove the load widget
  m_ui->wdg_main_widget_hlayout->removeWidget(m_ui->wdg_load);
  m_ui->wdg_load->setParent(NULL);

  // Set the gmm viewer
  m_gmm_viewer->setParent(m_ui->wdg_main_widget);
  m_ui->wdg_main_widget_hlayout->addWidget(m_gmm_viewer);
  m_gmm_viewer->show();
  m_gmm_viewer->setFocus();
  m_gmm_viewer->setFocusOnGrid();

  return;
} // method showGMMViewer

/**
 * Slot showUnitInfo
 *
 */
void MokaWindow::showUnitInfo(uint row, uint col)
{
  if (!m_unit_info_is_loaded)
    return;

  if (row >= m_unit_info_map.size() || col >= m_unit_info_map[0].size())
  {
    qDebug().nospace() << "MokaWindow::showUnitInfo: request info about unit ("
        << row << "," << col << ") but the info map is not so big.";
    return;
  }

  UnitInfoWindow *unit_info_window =
      new UnitInfoWindow(m_unit_info_map[row][col], this);

  unit_info_window->setWindowTitle
      (
        "unit (" + QString::number(row) + ", " + QString::number(col) + ")" +
        " - " + windowTitle()
      );
  // unit_info_window->setWindowFlags(Qt::Dialog);
  unit_info_window->setWindowFlags(Qt::Window);
  unit_info_window->setAttribute(Qt::WA_DeleteOnClose);
  unit_info_window->show();
  unit_info_window->raise();
  unit_info_window->activateWindow();

  return;
} // method showUnitInfo

} // namespace gui
