#ifndef MOKA_UTIL_INFO_H
#define MOKA_UTIL_INFO_H

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <moka/global.h>

namespace moka {
namespace util {

/**
 * Class Info
 *
 * A fixed-order container of information (InfoEntry). You can access the info
 * through the operator [] or through the method at() both using the entry
 * index or by the entry "key" (in this late case the behaviour is like a map:
 * if the key is not found an entry with that key is created and returned).
 * The info are mantained always in the order on which they were putted
 * into the container through the method "pushBack" or added with a call
 * of method at(key) or operator[key].
 * Entries with the same key are allowed but discouraged because in future can
 * be denied and the access through the key itself (operator[key]) returns only
 * the first entry found with that key.
 *
 * You can use the operator << to print an object of type Info. Some methods
 * allow you to choose the output format:
 *   - list(const std::string&): the info entries will be printed one below the
 *       other prefixed by the passed string.
 *   - csvHeader(): the names and the notes of the entries will be printed in
 *       one line comma separated with the name followed by the note in
 *       brackets.
 *   - csvLine(): the entries values will be printed in one line comma
 *       separated.
 *
 * If you call one of the above methods on an Info object all next prints will
 * be in that format. All the above methods return a reference to the object
 * itself, so you can do something like that:
 *
 *    Info info;
 *    // fills the info object
 *    // ...
 *    std::cout << info.getTitle() << std::endl;
 *    std::cout << info.list("  > ") << std::endl;
 *    // ...
 *    std::cout << info.csvHeader() << std::endl;
 *    std::cout << info.csvLine() << std::endl;
 *
 * To handle an information title see also setTitle and getTitle methods.
 *
 * WARNING: keep attention to don't put together in the same print statement
 * two (or more) format change commands since will have effect only the last
 * evaluated. For example:
 *
 *   std::cout << info.csvHeader() << std::endl
 *             << info.csvLine() << std::endl;  // wrong
 *
 * in the above statement the info object will be printed formatted to only
 * one format (csvHeader or csvLine) depending on the evaluation order of the
 * opearator <<.
 *
 * If you have only a const access to an object of type Info you can use
 * instead methods getList, getCsvHeader or getCsvLine.
 */
template <typename ValueType>
class Info
{
  public:
    typedef Global::Uint Uint;

    class Entry;

    Info();
    ~Info();
    Info(const Info& r);

    Entry& at(Uint i);
    const Entry& at(Uint i) const;
    Entry& at(const std::string& key);
    void clear();
    Info& csvHeader();
    Info& csvLine();
    Entry& getBack();
    const Entry& getBack() const;
    std::string getCsvHeader() const;
    std::string getCsvLine() const;
    std::string getList(const std::string& prefix = "") const;
    const std::string& getTitle() const;
    Uint getSize() const;
    Uint isEmpty() const;
    Info& list(const std::string& prefix = "");
    Info& operator=(const Info& r);
    Entry& operator[](Uint i);
    const Entry& operator[](Uint i) const;
    Entry& operator[](const std::string& key);
    void print(std::ostream& os) const;
    void printCsvHeader(std::ostream& os) const;
    void printCsvLine(std::ostream& os) const;
    void printList(std::ostream& os, const std::string& prefix = "") const;
    void pushBack(const Entry& entry);
    void pushBack(
        const std::string& key,
        const std::string& name,
        const ValueType& value,
        const std::string& note = "");
    void setTitle(const std::string& title);

  private:
    enum PrintMode
    {
      list_mode,
      csv_header_mode,
      csv_line_mode
    }; // enum PrintMode

    std::vector<Entry*> m_entries_vect;
    std::string m_title;
    PrintMode m_print_mode;
    std::string m_print_list_prefix;

    void clearVector();
    Entry& findOrCreate(const std::string& key);

}; // class Info

template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const Info<ValueType>& info);

/**
 * Class Info::Entry
 *
 * An information entry in the form:
 *   (key, name, value, note)
 * where "key" is a compact identifier of the result  (mainly used to access
 * the result if it is within a container, "name" is the description of the
 * result content, "value" is the value of the information and "note" is a
 * final note (e.g the unit measure).
 */
template <typename ValueType>
class Info<ValueType>::Entry
{
  public:
    Entry(
        const std::string& in_key,
        const std::string& in_name,
        const ValueType& in_value,
        const std::string& in_note) :
      key(in_key),
      name(in_name),
      value(in_value),
      note(in_note)
    { }

    Entry(const Entry& entry) :
      key(entry.key),
      name(entry.name),
      value(entry.value),
      note(entry.note)
    { }

    const std::string key;
    std::string name;
    ValueType value;
    std::string note;

  private:
    Entry& operator=(const Entry&);

}; // class Entry

} // namespace util
} // namespace moka

#include "info_impl.h"

#endif // MOKA_UTIL_INFO_H
