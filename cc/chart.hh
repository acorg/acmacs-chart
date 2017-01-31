#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "acmacs-base/virus-name.hh"
#include "acmacs-base/string.hh"
#include "acmacs-base/range.hh"
#include "acmacs-draw/color.hh"

#include "layout.hh"
#include "chart-plot-spec.hh"

namespace hidb
{
    class HiDb;
    class Antigen;
    class Serum;
    template <typename AS> class AntigenSerumData;
    class AntigenRefs;
}

// ----------------------------------------------------------------------

class Annotations : public std::vector<std::string>
{
 public:
    inline Annotations() = default;

    inline bool has(std::string anno) const { return std::find(begin(), end(), anno) != end(); }
    inline bool distinct() const { return has("DISTINCT"); }

    inline void sort() { std::sort(begin(), end()); }
    inline void sort() const { const_cast<Annotations*>(this)->sort(); }

    inline std::string join() const { return string::join(" ", begin(), end()); }

      // note annotations has to be sorted (regardless of const) to compare
    inline bool operator == (const Annotations& aNother) const
        {
            bool equal = size() == aNother.size();
            if (equal) {
                sort();
                aNother.sort();
                equal = std::mismatch(begin(), end(), aNother.begin()).first == end();
            }
            return equal;
        }

}; // class Annotations

// // ----------------------------------------------------------------------

class AntigenSerum
{
 public:
    inline AntigenSerum() = default;
    inline AntigenSerum(const AntigenSerum&) = default;
    inline AntigenSerum(AntigenSerum&&) = default;
    virtual ~AntigenSerum();

    inline AntigenSerum& operator=(const AntigenSerum&) = default;

    virtual std::string full_name() const = 0;
    virtual std::string full_name_without_passage() const = 0;

    inline const std::string name() const { return mName; }
    inline void name(const char* str, size_t length) { mName.assign(str, length); }
    inline const std::string lineage() const { return mLineage; }
    inline void lineage(const char* str, size_t length) { mLineage.assign(str, length); }
    inline const std::string passage() const { return mPassage; }
    inline void passage(const char* str, size_t length) { mPassage.assign(str, length); }
    inline bool has_passage() const { return !mPassage.empty(); }
    std::string passage_without_date() const;
    inline const std::string reassortant() const { return mReassortant; }
    inline void reassortant(const char* str, size_t length) { mReassortant.assign(str, length); }
    bool is_egg() const;
    inline bool is_reassortant() const { return !mReassortant.empty(); }
    inline bool distinct() const { return mAnnotations.distinct(); }
    inline const Annotations& annotations() const { return mAnnotations; }
    inline Annotations& annotations() { return mAnnotations; }
    inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
    inline const std::string semantic() const { return mSemanticAttributes; }
    inline void semantic(const char* str, size_t length) { mSemanticAttributes.assign(str, length); }

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

 private:
//       // Chart& mChart;
    std::string mName; // "N" "[VIRUS_TYPE/][HOST/]LOCATION/ISOLATION/YEAR" or "CDC_ABBR NAME" or "NAME"
    std::string mLineage; // "L"
    std::string mPassage; // "P"
    std::string mReassortant; // "R"
    Annotations mAnnotations; // "a"
    std::string mSemanticAttributes;       // string of single letter semantic boolean attributes: R - reference, V - current vaccine, v - previous vaccine, S - vaccine surrogate

}; // class AntigenSerum

// // ----------------------------------------------------------------------

class Serum;

class Antigen : public AntigenSerum
{
 public:
    inline Antigen() = default;
      // inline Antigen(Chart& aChart) : AntigenSerum(aChart) {}
    inline Antigen(const Antigen&) = default;
    inline Antigen(Antigen&&) = default;
    virtual inline std::string full_name() const { return string::join({name(), reassortant(), annotations().join(), passage()}); }
    virtual inline std::string full_name_without_passage() const { return string::join({name(), reassortant(), annotations().join()}); }

    // inline void name(const char* str, size_t length) { AntigenSerum::name(str, length); }

    inline const std::string date() const { return mDate; }
    inline void date(const char* str, size_t length) { mDate.assign(str, length); }
    inline bool reference() const { return has_semantic('R'); }
    inline const std::vector<std::string>& lab_id() const { return mLabId; }
    inline std::vector<std::string>& lab_id() { return mLabId; }
    inline bool has_lab_id(std::string aLabId) const { return std::find(mLabId.begin(), mLabId.end(), aLabId) != mLabId.end(); }
    inline const std::vector<std::string>& clades() const { return mClades; }
    inline std::vector<std::string>& clades() { return mClades; }

    const hidb::AntigenSerumData<hidb::Antigen>& find_in_hidb(const hidb::HiDb& aHiDb) const;

//     virtual std::string variant_id() const;

