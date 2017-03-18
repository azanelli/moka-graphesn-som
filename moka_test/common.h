#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <sstream>
#include <string>
#include <vector>

// ======================
// FILE-LOCAL DEFINITIONS
// ======================

namespace 
{

	// Time structures
	timeval time_start, time_end;
	clock_t tcpu_start, tcpu_end;

} // namespace "unnamed"

// =========
// FUNCTIONS
// =========

/**
 * Function s2v
 *
 * Converts a string of elements into a std::vector.
 */
template <typename T>
std::vector<T> s2v(const std::string& str) 
{
  std::stringstream ss;
  ss <<str;
  std::vector<T> vec;
  T element = T();
  while (ss.good()) {
    ss >>element;
    vec.push_back(element);
  }
  return vec;
} // function s2v

/**
 * Function startTimer
 *
 * Starts a timer that can be stopped with the function "endTimer".
 */
void startTimer() 
{
  gettimeofday(&time_start, NULL);
  tcpu_start = clock();
  return;
} // funciton start

/**
 * Function endTimer
 *
 * Stops the timer started with the function "startTimer" and prints on
 * standard output the elapsed time in the following format:
 *   Elapsed time: <t1> (seconds) \n
 *   CPU usage: <t2> (seconds) \n
 */
void endTimer() 
{
  gettimeofday(&time_end, NULL);
  tcpu_end = clock();
  std::cout 
      << "Elapsed time: "
      << (time_end.tv_sec + (time_end.tv_usec / 1000000.0)) -
         (time_start.tv_sec + (time_start.tv_usec / 1000000.0))
      << " (seconds)" << std::endl;
  std::cout 
      << "CPU usage: " << (tcpu_end-tcpu_start) / double(CLOCKS_PER_SEC)
      << " (seconds)" << std::endl;
  return;
} // funciton start

#endif // TEST_COMMON_H

