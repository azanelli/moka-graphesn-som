#ifndef OB_DEPICTEXT_H
#define OB_DEPICTEXT_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <openbabel/babelconfig.h>
#include <openbabel/math/vector3.h>
#include <openbabel/mol.h>
#include <openbabel/alias.h>
#include <openbabel/atomclass.h>
#include <openbabel/stereo/stereo.h>
#include <openbabel/obiter.h>
#include <openbabel_ext/depict/painter.h>

namespace OpenBabel
{

class OBMol;
class OBPainter;
class OBDepictExtPrivate;

#ifndef OBDEPICT
  #define OBDEPICT
#endif

/**
 * Class OBDepictExt
 *
 */
class OBDEPICT OBDepictExt
{
  public:
    enum AtomLabelType
    {
      AtomId = 1,
      AtomIndex,
      AtomSymmetryClass,
      AtomValence,
      AtomTetrahedralStereo,
      AtomCustomLabel
    };

    enum OptionType
    {
      bwAtoms              = 0x0001,
      internalColor        = 0x0002,
      drawTermC            = 0x0010,
      drawAllC             = 0x0020,
      noWedgeHashGen       = 0x0100,
      asymmetricDoubleBond = 0x0200
    };

    enum LabelAlignment
    {
      LeftAlignment,
      RightAlignment,
      UpAlignment,
      DownAlignment
    };

    OBDepictExt(OBPainter *painter);
    ~OBDepictExt();
    bool AddAtomLabels(AtomLabelType type);
    bool DrawMolecule(OBMol *mol);
    void SetBondLength(double length);
    double GetBondLength() const;
    void SetPenWidth(double width);
    double GetPenWidth() const;
    void SetBondSpacing(double spacing);
    double GetBondSpacing() const;
    void SetBondWidth(double width);
    double GetBondWidth() const;
    //void SetDrawingTerminalCarbon(bool enabled);
    //bool GetDrawingTerminalCarbon() const;
    void SetOption(unsigned opts);
    unsigned GetOptions() const;
    void ClearOptions();
    void SetFontFamily(const std::string &family);
    const std::string& GetFontFamily() const;
    void SetFontSize(int pointSize, bool subscript = false);
    int GetFontSize(bool subscript = false) const;
    void SetAliasMode(bool b=true);
    void SetBondColor(const std::string& scolor);

  private:
    OBDepictExtPrivate * const d;

    static LabelAlignment GetLabelAlignment(OBAtom *atom);
    static unsigned int GetAtomSymClass(OBAtom *atom);

    void addCustomAtomLabel
    (
        OBAtom *atom,
        const vector3& atom_pos,
        OBPainter *painter
    );

}; // class OBDepictExt

/**
 * Class OBDepictExtPrivate
 *
 * ...
 */
class OBDepictExtPrivate
{
  public:

    /**
     * Constructor
     */
    OBDepictExtPrivate();
    void DrawSimpleBond(OBAtom *beginAtom, OBAtom *endAtom, int order);
    void DrawWedge(OBAtom *beginAtom, OBAtom *endAtom);
    void DrawHash(OBAtom *beginAtom, OBAtom *endAtom);
    void DrawWobblyBond(OBAtom *beginAtom, OBAtom *endAtom);
    void DrawRingBond(OBAtom *beginAtom, OBAtom *endAtom, const vector3 &center, int order);
    void DrawAtomLabel(const std::string &label, OBDepictExt::LabelAlignment alignment, const vector3 &pos);
    bool HasLabel(OBAtom *atom);
    void SetWedgeAndHash(OBMol* mol);

  public:
    OBMol     *mol;
    OBPainter *painter;
    double     bondLength;
    double     penWidth;
    double     bondSpacing;
    double     bondWidth;
    //bool       drawTerminalC;
    int        fontSize, subscriptSize;
    bool       aliasMode;
    std::string fontFamily;
    OBColor    bondColor;
    unsigned   options;

}; // class OBDepictExtPrivate

} // namespace OpenBabel

#include "depictext_impl.h"

#endif // OB_DEPICTEXT_H
