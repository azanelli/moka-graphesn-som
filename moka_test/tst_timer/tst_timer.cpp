#include <sys/time.h>
#include <moka/util/timer.h>
#include "common.h"

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  moka::util::Timer timer;

  std::cout <<"Start... " <<std::endl;
  timer.start();
  startTimer();
  for (long i = 0; i < 100000000; ++i)
    std::sqrt(123.456L); // burn some time
  for (long i = 0; i < 1000000; ++i)
    std::cerr <<"."; // waste some time (hint: put std err on file)
  std::cout <<std::endl;
  timer.stop();
  endTimer();

  std::cout <<"Moka timer: wall time: " <<timer.getWallTime() <<std::endl;
  std::cout <<"Moka timer: cpu usage: " <<timer.getCpuUsage() <<std::endl;
  std::cout <<std::endl;

  std::cout <<"Try wrong stop..." <<std::endl;
  for (long i = 0; i < 10000000; ++i)
    std::sqrt(123.456L); // burn some time
  timer.stop();

  std::cout <<"Moka timer - Wall time: " <<timer.getWallTime() <<std::endl;
  std::cout <<"Moka timer - Cpu usage: " <<timer.getCpuUsage() <<std::endl;

  return 0;
} // function main

