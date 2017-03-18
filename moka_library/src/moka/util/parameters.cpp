#include "parameters.h"

#include <stdexcept>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <moka/log.h>

namespace moka {
namespace util {

namespace bpt = boost::property_tree;

typedef Global::Real Real;
typedef Global::Uint Uint;

/**
 * Constructor Parameters
 */
Parameters::Parameters()
{
  clearObject();
}

/**
 * Copy constructor Parameters
 */
Parameters::Parameters(const Parameters& params)
{
  this->operator=(params);
} // copy constructor

/**
 * Destructor ~Parameters
 */
Parameters::~Parameters()
{ }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method at
 *
 * As operator[], with this method you can do something like that:
 *  Parameters *paramspoint = new Parameters();
 *  paramspoint->at("key1") = "value1";
 *  paramspoint->at("key2") = "value2";
 */
std::string& Parameters::at(const std::string& key)
{
  return m_params_map[key];
} // method at

/**
 * Method check
 *
 * Returns true if the value associated with the passed key respects all the
 * setted flags (see the comment of the class for the list of flags), returns
 * false otherwise.
 * If the verbose mode is setted will be printed a message on Log::err whenever
 * the key does not respect something, i.e. when the method returns false.
 */
bool Parameters::check
(
    const std::string& key,
    int flags,
    const std::string& x1,
    const std::string& x2
) const
{
  std::map<std::string, std::string>::const_iterator it;

  // check required / optional
  if ((it = m_params_map.find(key)) == m_params_map.end())
  {
    if (flags & optional)
    {
      // printMsg(key + " not exists, but is optional.");
      return true;
    }
    else
    {
      printMsg(std::string("<") + key + "> not exists and is required");
      return false;
    } // if optional

  } // if exists

  if (flags & non_empty)
    if (it->second.empty())
    {
      printMsg(std::string("<") + key + "> should be non-empty.");
      return false;
    } // if non_empty

  // check uint
  if (flags & uint)
  {
    Uint value = Global::toUint(it->second);

    // check uint: positive
    if ((flags & positive) && (value == 0))
    {
      printMsg(
          std::string("<") + key + "> with value " + Global::toString(value) +
          " should be a positive unsigned integer");
      return false;
    } // if positive

    // check uint: non_zero
    if ((flags & non_zero) && (value == 0))
    {
      printMsg(
          std::string("<") + key + "> has value 0 but should be a non-zero "
          "unsigned integer");
      return false;
    } // if non_zero

    // check uint: non_negative ==> always true
    // check uint: less_than
    if (flags & less_than)
    {
      Uint v_x1 = Global::toUint(x1);
      if (value >= v_x1)
      {
        printMsg(
            std::string("<") + key + "> with value " +
            Global::toString(value) + " should be less then " +
            Global::toString(v_x1));
        return false;
      } // if
    } // if less_than

    // check uint: in_range
    if (flags & in_range)
    {
      Uint v_x1 = Global::toUint(x1);
      Uint v_x2 = Global::toUint(x2);
      if ((value < v_x1) || (value > v_x2))
      {
        printMsg(
            std::string("<") + key + "> with value " +
            Global::toString(value) + " should be between " +
            Global::toString(v_x1) + " and " + Global::toString(v_x2));
        return false;
      } // if
    } // if in_range

  } // if uint

  // check real
  if (flags & real)
  {
    Real value = Global::toReal(it->second);

    // check real: positive
    if ((flags & positive) && (value <= 0))
    {
      printMsg(
          std::string("<") + key + "> with value " + Global::toString(value) +
          " should be a positive real number");
      return false;
    } // if

    // check real: non_zero
    if ((flags & non_zero) && (value == 0))
    {
      printMsg(
          std::string("<") + key + "> has value 0 but should be a non-zero "
          "real number");
      return false;
    } // if

    // check real: non_negative
    if ((flags & non_negative) && (value < 0))
    {
      printMsg(
          std::string("<") + key + "> with value " + Global::toString(value) +
          " should be a non-negative real number");
      return false;
    } // if

    // check uint: less_than
    if (flags & less_than)
    {
      Real v_x1 = Global::toReal(x1);
      if (value >= v_x1)
      {
        printMsg(
            std::string("<") + key + "> with value " +
            Global::toString(value) + " should be less then " +
            Global::toString(v_x1));
        return false;
      } // if
    } // if less_than

    // check uint: in_range
    if (flags & in_range)
    {
      Real v_x1 = Global::toReal(x1);
      Real v_x2 = Global::toReal(x2);
      if ((value < v_x1) || (value > v_x2))
      {
        printMsg(
            std::string("<") + key + "> with value " +
            Global::toString(value) + " should be between " +
            Global::toString(v_x1) + " and " + Global::toString(v_x2));
        return false;
      } // if
    } // if in_range

  } // if real

  return true;
} // method check

/**
 * Method clear
 *
 * Clears this object as just created.
 */
void Parameters::clear()
{
  clearObject();
  return;
} // method clear

/**
 * Method contains
 *
 * Returns true if the <key> exists in the object, false otherwise.
 * As check(key, Params::required).
 */
bool Parameters::contains(const std::string& key) const
{
  return (m_params_map.find(key) != m_params_map.end());
} // method contains

/**
 * Method get
 *
 * If the key matches an element is returned the correspondent value,
 * otherwise is returned the default_value (an empty string by default).
 * Use the argument default_value if you have a default value for the key, in
 * this way if the key does not match any elements is returned that value.
 * You can also use the default_value as guard-value to check if the key match
 * an element.
 */
const std::string& Parameters::get
(
    const std::string& key,
    const std::string& default_value
) const
{
  std::map<std::string, std::string>::const_iterator it;
  it = m_params_map.find(key);
  if (it == m_params_map.end()) return default_value;
  return it->second;
} // method get

/**
 * Method info
 *
 * Converts this object into an Info<std::string> element and return it.
 */
Info<std::string> Parameters::info() const
{
  Info<std::string> inf;
  std::map<std::string, std::string>::const_iterator it;
  for (it = m_params_map.begin(); it != m_params_map.end(); ++it)
    inf.pushBack(it->first, it->first, it->second);
  return inf;
} // info

/**
 * Method isEmpty
 *
 * Returns true if no parameters are contained into this object, false
 * otherwise.
 */
bool Parameters::isEmpty() const
{
  return m_params_map.empty();
} // method isEmpty

/**
 * Method getBegin
 *
 * Returns a const iterator referring to the first parameter. Dereferencing
 * this iterator accesses the parameter by a std::pair where the first element
 * is the key and the second the parameter value.
 * Note that the order on which the parameters are store is not that on which
 * are inserted and may change at each insert.
 */
Parameters::const_iterator Parameters::getBegin() const
{
  return m_params_map.begin();
} // method getBegin

/**
 * Method getBool
 *
 * As the method get, plus a conversion to bool of the returned value.
 * The value is considered false only if is:
 *   - "false"
 *   - "FALSE"
 *   - "0"
 * otherwise is considered true.
 *
 * The argument default_value (returned if ket is not in the map) is setted to
 * false by default.
 */
bool Parameters::getBool(const std::string& key, bool default_value) const
{
  if (!this->contains(key))
    return default_value;

  std::string value = this->get(key);

  if (value == "FALSE" || value == "false" || value == "0")
    return false;

  return true;
} // method getBool

/**
 * Method getEnd
 *
 * Returns a const iterator referring to the past-the-end parameter.
 */
Parameters::const_iterator Parameters::getEnd() const
{
  return m_params_map.end();
} // method getEnd

/**
 * Method getReal
 *
 * As the method get, plus a conversion to Real of the returned value.
 * The argument default_value is setted to 0.0 by default.
 */
Global::Real Parameters::getReal
(
    const std::string& key,
    Global::Real default_value
) const
{
  if (!this->contains(key))
    return default_value;
  return Global::toReal(this->get(key));
} // method getReal

/**
 * Method getUint
 *
 * As the method get, plus a conversion to Uint of the returned value.
 * The argument default_value is setted to 0 by default.
 */
Global::Uint Parameters::getUint
(
    const std::string& key,
    Global::Uint default_value
) const
{
  if (!this->contains(key))
    return default_value;
  return Global::toUint(this->get(key));
} // method getUint

/**
 * Function merge
 *
 * The parameters contained in <p> are merged with the parameters in this
 * object. If some parameters already exists will be overwritten by those
 * contained in <p>.
 */
void Parameters::merge(const Parameters& p)
{
  std::map<std::string, std::string>::const_iterator it;
  for (it = p.m_params_map.begin(); it != p.m_params_map.end(); ++it)
    this->m_params_map[it->first] = it->second;
  return;
} // function merge

/**
 * operator[]
 *
 * If "key" matches the key of an element, the method returns a reference to
 * its mapped value. If "key" does not match the key of any element, the
 * method inserts a new element with that key and returns a reference to its
 * mapped value. If no mapped value is assigned to the element, it is
 * constructed as an empty string.
 */
std::string& Parameters::operator[](const std::string& key)
{
  return m_params_map[key];
} // operator[]

/**
 * operator=
 *
 * Assigment operator. Does what do you expect.
 */
Parameters& Parameters::operator=(const Parameters& params)
{
  m_params_map = params.m_params_map;
  m_verbose = params.m_verbose;
  m_msg_prefix = params.m_msg_prefix;
  m_msg_suffix = params.m_msg_suffix;
  return *this;
} // operator=

/**
 * Method print
 *
 * Prints, on the output stream passed as argument, all the parameters
 * contained in this object in the form:
 *   "key": "value"
 */
void Parameters::print(std::ostream& os) const
{
  std::map<std::string, std::string>::const_iterator it;
  for (it = m_params_map.begin(); it != m_params_map.end(); ++it)
    os <<"\"" <<it->first <<"\": \"" <<it->second <<"\"\n";
  return;
} // method print

/**
 * Method readFromIni
 *
 * Read parameters from the specified .ini file from the section SECTION.
 * In case of errors (invalid file or section not found) the method returns
 * false and this object is left empty. In case of success the method returns
 * true. Note that if the ini file is empty or if you specify a valid but empty
 * section this method returns true but the object will be empty.
 */
bool Parameters::readFromIni
(
    const std::string& ini_file,
    const std::string& section
)
{
  clear();

  try
  {
    bpt::ptree ini_tree;
    bpt::ini_parser::read_ini(ini_file, ini_tree);

    if (!section.empty())
      ini_tree = ini_tree.get_child(section);

    bpt::ptree::const_iterator it;
    for (it = ini_tree.begin(); it != ini_tree.end(); ++it)
      this->set(it->first, it->second.data());
  }
  catch(std::exception& ex)
  {
    // Log::err << "Parameters::readFromIni: " << ex.what() << "." << Log::endl;
    clear();
    return false;
  }

  return true;
} // method readFromIni

/**
 * Method remove
 *
 * Removes the parameter <key> from the object.
 */
void Parameters::remove(const std::string& key)
{
  m_params_map.erase(key);
  return;
} // method remove

/**
 * Method setBool
 *
 * Sets the element <key> with <value> of type bool. If the key don't exist an
 * element with <key> and <value> is created.
 */
void Parameters::setBool(const std::string& key, bool value)
{
  if (value)
    this->set(key, "true");
  else
    this->set(key, "false");
  return;
} // method setBool

/**
 * Method set
 *
 * Sets the element <key> with <value>. If the key don't exist an element
 * with <key> and <value> is created.
 */
void Parameters::set(const std::string& key, const std::string& value)
{
  m_params_map[key] = value;
  return;
} // method set

/**
 * Method setDefault
 *
 * If in the map is already presents a value for the passed key this method
 * does nothing, otherwise sets the passed value in the element.
 */
void Parameters::setDefault
(
    const std::string& key,
    const std::string& value
)
{
  std::map<std::string, std::string>::iterator it;
  if ((it = m_params_map.find(key)) != m_params_map.end())
    return;
  m_params_map[key] = value;
  return;
} // method setDefault

/**
 * Method setIfNot
 *
 * Sets the element <key> with <value> (see the method "set") only if <value>
 * is different from <notvalue> (the empty string for default), otherwise the
 * parameter is left unchanged.
 */
void Parameters::setIfNot(
    const std::string& key,
    const std::string& value,
    const std::string& notvalue)
{
  if (value != notvalue) this->set(key, value);
  return;
} // method setIfNot

/**
 * Method setIfNot
 *
 * Sets the element <key> with <value> (see the method "set") only if <value>
 * is different from <notvalue> (0 for default), otherwise the parameter is
 * left unchanged.
 */
void Parameters::setIfNot
(
    const std::string& key,
    const Global::Uint& value,
    const Global::Uint& notvalue
)
{
  if (value != notvalue)
    set(key, Global::toString(value));
  return;
} // method setIfNot

/**
 * Method setIfNot
 *
 * Sets the element <key> with <value> (see the method "set") only if <value>
 * is different from <notvalue> (0.0 for default), otherwise the parameter is
 * left unchanged.
 */
void Parameters::setIfNot
(
    const std::string& key,
    const Global::Real& value,
    const Global::Real& notvalue
)
{
  if (value != notvalue)
    set(key, Global::toString(value));
  return;
} // method setIfNot

/**
 * Method setReal
 *
 * Sets the element <key> with <value> of type Real. If the key don't exist an
 * element with <key> and <value> is created.
 */
void Parameters::setReal(const std::string& key, Real value)
{
  set(key, Global::toString(value));
  return;
} // method setReal

/**
 * Method setUint
 *
 * Sets the element <key> with <value> of type Uint. If the key don't exist an
 * element with <key> and <value> is created.
 */
void Parameters::setUint(const std::string& key, Uint value)
{
  set(key, Global::toString(value));
  return;
} // method setUint

/**
 * Method setMsgPrefix
 *
 * Sets the prefix of the log message: see the method setVerboseMode.
 */
void Parameters::setMsgPrefix(const std::string& prefix)
{
  m_msg_prefix = prefix;
  return;
} // method setMsgPrefix

/**
 * Method setMsgSuffix
 *
 * Sets the suffix of the log message: see the method setVerboseMode.
 */
void Parameters::setMsgSuffix(const std::string& suffix)
{
  m_msg_suffix = suffix;
  return;
} // method setMsgSuffix

/**
 * Method setVerboseMode
 *
 * If setted to "true" the method "check" prints on Log::vrb a message with the
 * following form whenever there are invalid parameters (or values):
 *   <prefix>message that describes the error<suffix><endl>
 * where the <prefix> and the <suffix> can be setted respectively with the
 * method "setMsgPrefix" and "setMsgSuffix", the defaults are "Params::check: "
 * for the first and a dot "." for the second one.
 */
void Parameters::setVerboseMode(bool verbose)
{
  m_verbose = verbose;
  return;
} // method setVerboseMode

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method clearObject
 *
 * Clears this object as just created (setting members to their initial values).
 */
void Parameters::clearObject()
{
  m_params_map.clear();
  m_verbose = false;
  m_msg_prefix = "Parameters::check: ";
  m_msg_suffix = ".";
  return;
} // method clearObject

/**
 * Method printMsg
 *
 * If is setted the verbose mode is printed on Log::vrb the following string:
 *   <msg_prefix><msg><msg_suffix><endl>
 */
void Parameters::printMsg(const std::string& msg) const
{
  if (m_verbose)
    Log::vrb << m_msg_prefix << msg << m_msg_suffix << Log::endl;
  return;
} // method printMsg

// =================
// RELATED FUNCTIONS
// =================

/**
 * operator<<
 */
std::ostream& operator<<(std::ostream& os, const Parameters& params)
{
  params.print(os);
  return os;
} // operator<<

} // namespace util
} // namespace moka
