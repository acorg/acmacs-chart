#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "acmacs-base/virus-name.hh"

// ----------------------------------------------------------------------

// class Annotations : public std::vector<std::string>
// {
//  public:
//     inline Annotations() = default;

//     inline bool has(std::string anno) const { return std::find(begin(), end(), anno) != end(); }
//     inline bool distinct() const { return has("DISTINCT"); }

//     inline void sort() { std::sort(begin(), end()); }
//     inline void sort() const { const_cast<Annotations*>(this)->sort(); }

//       // note annotations has to be sorted (regardless of const) to compare
//     inline bool operator == (const Annotations& aNother) const
//         {
//             bool equal = size() == aNother.size();
//             if (equal) {
//                 sort();
//                 aNother.sort();
//                 equal = std::mismatch(begin(), end(), aNother.begin()).first == end();
//             }
//             return equal;
//         }

// };

// // ----------------------------------------------------------------------

class AntigenSerum
{
 public:
    virtual ~AntigenSerum();
    inline AntigenSerum& operator=(const AntigenSerum&) = default;

    // virtual std::string full_name() const = 0;

//     inline const std::string name() const { return mName; }
//     inline std::string& name() { return mName; }
//     inline const std::string lineage() const { return mLineage; }
//     inline std::string& lineage() { return mLineage; }
//     inline const std::string passage() const { return mPassage; }
//     inline std::string& passage() { return mPassage; }
//     inline bool has_passage() const { return !mPassage.empty(); }
//     std::string passage_without_date() const;
//     inline const std::string reassortant() const { return mReassortant; }
//     inline std::string& reassortant() { return mReassortant; }
//     virtual bool is_egg() const;
//     inline bool is_reassortant() const { return !mReassortant.empty(); }
//     inline bool distinct() const { return mAnnotations.distinct(); }
//     inline const Annotations& annotations() const { return mAnnotations; }
//     inline Annotations& annotations() { return mAnnotations; }
//     inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
//     inline const std::string semantic() const { return mSemanticAttributes; }
//     inline std::string& semantic() { return mSemanticAttributes; }

//     virtual std::string variant_id() const = 0;
//     virtual AntigenSerumMatch match(const AntigenSerum& aNother) const;

//       // returned cdc abbreviation starts with #
//     inline std::string location() const { return virus_name::location(mName); }
//     inline std::string year() const { return virus_name::year(mName); }

//     inline bool operator == (const AntigenSerum& aNother) const { return name() == aNother.name() && variant_id() == aNother.variant_id(); }
//     inline bool operator < (const AntigenSerum& aNother) const { return name() == aNother.name() ? variant_id() < aNother.variant_id() : name() < aNother.name(); }

//  protected:
//     inline AntigenSerum() = default;
//     inline AntigenSerum(const AntigenSerum&) = default;
//       //inline AntigenSerum(Chart& aChart) : mChart(aChart) {}

//     virtual AntigenSerumMatch match_passage(const AntigenSerum& aNother) const;

//  private:
//       // Chart& mChart;
//     std::string mName; // "N" "[VIRUS_TYPE/][HOST/]LOCATION/ISOLATION/YEAR" or "CDC_ABBR NAME" or "NAME"
//     std::string mLineage; // "L"
//     std::string mPassage; // "P"
//     std::string mReassortant; // "R"
//     Annotations mAnnotations; // "a"
//     std::string mSemanticAttributes;       // string of single letter semantic boolean attributes: R - reference, V - current vaccine, v - previous vaccine, S - vaccine surrogate

}; // class AntigenSerum

// // ----------------------------------------------------------------------

class Serum;

