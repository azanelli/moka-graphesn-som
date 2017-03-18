#ifndef MOKA_UTIL_TIMER_H
#define MOKA_UTIL_TIMER_H

#include <sys/time.h>
#include <moka/global.h>
#include <moka/log.h>

namespace moka {
namespace util {

/**
 * Class Timer
 */
class Timer
{
  public:
    Timer() :
      m_wall_time(0.0),
      m_cpu_usage(0.0),
      m_is_running(false)
    { }

    /**
     * Method getCpuUsage
     *
     * Returns the seconds of cpu usage elapsed between the last invocation of
     * the method "start" and the last invocation of the method "stop".
     */
    Global::Real getCpuUsage()
    {
      return m_cpu_usage;
    } // method getCpuUsage

    /**
     * Method getWallTime
     *
     * Returns the number of seconds elapsed between the last invocation of the
     * method "start" and the last invocation of the method "stop".
     */
    Global::Real getWallTime()
    {
      return m_wall_time;
    } // method getWallTime

    /**
     * Method start
     *
     * Starts the timer. You can stop the timer through the "stop" method,
     * then with methods "getWallTime" and "getCpuUsage" you can take the
     * elapsed times.
     */
    void start()
    {
      gettimeofday(&m_wallt_start, NULL);
      m_cput_start = clock();
      m_is_running = true;
      return;
    } // method start

    /**
     * Method stop
     *
     * Stops the timer. Through methods "getWallTime" and "getCpuUsage" you can
     * take the elapsed times between the last call of the method "start" and
     * the call of this method.
     */
    void stop()
    {
      if (m_is_running)
      {
        gettimeofday(&m_wallt_end, NULL);
        m_cput_end = clock();
        m_wall_time =
            (m_wallt_end.tv_sec + (m_wallt_end.tv_usec / 1000000.0)) -
            (m_wallt_start.tv_sec + (m_wallt_start.tv_usec / 1000000.0));
        m_cpu_usage =
            (m_cput_end - m_cput_start) / Global::Real(CLOCKS_PER_SEC);
        m_is_running = false;
      } // if
      return;
    } // method stop

  private:
    Global::Real m_wall_time, m_cpu_usage;
    timeval m_wallt_start, m_wallt_end;
    clock_t m_cput_start, m_cput_end;
    bool m_is_running;

}; // class Timer

} // namespace util
} // namespace moka

#endif // MOKA_UTIL_TIMER_H
