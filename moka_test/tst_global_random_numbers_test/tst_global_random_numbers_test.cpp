#include <moka/exception.h>
#include <moka/global.h>
#include <moka/log.h>

using namespace moka;

typedef Global::Uint Uint;
typedef Global::Real Real;

/**
 * Function main
 *
 * Prints a series of random numbers in order to test the Global::getRand
 * funcitons.
 */
int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    Log::out <<"Usage: " <<Log::endl;
    Log::out <<"  argv[0] : <program name>" <<Log::endl;
    Log::out <<"  argv[1] : random seed (0 for time(NULL))" <<Log::endl;
    Log::out <<"  argv[2] : number of random numbers" <<Log::endl;
    return 1;
  } // if (argc < 4)

  Log::out <<"Random seed init:   " <<Global::getRandSeed() <<Log::endl;
  Uint seed = Global::toUint(argv[1]);
  if (seed == 0)
    Global::setRandSeed();
  else
    Global::setRandSeed(seed);
  Log::out <<"Random seed setted: " <<Global::getRandSeed() <<Log::endl;
  Uint N = Global::toUint(argv[2]);

  Log::out <<"Random real in [0,5]: ";
  for (Uint i = 0; i < N; ++i)
    Log::out <<Global::getRandReal(0.0,5.0) <<" ";
  Log::out <<Log::endl;

  Log::out <<"Random int in [0,5]: ";
  for (Uint i = 0; i < N; ++i)
    Log::out <<Global::getRandInt(0,5) <<" ";
  Log::out <<Log::endl;

  Log::out <<"Random real in [-10,5]: ";
  for (Uint i = 0; i < N; ++i)
    Log::out <<Global::getRandReal(-10.0,5.0) <<" ";
  Log::out <<Log::endl;

  Log::out <<"Random int in [-10,5]: ";
  for (Uint i = 0; i < N; ++i)
    Log::out <<Global::getRandInt(-10,5) <<" ";
  Log::out <<Log::endl;

  Log::out <<"Random real in [0,1]: ";
  for (Uint i = 0; i < N; ++i)
    Log::out <<Global::getRandReal(0.0,1.0) <<" ";
  Log::out <<Log::endl;

  Log::out <<"Random int in [0,1]: ";
  for (Uint i = 0; i < N; ++i)
    Log::out <<Global::getRandInt(0,1) <<" ";
  Log::out <<Log::endl;

  try
  {
    Log::out <<"Random real in [10,-10]: ";
    for (Uint i = 0; i < N; ++i)
      Log::out <<Global::getRandReal(10.0,-10.0) <<" ";
    Log::out <<Log::endl;
  }  // try
  catch (moka::app_exception& e)
  {
    Log::out <<e.what() <<Log::endl;
  } // try-catch

  try
  {
    Log::out <<"Random int in [10,-10]: ";
    for (Uint i = 0; i < N; ++i)
      Log::out <<Global::getRandInt(10,int(-10)) <<" ";
    Log::out <<Log::endl;
  } // try
  catch (moka::app_exception& e)
  {
    Log::out <<e.what() <<Log::endl;
  } // try-catch

  try
  {
    Log::out <<"Random real in [-1,500000000000000000000]: ";
    for (Uint i = 0; i < N; ++i)
      Log::out <<Global::getRandReal(-1.0,500000000000000000000.0) <<" ";
    Log::out <<Log::endl;
  } // try
  catch (moka::app_exception& e)
  {
    Log::out <<e.what() <<Log::endl;
  } // try-catch

  try
  {
    Log::out <<"Random int in [-1,500000000000000000000]: ";
    for (Uint i = 0; i < N; ++i)
      Log::out <<Global::getRandInt(-1,int(500000000000000000000.0)) <<" ";
    Log::out <<Log::endl;
  } // try
  catch (moka::app_exception& e)
  {
    Log::out <<e.what() <<Log::endl;
  } // try-catch

  try
  {
    Log::out <<"Random real in [1,1]: ";
    for (Uint i = 0; i < N; ++i)
      Log::out <<Global::getRandReal(1,1) <<" ";
    Log::out <<Log::endl;
  } // try
  catch (moka::app_exception& e)
  {
    Log::out <<e.what() <<Log::endl;
  } // try-catch

  try
  {
    Log::out <<"Random int in [1,1]: ";
    for (Uint i = 0; i < N; ++i)
      Log::out <<Global::getRandInt(1,1) <<" ";
    Log::out <<Log::endl;
  } // try
  catch (moka::app_exception& e)
  {
    Log::out <<e.what() <<Log::endl;
  } // try-catch

  return 0;
} // function main