class Antigen : public AntigenSerum
{
 public:
      // inline Antigen(Chart& aChart) : AntigenSerum(aChart) {}
    inline Antigen() = default;
//     virtual inline std::string full_name() const { const auto vi = variant_id(); std::string n = name(); if (!vi.empty()) { n.append(1, ' '); n.append(vi); } return n; }

//     inline const std::string date() const { return mDate; }
//     inline std::string& date() { return mDate; }
//     inline bool reference() const { return has_semantic('R'); }
//     inline const std::vector<std::string>& lab_id() const { return mLabId; }
//     inline std::vector<std::string>& lab_id() { return mLabId; }
//     inline bool has_lab_id(std::string aLabId) const { return std::find(mLabId.begin(), mLabId.end(), aLabId) != mLabId.end(); }
//     virtual std::string variant_id() const;
//     inline const std::vector<std::string>& clades() const { return mClades; }
//     inline std::vector<std::string>& clades() { return mClades; }

//     using AntigenSerum::match;
//     virtual AntigenSerumMatch match(const Serum& aNother) const;
//     virtual AntigenSerumMatch match(const Antigen& aNother) const;

//  private:
//     friend class ChartReaderEventHandler;
//     std::string mDate; // "D"
//     std::vector<std::string> mLabId; // "l"
//     std::vector<std::string> mClades; // "c"

}; // class Antigen

// ----------------------------------------------------------------------

class Serum : public AntigenSerum
{
 public:
      // inline Serum(Chart& aChart) : AntigenSerum(aChart), mHomologous(-1) {}
    inline Serum() : mHomologous(-1) {}
//     virtual std::string full_name() const;

//     inline const std::string serum_id() const { return mSerumId; }
//     inline std::string& serum_id() { return mSerumId; }
//     inline const std::string serum_species() const { return mSerumSpecies; }
//     inline std::string& serum_species() { return mSerumSpecies; }
//     virtual std::string variant_id() const;

//     template <typename No> inline void set_homologous(No ag_no) { mHomologous = static_cast<decltype(mHomologous)>(ag_no); }
//     inline bool has_homologous() const { return mHomologous >= 0; }
    inline int homologous() const { return mHomologous; }
    inline void homologous(int aHomologous) { mHomologous = aHomologous; }
//     virtual bool is_egg() const;

//     using AntigenSerum::match;
//     virtual AntigenSerumMatch match(const Serum& aNother) const;
//     virtual AntigenSerumMatch match(const Antigen& aNother) const;

//  protected:
//     virtual AntigenSerumMatch match_passage(const AntigenSerum& aNother) const;

 private:
//     friend class ChartReaderEventHandler;
//     std::string mSerumId; // "I"
    int mHomologous; // "h"
//     std::string mSerumSpecies; // "s"

}; // class Serum

// ----------------------------------------------------------------------

class Projection
{
 public:
      // inline Serum(Chart& aChart) : AntigenSerum(aChart), mHomologous(-1) {}
    inline Projection() : mStress(-1), mDodgyTiterIsRegular(false), mStressDiffToStop(1e-10) {}

    inline void comment(std::string aComment) { mComment = aComment; }
    inline std::string comment() const { return mComment; }

    inline std::vector<size_t>& disconnected() { return mDisconnected; }
    inline const std::vector<size_t>& disconnected() const { return mDisconnected; }

 private:
    std::string mComment;                           // "c"
    std::vector<std::vector<double>> mLayout;       // "l": [[]] layout, list of lists of doubles, if point is disconnected: emtpy list or ?[NaN, NaN]
      // size_t mNumberOfIterations;                // "i"
    double mStress;                                 // "s"
    std::string mMinimumColumnBasis;                // "m": "1280", "none" (default)
    std::vector<double> mColumnBases;               // "C"
    std::vector<double> mTransformation;            // "t": [1.0, 0.0, 0.0, 1.0]
    std::vector<double> mGradientMultipliers;       // "g": [] double for each point
    std::vector<double> mTiterMultipliers;          // "f": [],  antigens_sera_titers_multipliers, double for each point
    bool mDodgyTiterIsRegular;                      // "d": false
    double mStressDiffToStop;                       // "e": 1e-10 - precise, 1e-5 - rough
    std::vector<size_t> mUnmovable;                 // "U": [] list of indices of unmovable points (antigen/serum attribute for stress evaluation)
    std::vector<size_t> mDisconnected;              // "D": [] list of indices of disconnected points (antigen/serum attribute for stress evaluation)
    std::vector<size_t> mUnmovableInLastDimension;  // "u": [] list of indices of points unmovable in the last dimension (antigen/serum attribute for stress evaluation)

}; // class Serum

// ----------------------------------------------------------------------

