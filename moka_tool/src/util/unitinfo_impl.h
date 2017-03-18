#ifndef UTIL_UNITINFO_IMPL_H
#define UTIL_UNITINFO_IMPL_H

#include "unitinfo.h"

#include <stdexcept>

namespace util {

//! Constructor
inline
UnitInfo::UnitInfo() :
  m_row(0),
  m_col(0),
  m_weight(0.0)
{ }

//! Reference to the last label
inline
UnitInfo::Label& UnitInfo::backLabel()
{
  return m_labels.back();
}

//! Const reference to the last label
inline
const UnitInfo::Label& UnitInfo::backLabel() const
{
  return m_labels.back();
}

//! Unit column
inline
unsigned int& UnitInfo::getCol()
{
  return m_col;
}

//! Unit column
inline
unsigned int UnitInfo::getCol() const
{
  return m_col;
}

//! Reference to the i-th label
inline
UnitInfo::Label& UnitInfo::getLabel(size_t i)
{
  if (i >= m_labels.size())
    throw std::out_of_range("UnitInfo::getLabel: out of range");
  return m_labels[i];
}

//! Const reference to the i-th label
inline
const UnitInfo::Label& UnitInfo::getLabel(size_t i) const
{
  if (i >= m_labels.size())
    throw std::out_of_range("UnitInfo::getLabel: out of range");
  return m_labels[i];
}

//! All the labels in a vector
inline
std::vector<UnitInfo::Label>& UnitInfo::getLabels()
{
  return m_labels;
}

//! All the labels in a vector
inline
const std::vector<UnitInfo::Label>& UnitInfo::getLabels() const
{
  return m_labels;
}

//! Number of labels
inline
size_t UnitInfo::getLabelsSize() const
{
  return m_labels.size();
}

//! Unit row
inline
unsigned int& UnitInfo::getRow()
{
  return m_row;
}

//! Unit row
inline
unsigned int UnitInfo::getRow() const
{
  return m_row;
}

//! Unit weight
inline
float& UnitInfo::getWeight()
{
  return m_weight;
}

//! Unit weight
inline
float UnitInfo::getWeight() const
{
  return m_weight;
}

//! Add one label with the passed name
inline
void UnitInfo::pushBackLabel(const QString& name)
{
  m_labels.push_back(Label(name));
}

//! Return the index of the label NAME or the labels size (see getLabelsSize)
//! if the label was not found. If FOUND is not null it is setted true if
//! the label is found, false otherwise.
inline
size_t UnitInfo::searchLabel(const QString& name, bool *found) const
{
  size_t i = 0;
  for (/* nop */; i < m_labels.size(); ++i)
    if (m_labels[i].getName() == name)
      break;

  if (i == m_labels.size())
  {
    if (found)
      *found = false;

    return m_labels.size();
  }

  if (found)
    *found = true;

  return i;
} // method searchLabel

//! Unit column
inline
void UnitInfo::setCol(unsigned int col)
{
  m_col = col;
}

//! Unit row
inline
void UnitInfo::setRow(unsigned int row)
{
  m_row = row;
}

//! Unit weight
inline
void UnitInfo::setWeight(float weight)
{
  m_weight = weight;
}

// =====================
// CLASS UNITINFO::LABEL
// =====================

//! Constructor
inline
UnitInfo::Label::Label()
{ }

//! Constructor
inline
UnitInfo::Label::Label(const QString& name) :
  m_name(name)
{ }

//! Frequency of i-th label value
inline
unsigned int& UnitInfo::Label::getFreq(size_t i)
{
  if (i >= m_freq.size())
    throw std::out_of_range("UnitInfo::Label::getFreq: out of range");
  return m_freq[i];
}

//! Frequency of i-th label value
inline
unsigned int UnitInfo::Label::getFreq(size_t i) const
{
  if (i >= m_freq.size())
    throw std::out_of_range("UnitInfo::Label::getFreq: out of range");
  return m_freq[i];
}

//! Label name
inline
QString& UnitInfo::Label::getName()
{
  return m_name;
}

//! Label name
inline
const QString& UnitInfo::Label::getName() const
{
  return m_name;
}

//! Number of label values (pairs <value, freq>)
inline
size_t UnitInfo::Label::getSize() const
{
  return m_val.size();
}

//! Value of i-th label value
inline
QString& UnitInfo::Label::getVal(size_t i)
{
  if (i >= m_val.size())
    throw std::out_of_range("UnitInfo::Label::getVal: out of range");
  return m_val[i];
}

//! Value of i-th label value
inline
const QString& UnitInfo::Label::getVal(size_t i) const
{
  if (i >= m_val.size())
    throw std::out_of_range("UnitInfo::Label::getVal: out of range");
  return m_val[i];
}

//! Push back in the list a pair <val, freq>
inline
void UnitInfo::Label::pushBack(const QString& val, unsigned int freq)
{
  m_val.push_back(val);
  m_freq.push_back(freq);
  return;
}

//! Frequency of i-th label value
inline
void UnitInfo::Label::setFreq(size_t i, unsigned int freq)
{
  if (i >= m_freq.size())
    throw std::out_of_range("UnitInfo::Label::setFreq: out of range");
  m_freq[i] = freq;
  return;
}

//! Label name
inline
void UnitInfo::Label::setName(const QString& name)
{
  m_name = name;
}

//! Value of i-th label value
inline
void UnitInfo::Label::setVal(size_t i, const QString& val)
{
  if (i >= m_val.size())
    throw std::out_of_range("UnitInfo::Label::setVal: out of range");
  m_val[i] = val;
  return;
}

} // namespace util

#endif // UTIL_UNITINFO_IMPL_H
