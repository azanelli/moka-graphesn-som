#ifndef MOKA_EXCEPTION_H
#define MOKA_EXCEPTION_H

#include <stdexcept>
#include <string>

/*
  Standard exceptions (from STL):

  std::exception
    + std::bad_alloc
    + std::bad_cast
    + std::bad_exception
    + std::bad_typeid
    + std::ios_base::failure
    + std::logic_error
    |   + std::domain_error
    |   + std::invalid_argument
    |   + std::length_error
    |   \ std::out_of_range
    \ std::runtime_error
        + std::overflow_error
        + std::range_error
        \ std::underflow_error
*/

namespace moka {

/**
 * Class GenericException
 *
 * The most generic exception from moka library (subtype of std::exception).
 */
class GenericException : public std::exception
{
  public:
    explicit GenericException(const std::string& what_arg) throw() :
      m_what(what_arg)
    { }

    virtual ~GenericException() throw()
    { }

    virtual const char* what() const throw()
    {
      return m_what.c_str();
    }

  private:
    std::string m_what;

}; // class GenericException

} // namespace moka

#endif // MOKA_EXCEPTION_H
