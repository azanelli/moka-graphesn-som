#ifndef MOKA_LOG_H
#define MOKA_LOG_H

#include <iostream>
#include <sstream>

namespace moka {

/**
 * Class Log
 *
 * Utilities for logging. To be used as follows:
 *   Log::op << "Log message" << "Any object that can be printed" << Log::endl;
 * where op is one of the follows operators:
 *   out : for normal messages (normal operations).
 *   vrb : verbose mode: like out but printed only if verbose mode is active.
 *   err : for error messages.
 *   dbg : for debugging messages: printed only if _DEBUG is defined (i.e. with
 *         -D_DEBUG option in g++).
 * you can print any object on which is defined the operator <<.
 *
 * IMPORTANT: remember to always put a Log::end or a Log::endl at the end of
 * each message, this serves to send (to flush) the message that otherwise
 * remains suspended until a Log::end (Log::endl) don't appear. Note that you
 * can also do something like that:
 *   Log::out << "Output log message part 1 \n";
 *   Log::out << "Output log message part 2 \n";
 *   Log::out << Log::end;
 * or also:
 *   for (int i = 0; i < 10; ++i)
 *     Log::out << i <<" ";
 *   Log::out <<Log::endl;
 * with Log::end in a separate command, but always present.
 *
 * NOTE: std::endl can NOT be used, its analogue is Log::endl. To break lines
 * in a message use the character '\n'.
 */
class Log
{
  public:

    // -------
    // STRUCTS
    // -------

    //! Struct Stream
    struct Stream
    {
        std::ostringstream m_oss;
    }; // class Stream

    //! Struct OutputStream
    struct OutputStream : public Stream { };

    //! Struct VerboseStream
    struct VerboseStream : public Stream { };

    //! Struct ErrorStream
    struct ErrorStream : public Stream { };

    //! Struct DebugStream
    struct DebugStream : public Stream { };

    //! Struct FlushStream
    struct FlushStream { };

    //! Struct EndStream
    struct EndStream : public FlushStream { };

    //! Struct EndLine
    struct EndLine : public FlushStream { };

    // -------
    // OBJECTS
    // -------

    //! Object Log::out. Use this object to write normal messages.
    static OutputStream out;

    //! Object Log::vrb. Use this object to write normal messages in verbose
    //! mode (see Log::setVerbose).
    static VerboseStream vrb;

    //! Object Log::err. Use this object to write error messages.
    static ErrorStream err;

    //! Object Log::dbg. Use this object to write debug messages.
    static DebugStream dbg;

    //! Object Log::end. Use this object in the operator << to flush the log
    //! message.
    static EndStream end;

    //! Object Log::endl. Use this object in the operator << to flush the log
    //! message and add a carriage return at the end.
    static EndLine endl;

    // -------
    // METHODS
    // -------

    /**
     * Method isVerbose
     */
    static bool isVerbose()
    {
      return m_verbose;
    }

    /**
     * Method setVerbose
     *
     * Use Log::setVerbose() to print messages with Log::vrb. Use instead
     * Log::setVerbose(false) to stop print messages with Log::vrb.
     */
    static void setVerbose(bool verbose = true)
    {
      m_verbose = verbose;
    }

  private:
    static bool m_verbose;

}; // class Log

// =================
// RELATED FUNCTIONS
// =================

/**
 * operator << (Log::OutputStream)
 */
template <typename Type>
Log::OutputStream& operator<<(Log::OutputStream& los, const Type& obj)
{
  los.m_oss << obj;
  return los;
} // operator <<

/**
 * operator << (Log::VerboseStream)
 */
template <typename Type>
Log::VerboseStream& operator<<(Log::VerboseStream& lvs, const Type& obj)
{
  if (Log::isVerbose())
    lvs.m_oss << obj;
  return lvs;
} // operator <<

/**
 * operator << (Log::ErrorStream)
 */
template <typename Type>
Log::ErrorStream& operator<<(Log::ErrorStream& les, const Type& obj)
{
  les.m_oss << obj;
  return les;
} // operator <<

/**
 * operator << (Log::DebugStream)
 */
template <typename Type>
Log::DebugStream& operator<<(Log::DebugStream& lds, const Type& obj)
{
#ifdef _DEBUG
  lds.m_oss << obj;
#else
  (void)obj; // unused parameter warning suppress
#endif
  return lds;
} // operator <<

/**
 * operator << (Log::OutputStream - flush)
 *
 * Implementated in log.cpp.
 */
Log::OutputStream& operator<<(Log::OutputStream&, const Log::EndLine&);
Log::OutputStream& operator<<(Log::OutputStream& , const Log::EndStream&);

/**
 * operator << (Log::VerboseStream - flush)
 *
 * Implementated in log.cpp.
 */
Log::VerboseStream& operator<<(Log::VerboseStream&, const Log::EndLine&);
Log::VerboseStream& operator<<(Log::VerboseStream& , const Log::EndStream&);

/**
 * operator << (Log::ErrorStream - flush)
 *
 * Implementated in log.cpp.
 */
Log::ErrorStream& operator<<(Log::ErrorStream&, const Log::EndLine&);
Log::ErrorStream& operator<<(Log::ErrorStream& , const Log::EndStream&);

/**
 * operator << (Log::DebugStream - flush)
 *
 * Implementated in log.cpp.
 */
Log::DebugStream& operator<<(Log::DebugStream&, const Log::EndLine&);
Log::DebugStream& operator<<(Log::DebugStream& , const Log::EndStream&);

} // namespace moka

#endif // MOKA_LOG_H
