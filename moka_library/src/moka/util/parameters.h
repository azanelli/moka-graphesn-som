#ifndef MOKA_UTIL_PARAMETERS_H
#define MOKA_UTIL_PARAMETERS_H

#include <map>
#include <string>
#include <moka/global.h>
#include <moka/util/info.h>

namespace moka {
namespace util {

/**
 * Class Parameters
 *
 * Use this class like a std::map to store parameters, with strings as key and
 * value. To retrieve the parameters you can use the method "get" (which doesn't
 * adds elements in the map as the operator [] does).
 *
 * To check the parameters you can use the method "check" with a combination of
 * the following flags:
 *
 *   - optional: the parameter may not exist.
 *   - required: must exist (default behavior).
 *
 * Conversion flags (try to convert the parameter into a specified type in
 * order to apply other flags):
 *   - uint: will be converted as Uint.
 *   - real: will be converted as Real.
 *
 * Number flags (to use only if a conversion flag is setted, e.g. with the
 * flag Prm::real):
 *   - in_range: must be in the specified range (to call with two additional
 *       arguments that are the limits of the range).
 *   - less_than: must be less than the passad value (to call with one
 *       additional argument that is the passed value).
 *   - positive: the number must be greater than 0 (> 0).
 *   - non_zero: the number should not be 0 (!= 0).
 *   - non_negative: the number must be greater or equal than 0 (>= 0).
 *
 * You can combines the flags by using the operator |, for example:
 *
 *   params.check("n", Prm::required | Prm::uint | Prm::non_zero);
 *
 * Some flags requires one or two additional values: you must specify the
 * values as additional arguments, for example:
 *
 *   params.check("n", Prm::real | Prm::in_range, "-1.0", "1.0");
 *
 * tha arguments will be converted in the specified conversion (e.g. Real).
 * Attention: you can use only one at time of this type of flags, which takes
 * additional arguments.
 *
 * You can use also the alias Prm instead of Parameters for a better (cleaner)
 * flags access (e.g. Prm::optional).
 */
class Parameters
{
  public:
    enum Flags
    {
      optional         = (1<<0),
      required         = (1<<1),
      uint             = (1<<2),
      real             = (1<<3),
      in_range         = (1<<4),
      less_than        = (1<<5),
      positive         = (1<<6),
      non_zero         = (1<<7),
      non_negative     = (1<<8),
      non_empty        = (1<<9)
    }; // Flags

    typedef std::map<std::string, std::string>::const_iterator const_iterator;

    Parameters();
    Parameters(const Parameters& params);
    ~Parameters();

    std::string& at(const std::string& key);
    bool check(
        const std::string& key,
        int flags = required,
        const std::string& x1 = std::string(),
        const std::string& x2 = std::string()) const;
    void clear();
    bool contains(const std::string& key) const;
    const std::string& get(
        const std::string& key,
        const std::string& default_value = "") const;
    Info<std::string> info() const;
    bool isEmpty() const;
    const_iterator getBegin() const;
    bool getBool(const std::string& key, bool default_value = false) const;
    const_iterator getEnd() const;
    Global::Real getReal(
        const std::string& key,
        Global::Real default_value = 0.0) const;
    Global::Uint getUint(
        const std::string& key,
        Global::Uint default_value = 0) const;
    void merge(const Parameters& p);
    std::string& operator[](const std::string& key);
    Parameters& operator=(const Parameters& params);
    void print(std::ostream& os) const;
    bool readFromIni(
        const std::string& ini_file,
        const std::string& section);
    void remove(const std::string& key);
    void set(const std::string& key, const std::string& value = "");
    void setBool(const std::string& key, bool value = true);
    void setDefault(const std::string& key, const std::string& value);
    void setIfNot(
        const std::string& key,
        const std::string& value,
        const std::string& notvalue = "");
    void setIfNot(
        const std::string& key,
        const Global::Uint& value,
        const Global::Uint& notvalue = 0);
    void setIfNot(
        const std::string& key,
        const Global::Real& value,
        const Global::Real& notvalue = 0.0);
    void setReal(const std::string& key, Global::Real value = 0.0);
    void setUint(const std::string& key, Global::Uint value = 0);
    void setMsgPrefix(const std::string& prefix);
    void setMsgSuffix(const std::string& suffix);
    void setVerboseMode(bool verbose = true);

  private:
    std::map<std::string, std::string> m_params_map;
    bool m_verbose;
    std::string m_msg_prefix;
    std::string m_msg_suffix;

    void clearObject();
    void printMsg(const std::string& msg) const;

}; // class Parameters

std::ostream& operator<<(std::ostream& os, const Parameters& params);

// Use this alias for a better flags access (e.g. Prm::optional).
typedef Parameters Prm;

} // namespace util
} // namespace moka

#endif // MOKA_UTIL_PARAMETERS_H
