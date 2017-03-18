#ifndef THREAD_LOADUNITINFO_H
#define THREAD_LOADUNITINFO_H

#include <string>
#include <vector>
#include <QMutex>
#include <QTextStream>
#include <QThread>

#include "util/unitinfo.h"

namespace thread {

/**
 * Class LoadUnitInfo
 *
 * Load a GMM unit info file.
 *
 * You must call the method init before to start the thread execution (invoking
 * the method start) to set thread parameters.
 *
 * The execution perform the below steps and emits a set of signals:
 *   - Try lock the mutex
 *       - If fail the execution is stopped and a signal is emitted:
 *           - emit mutexIsLocked(void)
 *       - Otherwise the mutex is locked and the execution go on
 *   - Start loading from file:
 *       - emit loadStart(QString)
 *   - End loading
 *       - If loading failed a signal is emitted and the unit info matrix is
 *         cleared:
 *           - emit loadError(QString)
 *       - If loading was succesful:
 *           - emit loadEnd(void)
 *
 */
class LoadUnitInfo : public QThread
{
    Q_OBJECT

  public:
    typedef std::vector< std::vector<util::UnitInfo> > UnitInfoMap;

    explicit LoadUnitInfo(QObject *parent = 0);
    virtual ~LoadUnitInfo();

    void init(
        UnitInfoMap *unit_info_map,
        QMutex *unit_info_mutex,
        const QString& filename);

  signals:
    void loadEnd(QString filename);
    void loadError(QString reason);
    void loadStart(QString filename);
    void mutexIsLocked();

  protected:
    void run();

  private:
    UnitInfoMap *m_unit_info_map;
    QMutex *m_unit_info_mutex;
    QString m_filename;

    void loadUnitInfoFile(
        const QString& filename,
        UnitInfoMap& unit_info_map) const;
    QString readLine(QTextStream& in) const;
    uint readUInt(QTextStream& in) const;
    float readFloat(QTextStream& in) const;
    bool tryLockMutex();
    void unlockMutex();

}; // class LoadUnitInfo

} // namespace thread

#endif // THREAD_LOADUNITINFO_H
