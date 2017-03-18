#ifndef UTIL_UNITINFO_H
#define UTIL_UNITINFO_H

#include <utility>
#include <vector>
#include <QString>

namespace util {

/**
 * Class UnitInfo
 *
 * Container for unit informations.
 */
class UnitInfo
{
  public:
    class Label;

    UnitInfo();

    Label& backLabel();
    const Label& backLabel() const;
    unsigned int& getCol();
    unsigned int getCol() const;
    Label& getLabel(size_t i);
    const Label& getLabel(size_t i) const;
    std::vector<Label>& getLabels();
    const std::vector<Label>& getLabels() const;
    size_t getLabelsSize() const;
    unsigned int& getRow();
    unsigned int getRow() const;
    float& getWeight();
    float getWeight() const;
    void pushBackLabel(const QString& name);
    size_t searchLabel(const QString& name, bool *found = NULL) const;
    void setCol(unsigned int col);
    void setRow(unsigned int row);
    void setWeight(float weight);

  private:
    unsigned int m_row;
    unsigned int m_col;
    float m_weight;
    std::vector<Label> m_labels;

}; // class UnitInfo

/**
 * Class UnitInfo::Label
 *
 */
class UnitInfo::Label
{
  public:
    Label();
    Label(const QString& name);

    unsigned int& getFreq(size_t i);
    unsigned int getFreq(size_t i) const;
    QString& getName();
    const QString& getName() const;
    size_t getSize() const;
    QString& getVal(size_t i);
    const QString& getVal(size_t i) const;
    void pushBack(const QString& val, unsigned int freq);
    void setFreq(size_t i, unsigned int freq);
    void setName(const QString& name);
    void setVal(size_t i, const QString& val);

  private:
    QString m_name;
    std::vector<QString> m_val;
    std::vector<unsigned int> m_freq;

}; // class UnitInfo::Label

} // namespace util

#include "unitinfo_impl.h"

#endif // UTIL_UNITINFO_H
