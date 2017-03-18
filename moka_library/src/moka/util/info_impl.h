#ifndef MOKA_UTIL_INFO_IMPL_H
#define MOKA_UTIL_INFO_IMPL_H

#include "info.h"

#include <sstream>

namespace moka {
namespace util {

/**
 * Constructor Info
 */
template <typename T>
Info<T>::Info() :
  m_title(""),
  m_print_mode(list_mode),
  m_print_list_prefix("")
{ }

/**
 * Destructor ~Info
 */
template <typename T>
Info<T>::~Info()
{
  clearVector();
} // destructor

/**
 * Copy constructor Info
 */
template <typename T>
Info<T>::Info(const Info<T>& r)
{
  this->operator=(r);
} // copy constructor

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method at
 */
template <typename T>
typename Info<T>::Entry& Info<T>::at(Uint i)
{
  return *m_entries_vect.at(i);
} // method at

/**
 * Method at
 */
template <typename T>
const typename Info<T>::Entry& Info<T>::at(Uint i) const
{
  return *m_entries_vect.at(i);
} // method at

/**
 * Method at
 */
template <typename T>
typename Info<T>::Entry& Info<T>::at(const std::string& key)
{
  return findOrCreate(key);
} // method at

/**
 * Method clear
 */
template <typename T>
void Info<T>::clear()
{
  clearVector();
  m_title.clear();
  m_print_mode = list_mode;
  m_print_list_prefix.clear();
} // method clear

/**
 * Method csvHeader
 *
 * Sets the print method as "csv header". Will be printed the names and the
 * notes of the entries in one line comma separated with the name followed by
 * the note in brackets.
 *
 * For example, if you write this code:
 *   Info<std::string> info;
 *   info.pushBack("info1", "Name entry 1", "60", "%");
 *   info.pushBack("info2", "Name entry 2", "value entry 2", "note entry 2");
 *   info.pushBack("info3", "Name entry 3", "0.03", "sec.");
 *   std::cout << info.csvHeader() << std::endl;
 *   std::cout << info.csvLine() << std::endl;
 *
 * Will be printed:
 *   Name entry 1 (%), Name entry 2 (note entry 2), Name entry 3 (sec.)
 *   60, value entry 2, 0.03
 *
 * This type of printing make sense if you have something that procuce more
 * than one info element with a fixed structure and you want print out all
 * the info in a csv-like format.
 */
template <typename T>
Info<T>& Info<T>::csvHeader()
{
  m_print_mode = csv_header_mode;
  return *this;
} // method csvHeader

/**
 * Method csvLine
 *
 * Sets the print method as "csv line". Will be printed only the entry values
 * comma separated.
 *
 * For example, if you write this code:
 *   Info<std::string> info;
 *   info.pushBack("info1", "Name entry 1", "60", "%");
 *   info.pushBack("info2", "Name entry 2", "value entry 2", "note entry 2");
 *   info.pushBack("info3", "Name entry 3", "0.03", "sec.");
 *   std::cout << info.csvHeader() << std::endl;
 *   std::cout << info.csvLine() << std::endl;
 *
 * Will be printed:
 *   Name entry 1 (%), Name entry 2 (note entry 2), Name entry 3 (sec.)
 *   60, value entry 2, 0.03
 *
 * This type of printing make sense if you have something that procuce more
 * than one info element with a fixed structure and you want print out all
 * the info in a csv-like format.
 */
template <typename T>
Info<T>& Info<T>::csvLine()
{
  m_print_mode = csv_line_mode;
  return *this;
} // method csvLine

/**
 * Method getBack
 */
template <typename T>
typename Info<T>::Entry& Info<T>::getBack()
{
  return *m_entries_vect.back();
} // method getBack

/**
 * Method getBack
 */
template <typename T>
const typename Info<T>::Entry& Info<T>::getBack() const
{
  return *m_entries_vect.back();
} // method getBack

/**
 * Method getCsvHeader
 *
 * See the method "csvHeader". This method returns a string that contains the
 * object Info printed as if was call the method csvHeader. This method is
 * useful if you want print the object Info without modify it (e.g. if you
 * have only const access).
 * You can also use the method "printCsvHeader".
 */
template <typename T>
std::string Info<T>::getCsvHeader() const
{
  std::stringstream ss;
  printCsvHeader(ss);
  return ss.str();
} // method getCsvHeader

/**
 * Method getCsvLine
 *
 * See the method csvLine. This method returns a string that contains the
 * object Info printed as if was call the method "csvLine". This method is
 * useful if you want print the object Info without modify it (e.g. if you
 * have only const access).
 * You can also use the method "printCsvLine".
 */
template <typename T>
std::string Info<T>::getCsvLine() const
{
  std::stringstream ss;
  printCsvLine(ss);
  return ss.str();
} // method getCsvLine

/**
 * Method getList
 *
 * See the method "list". This method returns a string that contains the
 * object Info printed as if was call the method "list". This method is
 * useful if you want print the object Info without modify it (e.g. if you
 * have only const access).
 * You can also use the method "printList".
 */
template <typename T>
std::string Info<T>::getList(const std::string& prefix) const
{
  std::stringstream ss;
  printList(ss, prefix);
  return ss.str();
} // method getList

/**
 * Method getTitle
 */
template <typename T>
const std::string& Info<T>::getTitle() const
{
  return m_title;
} // method getTitle

/**
 * Method getSize
 */
template <typename T>
typename Info<T>::Uint Info<T>::getSize() const
{
  return m_entries_vect.size();
} // method getSize

/**
 * Method isEmpty
 */
template <typename T>
typename Info<T>::Uint Info<T>::isEmpty() const
{
  return m_entries_vect.empty();
} // method isEmpty

/**
 * Method list
 *
 * Sets the print method as "list". The entries will be printed one below the
 * other prefixed by the passed string.
 *
 * For example, if you write this code:
 *   Info<std::string> info;
 *   info.setTitle("Info:");
 *   info.pushBack("info1", "Name entry 1", "60", "%");
 *   info.pushBack("info2", "Name entry 2", "value entry 2", "note entry 2");
 *   info.pushBack("info3", "Name entry 3", "0.03", "sec.");
 *   info.list("  > ");
 *   std::cout << info.getTitle() << std::endl;
 *   std::cout << info << std::endl;
 *
 * Will be printed:
 *   Info:
 *     > Name entry 1: 60 %
 *     > Name entry 2: value entry 2 note entry 2
 *     > Name entry 3: 0.03 sec.
 *
 */
template <typename T>
Info<T>& Info<T>::list(const std::string& prefix)
{
  m_print_mode = list_mode;
  m_print_list_prefix = prefix;
  return *this;
} // method list

/**
 * Method operator=
 */
template <typename T>
Info<T>& Info<T>::operator=(const Info& r)
{
  clear();
  for (Global::Uint i = 0; i < r.getSize(); ++i)
    pushBack(r.at(i));
  m_title = r.m_title;
  m_print_mode = r.m_print_mode;
  m_print_list_prefix = r.m_print_list_prefix;
  return *this;
} // method operator=

/**
 * Method operator[]
 */
template <typename T>
typename Info<T>::Entry& Info<T>::operator[](Uint i)
{
  return this->at(i);
} // method operator[]

/**
 * Method operator[]
 */
template <typename T>
const typename Info<T>::Entry& Info<T>::operator[](Uint i) const
{
  return this->at(i);
} // method operator[]

/**
 * Method operator[]
 */
template <typename T>
typename Info<T>::Entry& Info<T>::operator[](const std::string& key)
{
  return this->at(key);
} // method operator[]

/**
 * Method print
 */
template <typename T>
void Info<T>::print(std::ostream& os) const
{
  if (isEmpty())
    return;

  switch (m_print_mode)
  {
  case list_mode:
    printList(os, m_print_list_prefix);
    break;
  case csv_header_mode:
    printCsvHeader(os);
    break;
  case csv_line_mode:
    printCsvLine(os);
    break;
  } // switch

  return;
} // method print

/**
 * Method printCsvHeader
 *
 * See the method "csvHeader". This method prints on the passed stream the
 * object Info as if was call the method "csvHeader".
 */
template <typename T>
void Info<T>::printCsvHeader(std::ostream& os) const
{
  os << at(0).name << (at(0).note.empty() ? "" : " (" + at(0).note + ")");
  for (Uint i = 1; i < getSize(); ++i)
    os << ", " << at(i).name
       << (at(i).note.empty() ? "" : " (" + at(i).note + ")");
  return;
} // method printCsvHeader

/**
 * Method printCsvLine
 *
 * See the method "csvLine". This method prints on the passed stream the
 * object Info as if was call the method "csvLine".
 */
template <typename T>
void Info<T>::printCsvLine(std::ostream& os) const
{
  os << at(0).value;
  for (Uint i = 1; i < getSize(); ++i)
    os << ", " << at(i).value;
  return;
} // method printCsvLine

/**
 * Method printList
 *
 * See the method "list". This method prints on the passed stream the
 * object Info as if was call the method "list".
 */
template <typename T>
void Info<T>::printList(std::ostream& os, const std::string& prefix) const
{
  os << prefix << at(0).name <<": " << at(0).value
     <<" " << at(0).note;
  for (Uint i = 1; i < getSize(); ++i)
    os << "\n" << prefix << at(i).name << ": " << at(i).value
       << " " << at(i).note;
  return;
} // method printList

/**
 * Method pushBack
 */
template <typename T>
void Info<T>::pushBack(const Info<T>::Entry& entry)
{
  m_entries_vect.push_back(new Info<T>::Entry(entry));
} // method pushBack

/**
 * Method pushBack
 */
template <typename T>
void Info<T>::pushBack
(
    const std::string& key,
    const std::string& name,
    const T& value,
    const std::string& note
)
{
  m_entries_vect.push_back(new Info<T>::Entry(key, name, value, note));
} // method pushBack

/**
 * Method setTitle
 */
template <typename T>
void Info<T>::setTitle(const std::string& title)
{
  m_title = title;
} // method setTitle

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method clearVector
 */
template <typename T>
void Info<T>::clearVector()
{
  std::for_each
      (
        m_entries_vect.begin(),
        m_entries_vect.end(),
        boost::lambda::delete_ptr()
      );
  m_entries_vect.clear();
  return;
} // method clearVector

/**
 * Method findOrCreate
 */
template <typename T>
typename Info<T>::Entry& Info<T>::findOrCreate(const std::string& key)
{
  for (Uint i = 0; i < m_entries_vect.size(); ++i)
    if (m_entries_vect[i]->key == key)
      return *m_entries_vect[i];

  m_entries_vect.push_back(
        new Info<T>::Entry(key, std::string(), T(), std::string()));

  return *m_entries_vect.back();
} // method findOrCreate

// =================
// RELATED FUNCTIONS
// =================

template <typename T>
std::ostream& operator<<(std::ostream& os, const Info<T>& info)
{
  info.print(os);
  return os;
} // operator <<

} // namespace util
} // namespace moka

#endif // MOKA_UTIL_INFO_IMPL_H