 private:
    std::string mDate; // "D"
    std::vector<std::string> mLabId; // "l"
    std::vector<std::string> mClades; // "c"

    const hidb::AntigenSerumData<hidb::Antigen>& find_in_suggestions(std::string aName, const hidb::AntigenRefs& aSuggestions) const;

}; // class Antigen

// ----------------------------------------------------------------------

class Serum : public AntigenSerum
{
 public:
    inline Serum() : mHomologous(-1) {}
      // inline Serum(Chart& aChart) : AntigenSerum(aChart), mHomologous(-1) {}
    inline Serum(const Serum&) = default;
    inline Serum(Serum&&) = default;
    virtual inline std::string full_name() const { return string::join({name(), reassortant(), serum_id(), annotations().join()}); } // serum_id comes before annotations, see hidb chart.cc Serum::variant_id
    virtual inline std::string full_name_without_passage() const { return full_name(); }

    inline const std::string serum_id() const { return mSerumId; }
    inline void serum_id(const char* str, size_t length) { mSerumId.assign(str, length); }
    inline const std::string serum_species() const { return mSerumSpecies; }
    inline void serum_species(const char* str, size_t length) { mSerumSpecies.assign(str, length); }

//     template <typename No> inline void set_homologous(No ag_no) { mHomologous = static_cast<decltype(mHomologous)>(ag_no); }
//     inline bool has_homologous() const { return mHomologous >= 0; }
    inline int homologous() const { return mHomologous; }
    inline void homologous(int aHomologous) { mHomologous = aHomologous; }
//     virtual bool is_egg() const;

    const hidb::AntigenSerumData<hidb::Serum>& find_in_hidb(const hidb::HiDb& aHiDb) const;

//  protected:
//     virtual AntigenSerumMatch match_passage(const AntigenSerum& aNother) const;

 private:
    std::string mSerumId; // "I"
    int mHomologous; // "h"
    std::string mSerumSpecies; // "s"

}; // class Serum

// ----------------------------------------------------------------------

// using AntigenRefs = std::vector<const Antigen*>;

class Antigens : public std::vector<Antigen>
{
 public:
    inline Antigens() {}

    void find_by_name(std::string aName, std::vector<size_t>& aAntigenIndices) const;

}; // class Antigens

// ----------------------------------------------------------------------

class Sera : public std::vector<Serum>
{
 public:
    inline Sera() {}

      // returns -1 if not found
    size_t find_by_name_for_exact_matching(std::string aFullName) const;

}; // class Sera

// ----------------------------------------------------------------------

class Projection
{
 public:
      // inline Serum(Chart& aChart) : AntigenSerum(aChart), mHomologous(-1) {}
    inline Projection() : mStress(-1), mDodgyTiterIsRegular(false), mStressDiffToStop(1e-10) {}

    inline void comment(const char* str, size_t length) { mComment.assign(str, length); }
    inline std::string comment() const { return mComment; }

    inline Layout& layout() { return mLayout; }
    inline const Layout& layout() const { return mLayout; }

    inline void stress(double aStress) { mStress = aStress; }
    inline double stress() const { return mStress; }

    inline void minimum_column_basis(const char* str, size_t length) { mMinimumColumnBasis.assign(str, length); }
    inline std::string minimum_column_basis() const { return mMinimumColumnBasis; }

    inline std::vector<double>& column_bases() { return mColumnBases; }
    inline const std::vector<double>& column_bases() const { return mColumnBases; }

    inline Transformation& transformation() { return mTransformation; }
    inline const Transformation& transformation() const { return mTransformation; }

    inline std::vector<double>& gradient_multipliers() { return mGradientMultipliers; }
    inline const std::vector<double>& gradient_multipliers() const { return mGradientMultipliers; }

    inline std::vector<double>& titer_multipliers() { return mTiterMultipliers; }
    inline const std::vector<double>& titer_multipliers() const { return mTiterMultipliers; }

    inline void dodgy_titer_is_regular(bool aDodgyTiterIsRegular) { mDodgyTiterIsRegular = aDodgyTiterIsRegular; }
    inline bool dodgy_titer_is_regular() const { return mDodgyTiterIsRegular; }

    inline void stress_diff_to_stop(double aStressDiffToStop) { mStressDiffToStop = aStressDiffToStop; }
    inline double stress_diff_to_stop() const { return mStressDiffToStop; }

    inline std::vector<size_t>& unmovable() { return mUnmovable; }
    inline const std::vector<size_t>& unmovable() const { return mUnmovable; }

    inline std::vector<size_t>& disconnected() { return mDisconnected; }
    inline const std::vector<size_t>& disconnected() const { return mDisconnected; }

