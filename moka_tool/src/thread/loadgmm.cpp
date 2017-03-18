#include "loadgmm.h"

#include <stdexcept>
#include <QDebug>

namespace thread {

namespace mmo = ::moka::model;

/**
 * Constructor
 *
 */
LoadGMM::LoadGMM(QObject *parent) :
  QThread(parent),
  m_gmm(NULL),
  m_gmm_mutex(NULL)
{ }

/**
 * Destructor
 *
 */
LoadGMM::~LoadGMM()
{
  // m_gmm: is a bind (not to be deleted)
  // m_gmm_mutex: is a bind (not to be deleted)
  return;
}

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method init
 *
 * This method initializes the thread with passed objects. You must call this
 * method before to start the thread execution (call the method start).
 *
 * Method parameters are the following:
 *   - GMM: pointer to a GMM object that will be loaded.
 *   - GMM_MUTEX: pointer to a mutex for an exclusive access to the object gmm.
 *       This mutex will be locked at the start of the loading and unlocked at
 *       the end. If the pointer is NULL the mutex is ignored.
 *   - FILENAME: filename (path) from which load the gmm object.
 */
void LoadGMM::init(mmo::GraphEsnSom *gmm, QMutex *gmm_mutex, const QString& filename)
{
  m_gmm = gmm;
  m_gmm_mutex = gmm_mutex;
  m_filename = filename;
  return;
} // method init

// =================
// PROTECTED METHODS
// =================

/**
 * Method run
 *
 */
void LoadGMM::run()
{
  // Try lock the GMM mutex
  if (!tryLockMutex())
  {
    emit mutexIsLocked();
    return;
  }

  // Start loading from file
  emit loadStart(m_filename);

  if (!m_gmm)
  {
    unlockMutex();
    emit loadError("null GMM");
    return;
  }

  try
  {
    m_gmm->loadFromFile(m_filename.toStdString());
  }
  catch (std::exception& ex)
  {
    // Loading failed
    qDebug() << "LoadGMM::run:" << ex.what();
    unlockMutex();
    emit loadError("invalid file " + m_filename);
    return;
  } // catch

  // Some check on the loaded model
  if (m_gmm->getOutputSize() > 1)
  {
    qDebug() << "LoadGMM::run: the model has more than one output.";
    unlockMutex();
    emit loadError("invalid model in file " + m_filename);
    return;
  }

  // Loading succesful
  unlockMutex();
  emit loadEnd(m_filename);

  return;
} // method run

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method tryLockMutex
 *
 */
bool LoadGMM::tryLockMutex()
{
  if (!m_gmm_mutex)
    return true;
  return m_gmm_mutex->tryLock();
} // method tryLockMutex

/**
 * Method unlockMutex
 *
 */
void LoadGMM::unlockMutex()
{
  if (!m_gmm_mutex)
    return;
  m_gmm_mutex->unlock();
  return;
} // method unlockMutex

} // namespace thread
