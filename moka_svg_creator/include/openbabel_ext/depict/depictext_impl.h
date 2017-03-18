#ifndef OB_DEPICTEXT_IMPL_H
#define OB_DEPICTEXT_IMPL_H

#include "depictext.h"

#include <algorithm>
#include <iterator>
#include <cmath>
#include <iostream>
#include <sstream>
#include <openbabel/stereo/stereo.h>
#include <openbabel/obiter.h>
#include <openbabel/mol.h>
#include <openbabel/alias.h>
#include <openbabel/atomclass.h>
#include <openbabel_ext/depict/painter.h>

namespace OpenBabel
{

/**
 * Constructor.
 */
OBDepictExt::OBDepictExt(OBPainter *painter) :
  d(new OBDepictExtPrivate)
{
  d->painter = painter;
}

/**
 * Destructor.
 */
OBDepictExt::~OBDepictExt()
{
  delete d->mol;
  d->mol = NULL;
  delete d;
}

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method AddAtomLabels
 *
 * Draw atom labels of a specified @p type.
 *
 * @return True if successful.
 */
bool OBDepictExt::AddAtomLabels(AtomLabelType type)
{
  OBColor pencolor(35.0 / 255.0, 140.0 / 255.0, 65.0 / 255.0); // dark green

  d->painter->SetPenColor(pencolor);
  d->painter->SetFillColor(pencolor);
  d->painter->SetFontSize((int)(GetFontSize() * 0.7)); // smaller text

  OBAtomIterator i;
  for (OBAtom *atom = d->mol->BeginAtom(i); atom; atom = d->mol->NextAtom(i))
  {
    vector3 pos(atom->GetVector());
    std::stringstream ss;
    switch (type)
    {
    case AtomId:
      ss << atom->GetId();
      d->painter->DrawText(pos.x(), pos.y(), ss.str());
      break;

    case AtomSymmetryClass:
      ss << GetAtomSymClass(atom);
      d->painter->DrawText(pos.x(), pos.y(), ss.str());
      break;

    case AtomIndex:
      ss << atom->GetIdx();
      d->painter->DrawText(pos.x(), pos.y(), ss.str());
      break;

    case AtomCustomLabel:
      addCustomAtomLabel(atom, pos, d->painter);
      break;

    default:
      break;

    } // switch
  } //for

  return true;
} // method AddAtomLabels

/**
 * Method DrawMolecule
 *
 * Draw @p mol using the painter previously stored in the constructor.
 *
 * @return True if successful.
 */
bool OBDepictExt::DrawMolecule(OBMol *mol)
{
  if (!d->painter)
    return false;

  if (d->mol != NULL)
    delete d->mol;
  d->mol = new OBMol(*mol); // Copy it

  OBAtomClassData* pac = NULL;
  if (mol->HasData("Atom Class"))
    pac = static_cast<OBAtomClassData*>(mol->GetData("Atom Class"));

  double width = 0.0, height = 0.0;

  OBAtom *atom;
  OBBondIterator j;
  OBAtomIterator i;

  // Determine which should be wedge and hash bonds...
  // Note: we need to do this before we invert the y-coordinate for depiction
  std::map<OBBond*, enum OBStereo::BondDirection> updown;
  std::map<OBBond*, OBStereo::Ref> from;
  TetStereoToWedgeHash(*d->mol, updown, from);

  if (mol->NumAtoms() > 0)
  {
    // scale bond lengths and invert the y coordinate (both SVG and Cairo use
    // top left as the origin)
    double bondLengthSum = 0.0;
    for (OBBond *bond = mol->BeginBond(j); bond; bond = mol->NextBond(j))
      bondLengthSum += bond->GetLength();
    const double averageBondLength = bondLengthSum / mol->NumBonds();
    const double f = mol->NumBonds() ? d->bondLength / averageBondLength : 1.0;
    for (atom = d->mol->BeginAtom(i); atom; atom = d->mol->NextAtom(i))
      atom->SetVector(atom->GetX() * f, - atom->GetY() * f, 0.0);

    // find min/max values
    double min_x, max_x;
    double min_y, max_y;
    atom = d->mol->BeginAtom(i);
    min_x = max_x = atom->GetX();
    min_y = max_y = atom->GetY();
    for (atom = d->mol->NextAtom(i); atom; atom = d->mol->NextAtom(i))
    {
      min_x = std::min(min_x, atom->GetX());
      max_x = std::max(max_x, atom->GetX());
      min_y = std::min(min_y, atom->GetY());
      max_y = std::max(max_y, atom->GetY());
    }

    const double margin = 40.0;
    // translate all atoms so the bottom-left atom is at margin,margin
    for (atom = d->mol->BeginAtom(i); atom; atom = d->mol->NextAtom(i))
      atom->SetVector
      (
          atom->GetX() - min_x + margin,
          atom->GetY() - min_y + margin,
          0.0
      );

    width  = max_x - min_x + 2 * margin;
    height = max_y - min_y + 2 * margin;

    //d->painter->SetPenWidth(d->penWidth);
    //d->painter->SetPenColor(d->pen));
    //d->painter->SetFillColor(OBColor("black"));

  } // if (mol->NumAtoms() > 0)

  d->painter->NewCanvas(width, height);

  // highlight atoms
  for (atom = d->mol->BeginAtom(i); atom; atom = d->mol->NextAtom(i))
    if (atom->HasData("HighlightAtom"))
      d->painter->DrawCircle
      (
          atom->GetX(),
          atom->GetY() + 0.15 * GetFontSize(),
          0.9 * GetFontSize(),
          OBColor(1.0, 1.0, 0.78)
      );

  // draw bonds
  for (OBBond *bond = d->mol->BeginBond(j); bond; bond = d->mol->NextBond(j))
  {
    OBAtom *begin = bond->GetBeginAtom();
    OBAtom *end = bond->GetEndAtom();
    if ((d->options & internalColor) && bond->HasData("color"))
      d->painter->SetPenColor(OBColor(bond->GetData("color")->GetValue()));
    else
      d->painter->SetPenColor(d->bondColor);

    if(from.find(bond)!=from.end())
    {
      // is a wedge or hash bond
      if (from[bond] == bond->GetEndAtom()->GetId())
        std::swap(begin, end);
      if (updown[bond] == OBStereo::UpBond)
        d->DrawWedge(begin, end);
      else if (updown[bond] == OBStereo::DownBond)
        d->DrawHash(begin, end);
      else  // this is a bond to a chiral center specified as unknown
        d->DrawWobblyBond(begin, end);
    } // if
    else if (!bond->IsInRing())
      d->DrawSimpleBond(begin, end, bond->GetBO());

  } // for bond

  // draw ring bonds
  std::vector<OBRing*> rings(mol->GetSSSR());
  OBBitVec drawnBonds;
  for (std::vector<OBRing*>::iterator k = rings.begin(); k != rings.end(); ++k)
  {
    OBRing *ring = *k;
    std::vector<int> indexes = ring->_path;
    vector3 center(VZero);
    for (std::vector<int>::iterator l = indexes.begin(); l != indexes.end(); ++l)
    {
      center += d->mol->GetAtom(*l)->GetVector();
    }
    center /= indexes.size();

    for (unsigned int l = 0; l < indexes.size(); ++l)
    {
      OBAtom *begin = d->mol->GetAtom(indexes[l]);
      OBAtom *end;
      if (l+1 < indexes.size())
        end = d->mol->GetAtom(indexes[l+1]);
      else
        end = d->mol->GetAtom(indexes[0]);

      OBBond *ringBond = d->mol->GetBond(begin, end);
      if (drawnBonds.BitIsSet(ringBond->GetId()))
        continue;

      if ((d->options & internalColor) && ringBond->HasData("color"))
        d->painter->SetPenColor(OBColor(ringBond->GetData("color")->GetValue()));
      else
        d->painter->SetPenColor(d->bondColor);

      d->DrawRingBond(begin, end, center, ringBond->GetBO());
      drawnBonds.SetBitOn(ringBond->GetId());
    } // for l

  } // for k

  // draw atom labels
  for (atom = d->mol->BeginAtom(i); atom; atom = d->mol->NextAtom(i))
  {
    double x = atom->GetX();
    double y = atom->GetY();

    LabelAlignment alignment = GetLabelAlignment(atom);
    bool rightAligned = false;
    switch (alignment)
    {
      case RightAlignment:
        rightAligned = true;
      default:
        break;
    }

    if ((d->options & internalColor) && atom->HasData("color"))
      d->painter->SetPenColor(OBColor(atom->GetData("color")->GetValue()));
    else if(d->options & bwAtoms)
      d->painter->SetPenColor(d->bondColor);
    else
      d->painter->SetPenColor(OBColor(etab.GetRGB(atom->GetAtomicNum())));

    // charge and radical
    int charge = atom->GetFormalCharge();
    int spin = atom->GetSpinMultiplicity();
    if (charge || spin)
    {
      OBFontMetrics metrics = d->painter->GetFontMetrics("N");
      double yoffset =
          d->HasLabel(atom) ? -0.2 * metrics.height : -0.2 * metrics.height;

      /*
      switch (GetLabelAlignment(atom))
      {
      case UpAlignment:
      case LeftAlignment:
      case RightAlignment:
        yoffset = -1.2 * metrics.height;
        break;
      case DownAlignment:
        // nop
        break;
      }
      */

      std::stringstream ss;
      if(charge)
      {
        if (abs(charge) != 1)
          ss << abs(charge);
        ss << (charge > 0 ? "+" : "-") ;
      }
      if(spin)
      {
        ss << (spin == 2 ? "." : "..");
        yoffset += 0.5 * metrics.height;
      }
      if (spin || charge < 0)
        d->painter->SetFontSize(2 * metrics.fontSize);
      d->painter->DrawText(x + 0.4 * metrics.width, y + yoffset, ss.str());
      d->painter->SetFontSize(metrics.fontSize); //restore
    } // if (charge || spin)

    if (atom->IsCarbon())
    {
      if (!(d->options & drawAllC))
      {
        if (atom->GetValence() > 1)
          continue;
        if ((atom->GetValence() == 1) && !(d->options & drawTermC))
          continue;
      }
    } // if (atom->IsCarbon())

    std::stringstream ss;
    AliasData* ad = NULL;
    if(d->aliasMode && atom->HasData(AliasDataType))
      ad = static_cast<AliasData*>(atom->GetData(AliasDataType));

    // For unexpanded aliases use appropriate form of alias instead of element
    // symbol, Hs, etc
    if (ad && !ad->IsExpanded())
    {
      ss << ad->GetAlias(rightAligned);
      OBColor aliasColor =
          !ad->GetColor().empty() ? ad->GetColor() : d->bondColor;
      d->painter->SetPenColor(aliasColor);
    } // if

    // Atoms with no AliasData, but 0 atomic num and atomclass == n are output
    // as Rn
    else if (pac && atom->GetAtomicNum() == 0 && pac->HasClass(atom->GetIdx()))
    {
      ss << 'R' << pac->GetClass(atom->GetIdx());
      d->painter->SetPenColor(OBColor("black"));
    } // else if

    else
    {
      const char *atomSymbol;
      if (atom->IsHydrogen() && atom->GetIsotope() > 1)
        atomSymbol = atom->GetIsotope() == 2 ? "D" : "T";
      else
        atomSymbol = etab.GetSymbol(atom->GetAtomicNum());

      unsigned int hCount = atom->ImplicitHydrogenCount();
      // rightAligned:
      //   false  CH3
      //   true   H3C
      if (hCount && rightAligned)
        ss << "H";
      if ((hCount > 1) && rightAligned)
        ss << hCount;
      ss << atomSymbol;
      if (hCount && !rightAligned)
        ss << "H";
      if ((hCount > 1) && !rightAligned)
        ss << hCount;
    } // if-else

    d->DrawAtomLabel(ss.str(), alignment, vector3(x, y, 0.0));

  } // for atom

  return true;
} // method DrawMolecule

/**
 * Method SetBondLength
 */
void OBDepictExt::SetBondLength(double length)
{
  d->bondLength = length;
}

/**
 * Method GetBondLength
 */
double OBDepictExt::GetBondLength() const
{
  return d->bondLength;
}

/**
 * Method SetPenWidth
 */
void OBDepictExt::SetPenWidth(double width)
{
  d->penWidth = width;
  d->painter->SetPenWidth(width);
}

/**
 * Method GetPenWidth
 */
double OBDepictExt::GetPenWidth() const
{
  return d->penWidth;
}

/**
 * Method SetBondSpacing
 */
void OBDepictExt::SetBondSpacing(double spacing)
{
  d->bondSpacing = spacing;
}

/**
 * Method GetBondSpacing
 */
double OBDepictExt::GetBondSpacing() const
{
  return d->bondSpacing;
}

/**
 * Method SetBondWidth
 */
void OBDepictExt::SetBondWidth(double width)
{
  d->bondWidth = width;
}

/**
 * Method GetBondWidth
 */
double OBDepictExt::GetBondWidth() const
{
  return d->bondWidth;
}

/**
 * Method SetOption
 */
void OBDepictExt::SetOption(unsigned opts)
{
  d->options |= opts;
}

/**
 * Method GetOptions
 */
unsigned OBDepictExt::GetOptions() const
{
  return d->options;
}

/**
 * Method ClearOptions
 */
void OBDepictExt::ClearOptions()
{
  d->options = 0;
}

/**
 * Method SetFontFamily
 */
void OBDepictExt::SetFontFamily(const std::string &family)
{
  d->fontFamily = family;
  d->painter->SetFontFamily(family);
}

/**
 * Method GetFontFamily
 */
const std::string& OBDepictExt::GetFontFamily() const
{
  return d->fontFamily;
}

/**
 * Method SetFontSize
 */
void OBDepictExt::SetFontSize(int pointSize, bool subscript)
{
  if (subscript) {
    d->subscriptSize = pointSize;
    return;
  }

  d->fontSize = pointSize;
  d->subscriptSize = (int)(0.85 * pointSize);
}

/**
 * Method GetFontSize
 */
int OBDepictExt::GetFontSize(bool subscript) const
{
  if (subscript)
    return d->subscriptSize;
  return d->fontSize;
}

/**
 * Method SetAliasMode
 */
void OBDepictExt::SetAliasMode(bool b)
{
  d->aliasMode = b;
}

/**
 * Method SetBondColor
 */
void OBDepictExt::SetBondColor(const std::string& scolor)
{
  d->bondColor = scolor;
}

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method GetLabelAlignment
 */
OBDepictExt::LabelAlignment OBDepictExt::GetLabelAlignment(OBAtom *atom)
{
  // compute the sum of the bond vectors, this gives
  vector3 direction(VZero);
  OBBondIterator i;
  for (OBAtom *nbr = atom->BeginNbrAtom(i); nbr; nbr = atom->NextNbrAtom(i))
    direction += atom->GetVector() - nbr->GetVector();

  const double bias = -0.1; //towards left-alignment, which is more natural
  LabelAlignment alignment = LeftAlignment;
  if ((atom->GetValence() == 2) && (abs(direction.y()) > abs(direction.x())))
  {
    if (direction.y() <= 0.0)
      alignment = UpAlignment;
    else
      alignment = DownAlignment;
  }
  else
  {
    if (direction.x() < bias)
      alignment = RightAlignment;
    else
      alignment = LeftAlignment;
  }

  return alignment;
} // method GetLabelAlignment

/**
 * Method GetAtomSymClass
 */
unsigned int OBDepictExt::GetAtomSymClass(OBAtom *atom)
{
  OBPairData *pd = dynamic_cast<OBPairData*>(
        atom->GetParent()->GetData("OpenBabel Symmetry Classes"));
  if (pd)
  {

    std::cout << "same? = " << pd->GetValue() << std::endl;

    std::istringstream iss(pd->GetValue());
    std::vector<unsigned int> symmetry_classes;
    std::copy(std::istream_iterator<unsigned int>(iss),
         std::istream_iterator<unsigned int>(),
         std::back_inserter< std::vector<unsigned int> >(symmetry_classes));
    // Now find the number of unique elements
    std::vector<unsigned int> copy_sym = symmetry_classes;
    std::sort(copy_sym.begin(), copy_sym.end());
    std::vector<unsigned int>::iterator end_pos =
        std::unique(copy_sym.begin(), copy_sym.end()); // Requires sorted elements
    int nclasses = end_pos - copy_sym.begin();
    (void)nclasses;

    std::cout << "sym_class[" << atom->GetIndex() << "] = "
              << symmetry_classes.at(atom->GetIndex()) << std::endl;
    return symmetry_classes.at(atom->GetIndex());
  }

  return 99;
} // method GetAtomSymClass

/**
 * Method addCustomAtomLabel
 *
 * Method used in AddAtomLabels to adds the custom atom label.
 */
void OBDepictExt::addCustomAtomLabel
(
    OBAtom *atom,
    const vector3& atom_pos,
    OBPainter *painter
)
{
  // OBFontMetrics metrics = painter->GetFontMetrics("H");
  int fontsize = GetFontSize();

  if (atom->HasData("CustomLabelText"))
  {
    // Get custom color
    if (atom->HasData("CustomLabelColor"))
    {
      std::string clblcol = atom->GetData("CustomLabelColor")->GetValue();
      std::vector<std::string> clblcols;
      boost::algorithm::split
      (
          clblcols,
          clblcol,
          boost::algorithm::is_any_of(",")
      );
      if (clblcols.size() == 3)
      {
        double red = atof(clblcols[0].c_str());
        double green = atof(clblcols[1].c_str());
        double blue = atof(clblcols[2].c_str());
        painter->SetPenColor(OBColor(red, green, blue));
        painter->SetFillColor(OBColor(red, green, blue));
      }
      else
        std::cerr << "Invalid CustomLabelColor (default setted)." << std::endl;
    } // if CustomLabelColor

    // Draw custom label
    painter->SetFontSize(floor(fontsize * 0.4)); // smaller text
    //    painter->DrawText
    //    (
    //        atom_pos.x() - 1.7 * metrics.width,
    //        atom_pos.y() + 1.5 * metrics.height,
    //        atom->GetData("CustomLabelText")->GetValue(),
    //        0.5
    //    );
    painter->DrawText
    (
        atom_pos.x() - 0.4 * fontsize,
        atom_pos.y() - 0.4 * fontsize,
        atom->GetData("CustomLabelText")->GetValue(),
        0.5
    );

    // painter->SetFontSize(metrics.fontSize); // restore

  } // if CustomLabelText

  // Draw custom sub label
  if (atom->HasData("CustomSubLabelText"))
  {
    painter->SetPenColor(OBColor("black"));
    painter->SetFontSize(floor(fontsize * 0.3)); // more smaller
    //      painter->DrawText
    //      (
    //          atom_pos.x() - 1.5 * metrics.width,
    //          atom_pos.y() + 1.8 * metrics.height,
    //          atom->GetData("CustomSubLabelText")->GetValue(),
    //          0.1
    //      );
    painter->DrawText
    (
        atom_pos.x() - 0.2 * fontsize,
        atom_pos.y() + 0.8 * fontsize,
        atom->GetData("CustomSubLabelText")->GetValue(),
        0.1
    );
  } // if CustomSubLabelText

  return;
} // method addCustomAtomLabel

// ========================
// CLASS OBDEPICTEXTPRIVATE
// ========================

/**
 * Constructor
 */
OBDepictExtPrivate::OBDepictExtPrivate() :
  mol(0),
  painter(0),
  bondLength(40.0),
  penWidth(2.0),
  bondSpacing(6.0),
  bondWidth(8.0),
  fontSize(16),
  subscriptSize(13),
  aliasMode(false),
  bondColor("black"),
  options(0)
{ }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method DrawSimpleBond
 */
void OBDepictExtPrivate::DrawSimpleBond(OBAtom *beginAtom, OBAtom *endAtom, int order)
{
    vector3 begin = beginAtom->GetVector();
    vector3 end = endAtom->GetVector();
    vector3 vb = end - begin;

    vb.normalize();

    if (HasLabel(beginAtom))
      begin += 13. * vb; // Length is normally 40
    if (HasLabel(endAtom))
      end -= 13. * vb;

    if (order == 1) {
      painter->DrawLine(begin.x(), begin.y(), end.x(), end.y());
    } else if (order == 2) {
      vector3 orthogonalLine = cross(end - begin, VZ).normalize();

      bool useAsymmetricDouble = options & OBDepictExt::asymmetricDoubleBond;
      if (HasLabel(beginAtom) && HasLabel(endAtom))
        useAsymmetricDouble = false;
      if (HasLabel(beginAtom) && endAtom->GetValence() == 3)
        useAsymmetricDouble = false;
      if (HasLabel(endAtom) && beginAtom->GetValence() == 3)
        useAsymmetricDouble = false;


      if (!useAsymmetricDouble) {
        // style1
        //
        // -----------
        // -----------
        vector3 offset = orthogonalLine * 0.5 * bondSpacing;
        painter->DrawLine(begin.x() + offset.x(), begin.y() + offset.y(),
                          end.x() + offset.x(), end.y() + offset.y());
        painter->DrawLine(begin.x() - offset.x(), begin.y() - offset.y(),
                          end.x() - offset.x(), end.y() - offset.y());
      } else {
        // style2
        //
        //   -------
        // -----------
        vector3 offset1 = orthogonalLine * /*0.5 * */ bondSpacing;
        vector3 offset2 = vb * /*0.5 * */ bondSpacing;
        vector3 offset3 = - vb * /*0.5 * */ bondSpacing;

        if (HasLabel(beginAtom))
          offset2 = VZero;
        if (HasLabel(endAtom))
          offset3 = VZero;

        painter->DrawLine(begin.x(), begin.y(), end.x(), end.y());
        painter->DrawLine(begin.x() + offset1.x() + offset2.x(),
                          begin.y() + offset1.y() + offset2.y(),
                          end.x() + offset1.x() + offset3.x(),
                          end.y() + offset1.y() + offset3.y());
      }
    } else if (order == 3) {
      vector3 orthogonalLine = cross(end - begin, VZ).normalize();
      vector3 offset = orthogonalLine * 0.7 * bondSpacing;
      painter->DrawLine(begin.x(), begin.y(), end.x(), end.y());
      painter->DrawLine(begin.x() + offset.x(), begin.y() + offset.y(),
                        end.x() + offset.x(), end.y() + offset.y());
      painter->DrawLine(begin.x() - offset.x(), begin.y() - offset.y(),
                        end.x() - offset.x(), end.y() - offset.y());
    }

    return;
  } // method DrawSimpleBond

/**
 * Method DrawWedge
 */
void OBDepictExtPrivate::DrawWedge(OBAtom *beginAtom, OBAtom *endAtom)
{
    vector3 begin = beginAtom->GetVector();
    vector3 end = endAtom->GetVector();
    vector3 vb = end - begin;

    if (HasLabel(beginAtom))
      begin += 0.33 * vb;
    if (HasLabel(endAtom))
      end -= 0.33 * vb;

    vector3 orthogonalLine = cross(vb, VZ);
    orthogonalLine.normalize();
    orthogonalLine *= 0.5 * bondWidth;
    std::vector<std::pair<double,double> > points;

    points.push_back(std::pair<double,double>(begin.x(), begin.y()));
    points.push_back(std::pair<double,double>(end.x() + orthogonalLine.x(),
                                              end.y() + orthogonalLine.y()));
    points.push_back(std::pair<double,double>(end.x() - orthogonalLine.x(),
                                              end.y() - orthogonalLine.y()));
    painter->DrawPolygon(points);
    return;
  } // method DrawWedge

/**
 * Method DrawHash
 */
void OBDepictExtPrivate::DrawHash(OBAtom *beginAtom, OBAtom *endAtom)
{
    vector3 begin = beginAtom->GetVector();
    vector3 end = endAtom->GetVector();
    vector3 vb = end - begin;

    if (HasLabel(beginAtom))
      begin += 0.33 * vb;
    if (HasLabel(endAtom))
      end -= 0.33 * vb;

    vb = end - begin; // Resize the extents of the vb vector

    vector3 orthogonalLine = cross(vb, VZ);
    orthogonalLine.normalize();
    orthogonalLine *= 0.5 * bondWidth;

    double lines[6] = { 0.20, 0.36, 0.52, 0.68, 0.84, 1.0 };

    for (int k = 0; k < 6; ++k) {
      double w = lines[k];
      painter->DrawLine(begin.x() + vb.x() * w + orthogonalLine.x() * w,
                        begin.y() + vb.y() * w + orthogonalLine.y() * w,
                        begin.x() + vb.x() * w - orthogonalLine.x() * w,
                        begin.y() + vb.y() * w - orthogonalLine.y() * w);
    }
    return;
  } // method DrawHash

/**
 * Method DrawWobblyBond
 */
void OBDepictExtPrivate::DrawWobblyBond(OBAtom *beginAtom, OBAtom *endAtom)
{
  vector3 begin = beginAtom->GetVector();
  vector3 end = endAtom->GetVector();
  vector3 vb = end - begin;

  if (HasLabel(beginAtom))
    begin += 0.33 * vb;
  if (HasLabel(endAtom))
    end -= 0.33 * vb;

  vb = end - begin; // Resize the extents of the vb vector

  vector3 orthogonalLine = cross(vb, VZ);
  orthogonalLine.normalize();
  orthogonalLine *= 0.5 * bondWidth;

  double lines[6] = { 0.20, 0.36, 0.52, 0.68, 0.84, 1.0 };

  // This code is adapted from DrawWedge():
  // What we do is just join up the opposite ends of each of the wedge strokes
  // to create a zig-zag bond

  double oldx, oldy, newx, newy;
  oldx = begin.x();
  oldy = begin.y();
  int sign = 1;
  for (int k = 0; k < 6; ++k)
  {
    double w = lines[k];
    newx = begin.x() + vb.x() * w + sign * orthogonalLine.x() * w;
    newy = begin.y() + vb.y() * w + sign * orthogonalLine.y() * w;
    painter->DrawLine(oldx, oldy, newx, newy);
    oldx = newx;
    oldy = newy;
    sign =- sign;
  }

  return;
}  // method DrawWobblyBond

/**
 * Method DrawRingBond
 */
void OBDepictExtPrivate::DrawRingBond
(
    OBAtom *beginAtom,
    OBAtom *endAtom,
    const vector3 &center,
    int order
)
{
  if (order != 2)
  {
    DrawSimpleBond(beginAtom, endAtom, order);
    return;
  }

  vector3 begin = beginAtom->GetVector();
  vector3 end = endAtom->GetVector();

  vector3 vb = (end - begin).normalize();
  vector3 orthogonalLine = cross(vb, VZ)/*.normalize()*/;
  vector3 spacing = orthogonalLine * bondSpacing * 1.2;
  vector3 offset = vb * bondSpacing;
  if ((begin + spacing - center).length() > (begin - spacing - center).length())
    spacing *= -1.0;

  vector3 vbb = end - begin;
  if (HasLabel(beginAtom))
    begin += 0.33 * vbb;
  if (HasLabel(endAtom))
    end -= 0.33 * vbb;
  painter->DrawLine(begin.x(), begin.y(), end.x(), end.y());

  if (HasLabel(beginAtom))
    begin -= 0.10 * vbb;
  if (HasLabel(endAtom))
    end += 0.10 * vbb;
  painter->DrawLine(begin.x() + spacing.x() + offset.x(), begin.y() + spacing.y() + offset.y(),
                    end.x() + spacing.x() - offset.x(), end.y() + spacing.y() - offset.y());
  return;
} // method DrawRingBond

/**
 * Method DrawAtomLabel
 */
void OBDepictExtPrivate::DrawAtomLabel
(
    const std::string &label,
    OBDepictExt::LabelAlignment alignment,
    const vector3 &pos
)
{
  /*
  std::cout << "FontMetrics(" << label << "):" << std::endl;
  std::cout << "  ascent = " << metrics.ascent << std::endl;
  std::cout << "  descent = " << metrics.descent << std::endl;
  std::cout << "  width = " << metrics.width << std::endl;
  std::cout << "  height = " << metrics.height << std::endl;

  painter->SetFillColor(OBColor("white"));
  painter->SetPenColor(OBColor("white"));
  painter->DrawCircle(pos.x(), pos.y(), metrics.ascent / 2);
  painter->SetPenColor(OBColor("black"));
  */

  // compute the total width
  double totalWidth = 0.0;
  if ((alignment == OBDepictExt::RightAlignment) ||
      (alignment == OBDepictExt::LeftAlignment) ||
      (label.find("H") == std::string::npos))
  {
    for (unsigned int i = 0; i < label.size(); ++i) {
      if (!isalpha(label[i]))
      {
        painter->SetFontSize(subscriptSize);
        totalWidth += painter->GetFontMetrics(label.substr(i, 1)).width;
      }
      else
      {
        painter->SetFontSize(fontSize);
        totalWidth += painter->GetFontMetrics(label.substr(i, 1)).width;
      }
    }
  }
  else
  {
    painter->SetFontSize(fontSize);
    totalWidth = painter->GetFontMetrics(label.substr(0, label.find("H"))).width;
    double width = 0.0;
    for (unsigned int i = label.find("H"); i < label.size(); ++i)
    {
      if (!isalpha(label[i]))
      {
        painter->SetFontSize(subscriptSize);
        width += painter->GetFontMetrics(label.substr(i, 1)).width;
      }
      else
      {
        painter->SetFontSize(fontSize);
        width += painter->GetFontMetrics(label.substr(i, 1)).width;
      }
    }

    if (width > totalWidth)
      totalWidth = width;
  }

  painter->SetFontSize(fontSize);
  OBFontMetrics metrics = painter->GetFontMetrics(label);

  std::string str, subscript;
  // compute the horizontal starting position
  double xOffset, yOffset, yOffsetSubscript;
  switch (alignment)
  {
  case OBDepictExt::RightAlignment:
    xOffset = 0.5 * painter->GetFontMetrics(label.substr(0, 1)).width -
              painter->GetFontMetrics(label).width;
    break;
  case OBDepictExt::LeftAlignment:
    xOffset = - 0.5 * painter->GetFontMetrics(label.substr(label.size()-1, 1)).width;
    break;
  case OBDepictExt::UpAlignment:
  case OBDepictExt::DownAlignment:
    if (label.find("H") != std::string::npos)
      xOffset = - 0.5 * painter->GetFontMetrics(label.substr(0, label.find("H"))).width;
    else
      xOffset = - 0.5 * totalWidth;
  default:
    xOffset = - 0.5 * totalWidth;
    break;
  }

  // compute the vertical starting position
  yOffset = 0.5 * (metrics.ascent /*- metrics.descent*/);
  yOffsetSubscript = yOffset - metrics.descent;
  double xInitial = xOffset;

  for (unsigned int i = 0; i < label.size(); ++i)
  {
    if (label[i] == 'H') {
      if ((alignment == OBDepictExt::UpAlignment) || (alignment == OBDepictExt::DownAlignment))
        if (!str.empty())
        {
          // write the current string
          painter->SetFontSize(fontSize);
          painter->DrawText(pos.x() + xOffset, pos.y() + yOffset, str);
          if (alignment == OBDepictExt::DownAlignment)
          {
            yOffset += metrics.fontSize;
            yOffsetSubscript += metrics.fontSize;
          } else {
            yOffset -= metrics.fontSize;
            yOffsetSubscript -= metrics.fontSize;
          }
          xOffset = xInitial;
          str.clear();
        }
    }

    if (!isalpha(label[i])) {
      if (!str.empty())
      {
        // write the current string
        painter->SetFontSize(fontSize);
        OBFontMetrics metrics = painter->GetFontMetrics(str);
        painter->DrawText(pos.x() + xOffset, pos.y() + yOffset, str);
        xOffset += metrics.width;
        str.clear();
      }

      subscript += label.substr(i, 1);
    } else {
      if (!subscript.empty())
      {
        // write the current subscript
        painter->SetFontSize(subscriptSize);
        OBFontMetrics metrics = painter->GetFontMetrics(subscript);
        painter->DrawText(pos.x() + xOffset, pos.y() + yOffsetSubscript, subscript);
        xOffset += metrics.width;
        subscript.clear();
      }

      str += label.substr(i, 1);
    }
  }
  if (!str.empty())
  {
    painter->SetFontSize(fontSize);
    OBFontMetrics metrics = painter->GetFontMetrics(str);
    (void)metrics;
    painter->DrawText(pos.x() + xOffset, pos.y() + yOffset, str);
  }
  if (!subscript.empty())
  {
    painter->SetFontSize(subscriptSize);
    OBFontMetrics metrics = painter->GetFontMetrics(subscript);
    (void)metrics;
    double yOffset = ispunct(subscript[subscript.size()-1]) || ispunct(subscript[0]) || ispunct(subscript[1])
      ? -yOffsetSubscript : yOffsetSubscript;
    painter->DrawText(pos.x() + xOffset, pos.y() + yOffset, subscript);
  }

  return;
} // method DrawAtomLabel

/**
 * Method HasLabel
 */
bool OBDepictExtPrivate::HasLabel(OBAtom *atom)
{
  if (!atom->IsCarbon())
    return true;
  if ((options & OBDepictExt::drawAllC) ||
      ((options & OBDepictExt::drawTermC) && (atom->GetValence() == 1)))
    return true;
  return false;
} // method HasLabel

/**
 * Method SetWedgeAndHash
 */
void OBDepictExtPrivate::SetWedgeAndHash(OBMol* mol)
{
  // Remove any existing wedge and hash bonds
  FOR_BONDS_OF_MOL(b,mol)
  {
    b->UnsetWedge();
    b->UnsetHash();
  }

  std::map<OBBond*, enum OBStereo::BondDirection> updown;
  std::map<OBBond*, OBStereo::Ref> from;
  std::map<OBBond*, OBStereo::Ref>::const_iterator from_cit;
  TetStereoToWedgeHash(*mol, updown, from);

  for(from_cit=from.begin();from_cit!=from.end();++from_cit)
  {
    OBBond* pbond = from_cit->first;
    if(updown[pbond]==OBStereo::UpBond)
      pbond->SetHash();
    else if(updown[pbond]==OBStereo::DownBond)
      pbond->SetWedge();
  }
  return;
} // method SetWedgeAndHash

} // namespace OpenBabel

#endif // OB_DEPICTEXT_IMPL_H