    inline std::vector<size_t>& unmovable_in_last_dimension() { return mUnmovableInLastDimension; }
    inline const std::vector<size_t>& unmovable_in_last_dimension() const { return mUnmovableInLastDimension; }

 private:
    std::string mComment;                           // "c"
    Layout mLayout;       // "l": [[]] layout, list of lists of doubles, if point is disconnected: emtpy list or ?[NaN, NaN]
      // size_t mNumberOfIterations;                // "i"
    double mStress;                                 // "s"
    std::string mMinimumColumnBasis;                // "m": "1280", "none" (default)
    std::vector<double> mColumnBases;               // "C"
    Transformation mTransformation;                 // "t": [1.0, 0.0, 0.0, 1.0]
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

    inline const std::string virus() const { return merge_text_fields(&ChartInfo::mVirus); }
    inline const std::string virus_type() const { return merge_text_fields(&ChartInfo::mVirusType); }
    inline const std::string assay() const { return merge_text_fields(&ChartInfo::mAssay); }
    const std::string date() const;
    inline const std::string lab() const { return merge_text_fields(&ChartInfo::mLab); }
    inline const std::string rbc() const { return merge_text_fields(&ChartInfo::mRbc); }
    inline const std::string name() const { return merge_text_fields(&ChartInfo::mName); }
    inline const std::string subset() const { return merge_text_fields(&ChartInfo::mSubset); }
    inline TableType type() const { return mType; }
    inline std::string type_as_string() const
        {
            switch(mType) {
              case Antigenic:
                  return "A";
              case Genetic:
                  return "G";
            }
            return "?";         // to keep gcc happy
        }

    inline void virus(const char* str, size_t length) { mVirus.assign(str, length); }
    inline void virus_type(const char* str, size_t length) { mVirusType.assign(str, length); }
    inline void assay(const char* str, size_t length) { mAssay.assign(str, length); }
    inline void date(const char* str, size_t length) { mDate.assign(str, length); }
    inline void lab(const char* str, size_t length) { mLab.assign(str, length); }
    inline void rbc(const char* str, size_t length) { mRbc.assign(str, length); }
    inline void name(const char* str, size_t length) { mName.assign(str, length); }
    inline void subset(const char* str, size_t length) { mSubset.assign(str, length); }
    inline void type(const char* str, size_t length) // reading from json
        {
            if (length != 1)
                throw std::runtime_error("ChartInfo: unrecognized table type: " + std::string(str, length));
            switch (*str) {
              case 'A':
                  mType = Antigenic;
                  break;
              case 'G':
                  mType = Genetic;
                  break;
              default:
                  throw std::runtime_error("ChartInfo: unrecognized table type: " + std::string(str, length));
            }
        }

    inline auto& sources() { return mSources; }
    inline const auto& sources() const { return mSources; }

 private:
    std::string mVirus;              // "v"
    std::string mVirusType;          // "V"
    std::string mAssay;              // "A"
    std::string mDate;               // "D"
    std::string mLab;                // "l"
    std::string mRbc;                // "r"
    std::string mName;               // "N"
    std::string mSubset;             // "s"
    TableType mType;                 // "T"
    std::vector<ChartInfo> mSources; // "S"

    std::string merge_text_fields(std::string ChartInfo::* aMember) const;

}; // class ChartInfo

// // ----------------------------------------------------------------------

class ChartTiters
{
 public:
    using List = std::vector<std::vector<std::string>>;
    using Dict = std::vector<std::vector<std::pair<std::string, std::string>>>;
    using Layers = std::vector<Dict>;

    inline ChartTiters() {}

//     inline const List& as_list() const
//         {
//             if (mList.empty()) {
//                 std::cerr << "Warning: sparse matrix of titers ignored" << std::endl;
//                   //throw std::runtime_error("ChartTiters::as_list");
//             }
//             return mList;
//         }

    inline List& list() { return mList; }
    inline Dict& dict() { return mDict; }
    inline Layers& layers() { return mLayers; }

    inline const List& list() const { return mList; }
    inline const Dict& dict() const { return mDict; }
    inline const Layers& layers() const { return mLayers; }

 private:
    List mList;                 // "l"
    Dict mDict;                 // "d"
    Layers mLayers;             // "L"

}; // class ChartTiters

// ----------------------------------------------------------------------

class Vaccines;

class Chart
{
 public:
    inline Chart() {}
    inline Chart(const Chart&) = default;
    inline Chart& operator=(const Chart&) = default;
    ~Chart();

    // inline std::string virus_type() const { return mInfo.virus_type(); }

    inline size_t number_of_antigens() const { return mAntigens.size(); }
    inline size_t number_of_sera() const { return mSera.size(); }
    inline size_t number_of_points() const { return number_of_antigens() + number_of_sera(); }
    std::string lineage() const;