class ChartInfo
{
 public:
    enum TableType {Antigenic, Genetic};
    inline ChartInfo() : mType(Antigenic) {}
//     std::string table_id(std::string lineage) const;

//     inline const std::string virus() const { return mVirus; }
//     inline const std::string virus_type() const { return mVirusType; }
//     inline const std::string assay() const { return mAssay; }
//     inline const std::string date() const { return mDate; }
//     inline const std::string lab() const { return mLab; }
//     inline const std::string rbc() const { return mRbc; }
//     inline const std::string name() const { return mName; }
//     inline const std::string subset() const { return mSubset; }
    inline TableType type() const { return mType; }

//     inline std::string& virus() { return mVirus; }
//     inline std::string& virus_type() { return mVirusType; }
//     inline std::string& assay() { return mAssay; }
//     inline std::string& date() { return mDate; }
//     inline std::string& lab() { return mLab; }
//     inline std::string& rbc() { return mRbc; }
//     inline std::string& name() { return mName; }
//     inline std::string& subset() { return mSubset; }
//     inline TableType& type() { return mType; }

//     inline auto& sources() { return mSources; }
//     inline const auto& sources() const { return mSources; }

 private:
//     std::string mVirus;     // "v"
//     std::string mVirusType;     // "V"
//     std::string mAssay;         // "A"
//     std::string mDate;          // "D"
//     std::string mLab;           // "l"
//     std::string mRbc;           // "r"
//     std::string mName;           // "N"
//     std::string mSubset;           // "s"
    TableType mType;             // "T"
//     std::vector<ChartInfo> mSources; // "S"

}; // class ChartInfo

// // ----------------------------------------------------------------------

class ChartTiters
{
 public:
    typedef std::vector<std::vector<std::string>> List;
    typedef std::vector<std::vector<std::pair<std::string, std::string>>> Dict;

    inline ChartTiters() {}

//     inline const List& as_list() const
//         {
//             if (mList.empty()) {
//                 std::cerr << "Warning: sparse matrix of titers ignored" << std::endl;
//                   //throw std::runtime_error("ChartTiters::as_list");
//             }
//             return mList;
//         }

//     inline List& list() { return mList; }
//     inline Dict& dict() { return mDict; }
//     inline auto& layers() { return mLayers; }

//  private:
//     List mList;
//     Dict mDict;
//     std::vector<Dict> mLayers;

}; // class ChartTiters

// ----------------------------------------------------------------------

class Chart
{
 public:
    inline Chart() {}
    inline Chart(const Chart&) = default;
    inline Chart& operator=(const Chart&) = default;
    ~Chart();

    inline void xxx(const char* str, size_t length) { std::cerr << "Chart::xxx: " << std::string(str, length) << std::endl; }
    // inline std::string virus_type() const { return mInfo.virus_type(); }

    inline size_t number_of_antigens() const { return mAntigens.size(); }
    inline size_t number_of_sera() const { return mSera.size(); }
    // inline std::string table_id() const { return mInfo.table_id(lineage()); }
    // std::string lineage() const;

    inline const ChartInfo& chart_info() const { return mInfo; }
    inline ChartInfo& chart_info() { return mInfo; }

    inline const std::vector<Antigen>& antigens() const { return mAntigens; }
    inline std::vector<Antigen>& antigens() { return mAntigens; }
    inline Antigen& antigen(size_t ag_no) { return mAntigens[ag_no]; }

    inline const std::vector<Serum>& sera() const { return mSera; }
    inline std::vector<Serum>& sera() { return mSera; }
    inline Serum& serum(size_t sr_no) { return mSera[sr_no]; }

    inline const ChartTiters& titers() const { return mTiters; }
    inline ChartTiters& titers() { return mTiters; }

    inline const auto& column_bases() const { return mColumnBases; }
    inline auto& column_bases() { return mColumnBases; }

    inline std::vector<Projection>& projections() { return mProjections; }
    inline const std::vector<Projection>& projections() const { return mProjections; }

    // void find_homologous_antigen_for_sera();
    // inline void find_homologous_antigen_for_sera_const() const { const_cast<Chart*>(this)->find_homologous_antigen_for_sera(); }

    // inline bool operator < (const Chart& aNother) const { return table_id() < aNother.table_id(); }

 private:
    ChartInfo mInfo;
    std::vector<Antigen> mAntigens;
    std::vector<Serum> mSera;
    ChartTiters mTiters;
    std::vector <double> mColumnBases;
    std::vector<Projection> mProjections;

}; // class Chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
