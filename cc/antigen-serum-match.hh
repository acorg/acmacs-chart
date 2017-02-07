#pragma once

#include <iostream>

// ----------------------------------------------------------------------

class AntigenSerumMatch
{
 public:
    enum Level : size_t { Perfect=0, SerumSpeciesMismatch=0x1, PassageMismatch=0x2, PassageWithoutDateMismatch=0x4, EggCellUnknown=0x8, EggCellMismatch=0x10,
                          Mismatch=0x20, AnnotationMismatch=0x40, ReassortantMismatch=0x80, SerumIdMismatch=0x100, NameMismatch=0x200 };

    inline bool operator < (AntigenSerumMatch m) const { return mLevel < m.mLevel; }
    inline bool operator < (Level l) const { return mLevel < l; }
    inline bool operator == (AntigenSerumMatch m) const { return mLevel == m.mLevel; }
    inline bool operator != (AntigenSerumMatch m) const { return ! operator==(m); }

    inline void add(Level toAdd) { mLevel += toAdd; }
    inline void add(AntigenSerumMatch toAdd) { mLevel += toAdd.mLevel; }
    inline bool mismatch() const { return mLevel > Mismatch; }
    inline bool name_match() const { return mLevel < NameMismatch; }
    inline bool reassortant_match() const { return mLevel < ReassortantMismatch; }
    inline bool homologous_match() const { return mLevel < EggCellMismatch; }
    inline bool perfect() const { return mLevel == Perfect; }

 private:
    size_t mLevel;
    inline AntigenSerumMatch() : mLevel(Perfect) {}

    friend class AntigenSerum;
    friend class Serum;

    friend inline std::ostream& operator << (std::ostream& out, const AntigenSerumMatch& m) { return out << "0x" << std::hex << m.mLevel << std::dec; }

}; // class AntigenSerumMatch

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
