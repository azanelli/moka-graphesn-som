#ifndef THREAD_LOADGMM_H
#define THREAD_LOADGMM_H

#include <QMutex>
#include <QThread>
#include <QString>
#include <moka/model/graphesnsom.h>

namespace thread {

/**
 * Class LoadGMM
 *
 * Load a GMM object.
 *
 * You must call the method init before to start the thread execution (invoking
 * the method start) to set thread parameters.
 *
 * The execution perform the below steps and emits a set of signals:
 *   - Try lock the GMM mutex
 *       - If fail the execution is stopped and a signal is emitted:
 *           - emit mutexIsLocked(void)
 *       - Otherwise the mutex is locked and the execution go on
 *   - Start loading from file:
 *       - emit loadStart(QString)
 *   - End loading
 *       - If loading failed a signal is emitted and the GMM is cleared
 *           - emit loadError(QString)
 *       - If loading was succesful:
 *           - emit loadEnd(void)
 *
 */
class LoadGMM : public QThread
{
    Q_OBJECT

  public:
    explicit LoadGMM(QObject *parent = 0);
    virtual ~LoadGMM();

    void init(
        moka::model::GraphEsnSom *gmm,
        QMutex *gmm_mutex,
        const QString& filename);

  signals:
    void loadEnd(QString filename);
    void loadError(QString reason);
    void loadStart(QString filename);
    void mutexIsLocked();

  protected:
    void run();

  private:
    ::moka::model::GraphEsnSom *m_gmm;
    QMutex *m_gmm_mutex;
    QString m_filename;

    bool tryLockMutex();
    void unlockMutex();

}; // class LoadGMM

} // namespace thread

#endif // THREAD_LOADGMM_H
