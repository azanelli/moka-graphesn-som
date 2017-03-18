#include "log.h"

namespace moka {

// ==============
// STATIC MEMBERS
// ==============

Log::OutputStream  Log::out;
Log::VerboseStream Log::vrb;
Log::ErrorStream   Log::err;
Log::DebugStream   Log::dbg;
Log::EndStream     Log::end;
Log::EndLine       Log::endl;

// Init values
bool Log::m_verbose = false;

// =================
// RELATED FUNCTIONS
// =================

/**
 * OutputStream operator<< for the end of stream
 */
Log::OutputStream& operator<<(Log::OutputStream& los, const Log::EndStream& e)
{
  (void)e; // suppress unused parameter warning
  // *** Handle here the output log message into the stream object los ***
  std::cout << los.m_oss.str();
  std::cout.flush();
  los.m_oss.str("");
  los.m_oss.clear();
  return los;
} // operator<<

/**
 * OutputStream operator<< for the end of line
 */
Log::OutputStream& operator<<(Log::OutputStream& los, const Log::EndLine& e)
{
  (void)e; // suppress unused parameter warning
  los.m_oss << "\n";
  return los << Log::end;
} // operator<<

/**
 * VerboseStream operator<< for the end of stream
 */
Log::VerboseStream& operator<<(Log::VerboseStream& lvs, const Log::EndStream& e)
{
  (void)e; // suppress unused parameter warning
  // *** Handle here the output log message into the stream object lvs ***
  if (Log::isVerbose())
  {
    std::cout << "[LOG] " << lvs.m_oss.str();
    std::cout.flush();
    lvs.m_oss.str("");
    lvs.m_oss.clear();
  }
  return lvs;
} // operator<<

/**
 * VerboseStream operator<< for the end of line
 */
Log::VerboseStream& operator<<(Log::VerboseStream& lvs, const Log::EndLine& e)
{
  (void)e; // suppress unused parameter warning
  if (Log::isVerbose())
  {
    lvs.m_oss << "\n";
    return lvs << Log::end;
  }
  return lvs;
} // operator<<

/**
 * ErrorStream operator<< for the end of stream
 */
Log::ErrorStream& operator<<(Log::ErrorStream& les, const Log::EndStream& e)
{
  (void)e; // suppress unused parameter warning
  // *** Handle here the output log message into the stream object les ***
  std::cerr << les.m_oss.str();
  std::cerr.flush();
  les.m_oss.str("");
  les.m_oss.clear();
  return les;
} // operator<<

/**
 * ErrorStream operator<< for the end of line
 */
Log::ErrorStream& operator<<(Log::ErrorStream& les, const Log::EndLine& e)
{
  (void)e; // suppress unused parameter warning
  les.m_oss << "\n";
  return les << Log::end;
} // operator<<

/**
 * DebugStream operator<< for the end of stream
 */
Log::DebugStream& operator<<(Log::DebugStream& lds, const Log::EndStream& e)
{
  (void)e; // suppress unused parameter warning
#ifdef _DEBUG
  // *** Handle here the debug log message into the stream object lds ***
  std::cout << "[DEBUG] " << lds.m_oss.str();
  std::cout.flush();
  lds.m_oss.str("");
  lds.m_oss.clear();
#endif
  return lds;
} // operator<<

/**
 * DebugStream operator<< for the end of line
 */
Log::DebugStream& operator<<(Log::DebugStream& lds, const Log::EndLine& e)
{
  (void)e; // suppress unused parameter warning
  lds.m_oss << "\n";
  return lds << Log::end;
} // operator<<

} // namespace moka