    inline const ChartInfo& chart_info() const { return mInfo; }
    inline ChartInfo& chart_info() { return mInfo; }

    inline const Antigens& antigens() const { return mAntigens; }
    inline Antigens& antigens() { return mAntigens; }
    inline Antigen& antigen(size_t ag_no) { return mAntigens[ag_no]; }

    inline const Sera& sera() const { return mSera; }
    inline Sera& sera() { return mSera; }
    inline Serum& serum(size_t sr_no) { return mSera[sr_no]; }

    inline const ChartTiters& titers() const { return mTiters; }
    inline ChartTiters& titers() { return mTiters; }

    inline const auto& column_bases() const { return mColumnBases; }
    inline auto& column_bases() { return mColumnBases; }

    inline std::vector<Projection>& projections() { return mProjections; }
    inline const std::vector<Projection>& projections() const { return mProjections; }
    inline Projection& projection(size_t aProjectionNo) { return mProjections[aProjectionNo]; }

    inline const ChartPlotSpec& plot_spec() const { return mPlotSpec; }
    inline ChartPlotSpec& plot_spec() { return mPlotSpec; }

    Vaccines* vaccines(std::string aName, const hidb::HiDb& aHiDb) const;

    inline IndexGenerator antigen_indices() const { return {number_of_antigens(), [](size_t) { return true; } }; }
    inline IndexGenerator reference_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].reference(); } }; }
    inline IndexGenerator test_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return !mAntigens[index].reference(); } }; }
    inline IndexGenerator egg_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].is_egg(); } }; }
    inline IndexGenerator reassortant_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].is_reassortant(); } }; }
    inline IndexGenerator serum_indices() const { return {number_of_antigens(), number_of_points(), [](size_t) { return true; } }; }

    // void find_homologous_antigen_for_sera();
    // inline void find_homologous_antigen_for_sera_const() const { const_cast<Chart*>(this)->find_homologous_antigen_for_sera(); }

    // inline bool operator < (const Chart& aNother) const { return table_id() < aNother.table_id(); }

 private:
    ChartInfo mInfo;                       // "i"
    Antigens mAntigens;                    // "a"
    Sera mSera;                            // "s"
    ChartTiters mTiters;                   // "t"
    std::vector <double> mColumnBases;     // "C"
    std::vector<Projection> mProjections;  // "P"
    ChartPlotSpec mPlotSpec;               // "p"

}; // class Chart

// ----------------------------------------------------------------------

class Vaccines
{
 public:
    class HomologousSerum
    {
     public:
        inline HomologousSerum(size_t aSerumIndex, const Serum* aSerum, const hidb::AntigenSerumData<hidb::Serum>* aSerumData, std::string aMostRecentTableDate)
            : serum(aSerum), serum_index(aSerumIndex), serum_data(aSerumData), most_recent_table_date(aMostRecentTableDate) {}
        bool operator < (const HomologousSerum& a) const;
        size_t number_of_tables() const;

        const Serum* serum;
        size_t serum_index;
        const hidb::AntigenSerumData<hidb::Serum>* serum_data;
        std::string most_recent_table_date;
    };

    class Entry
    {
     public:
        inline Entry(size_t aAntigenIndex, const Antigen* aAntigen, const hidb::AntigenSerumData<hidb::Antigen>* aAntigenData, std::vector<HomologousSerum>&& aSera, std::string aMostRecentTableDate)
            : antigen(aAntigen), antigen_index(aAntigenIndex), antigen_data(aAntigenData), homologous_sera(aSera), most_recent_table_date(aMostRecentTableDate)
            { std::sort(homologous_sera.begin(), homologous_sera.end()); }
        bool operator < (const Entry& a) const;

        const Antigen* antigen;
        size_t antigen_index;
        const hidb::AntigenSerumData<hidb::Antigen>* antigen_data;
        std::vector<HomologousSerum> homologous_sera; // sorted by number of tables and the most recent table
        std::string most_recent_table_date;
    };

    inline Vaccines() {}

    inline const Entry* egg() const { return mEgg.empty() ? nullptr : &mEgg.front(); }
    inline const Entry* cell() const { return mCell.empty() ? nullptr : &mCell.front(); }
    inline const Entry* reassortant() const { return mReassortant.empty() ? nullptr : &mReassortant.front(); }

    std::string report() const;

 private:
    std::vector<Entry> mEgg;
    std::vector<Entry> mCell;
    std::vector<Entry> mReassortant;

    friend class Chart;

    void add(size_t aAntigenIndex, const Antigen& aAntigen, const hidb::AntigenSerumData<hidb::Antigen>* aAntigenData, std::vector<HomologousSerum>&& aSera, std::string aMostRecentTableDate);
    void sort();

}; // class Vaccines

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
