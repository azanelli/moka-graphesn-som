#include "loadunitinfo.h"

#include <stdexcept>
#include <QDebug>
#include <QFile>

namespace thread {

/**
 * Constructor
 *
 */
LoadUnitInfo::LoadUnitInfo(QObject *parent) :
  QThread(parent),
  m_unit_info_map(NULL),
  m_unit_info_mutex(NULL)
{ }

/**
 * Destructor
 *
 */
LoadUnitInfo::~LoadUnitInfo()
{
  // m_unit_info_map: is a bind (not to be deleted)
  // m_unit_info_mutex: is a bind (not to be deleted)
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
 *   - UNIT_INFO_MAP: pointer to a matrix of unit info that will be filled in
 *       the loading procedure.
 *   - UNIT_INFO_MAP_MUTEX: pointer to a mutex for an exclusive access to the
 *       info unit map. This mutex will be locked at the start of the loading
 *       and unlocked at the end. If the pointer is NULL the mutex is ignored.
 *   - FILENAME: unit info filename (path).
 */
void LoadUnitInfo::init
(
    UnitInfoMap *unit_info_map,
    QMutex *unit_info_mutex,
    const QString& filename
)
{
  m_unit_info_map = unit_info_map;
  m_unit_info_mutex = unit_info_mutex;
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
void LoadUnitInfo::run()
{
  // Try lock the GMM mutex
  if (!tryLockMutex())
  {
    emit mutexIsLocked();
    return;
  }

  // Start loading from file
  emit loadStart(m_filename);

  if (!m_unit_info_map)
  {
    unlockMutex();
    emit loadError("null info map");
    return;
  }

  try
  {
    loadUnitInfoFile(m_filename, *m_unit_info_map);
  }
  catch (std::exception& ex)
  {
    // Loading failed
    m_unit_info_map->clear();
    qDebug().nospace() << "LoadUnitInfo::run: " << ex.what() << ".";
    unlockMutex();
    emit loadError("invalid file " + m_filename);
    return;
  } // catch

  // Loading succesful
  unlockMutex();
  emit loadEnd(m_filename);

  return;
} // method run

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method loadUnitInfoFile
 *
 * Load a file written using the method GMM::saveUnitInfo, formatted as follow:
 *
 *   <version>
 *   <n. units>
 *   <n. rows>
 *   <n. cols>
 *   [info unit 1]
 *   ...
 *   [info unit N]
 *
 * Each information block ([info unit i]) is written as follows:
 *
 *   <row>
 *   <col>
 *   <weight>
 *   <n. labels>
 *   [label value 1]
 *   ...
 *   [label value L]
 *
 * The label value block ([label value j]) is written as follows:
 *
 *   <label name>
 *   <n. values>
 *   <value 1>
 *   <freq. value 1>
 *   ...
 *   <value M>
 *   <frew. value M>
 *
 * The object UNIT_INFO_MAP is resized and filled with UnitInfo object parsed
 * from FILENAME.
 */
void LoadUnitInfo::loadUnitInfoFile
(
    const QString& filename,
    UnitInfoMap& unit_info_map
) const
{
  QFile file(filename);

  // Open the input file
  if (!file.open(QIODevice::ReadOnly))
    throw std::runtime_error(file.errorString().toStdString());

  // Start reading the file
  QTextStream in(&file);

  // The version is ignored
  in.readLine();

  // Map dimensions
  uint n_units = readUInt(in);
  uint n_rows = readUInt(in);
  uint n_cols = readUInt(in);

  // Init the map
  unit_info_map.resize(n_rows);
  for (uint i = 0; i < n_rows; ++i)
    unit_info_map[i].resize(n_cols);

  // Go on to read each unit info
  for (uint unit = 0; unit < n_units; ++unit)
  {
    uint row = readUInt(in);
    uint col = readUInt(in);
    float weight = readFloat(in);
    uint n_labels = readUInt(in);

    // Check to be sure
    if (row >= n_rows || col >= n_cols)
      throw std::runtime_error("the unit row (or the column) is to high");

    unit_info_map[row][col].setRow(row);
    unit_info_map[row][col].setCol(col);
    unit_info_map[row][col].setWeight(weight);

    // Read each label
    for (uint l = 0; l < n_labels; ++l)
    {
      QString label_name = readLine(in);
      unit_info_map[row][col].pushBackLabel(label_name);

      util::UnitInfo::Label& label = unit_info_map[row][col].backLabel();

      uint n_values = readUInt(in);
      for (uint v = 0; v < n_values; ++v)
      {
        QString val = readLine(in);
        uint freq = readUInt(in);
        label.pushBack(val, freq);
      }

    } // for l

  } // for unit

  // Close the input file stream and return
  file.close();

  return;
} // method loadUnitInfoFile

/**
 * Method readLine
 *
 * Read and return a line readed using the QTextStream. If the stream has read
 * to the end of the file an exception will be thrown.
 */
QString LoadUnitInfo::readLine(QTextStream& in) const
{
  QString line = in.readLine(1024);
  if (line.isNull())
    throw std::runtime_error("Reached the end of file");
  return line;
} // method readLine

/**
 * Method readUInt
 *
 * Throw an exception if the stream has read to the end of the file or if the
 * conversion is failed.
 */
uint LoadUnitInfo::readUInt(QTextStream& in) const
{
  QString line = in.readLine(256);
  if (line.isNull())
    throw std::runtime_error("Reached the end of file");

  bool ok;
  uint value = line.toUInt(&ok);

  if (!ok)
    throw std::runtime_error(
        "Invalid integer value (" + line.toStdString() + ")");

  return value;
} // method readUInt

/**
 * Method readFloat
 *
 * Throw an exception if the stream has read to the end of the file or if the
 * conversion is failed.
 */
float LoadUnitInfo::readFloat(QTextStream& in) const
{
  QString line = in.readLine(256);
  if (line.isNull())
    throw std::runtime_error("Reached the end of file");

  bool ok;
  float value = line.toFloat(&ok);

  if (!ok)
    throw std::runtime_error(
        "Invalid float value (" + line.toStdString() + ")");

  return value;
} // method readFloat

/**
 * Method tryLockMutex
 *
 */
bool LoadUnitInfo::tryLockMutex()
{
  if (!m_unit_info_mutex)
    return true;
  return m_unit_info_mutex->tryLock();
} // method tryLockMutex

/**
 * Method unlockMutex
 *
 */
void LoadUnitInfo::unlockMutex()
{
  if (!m_unit_info_mutex)
    return;
  m_unit_info_mutex->unlock();
  return;
} // method unlockMutex

} // namespace thread
