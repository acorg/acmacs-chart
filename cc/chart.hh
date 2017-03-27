#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
// #include <algorithm>

#include "acmacs-base/virus-name.hh"
#include "acmacs-base/string.hh"
#include "acmacs-base/range.hh"
#include "acmacs-base/passage.hh"

#include "acmacs-chart/antigen-serum-match.hh"
#include "acmacs-chart/layout.hh"
#include "acmacs-chart/chart-plot-spec.hh"

class LocDb;

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
    virtual ~AntigenSerum();

    inline AntigenSerum& operator=(const AntigenSerum&) = default;

    virtual std::string full_name() const = 0;
    virtual std::string full_name_without_passage() const = 0;
    virtual std::string abbreviated_name(const LocDb& aLocDb) const = 0;

    inline const std::string name() const { return mName; }
    inline std::string& name() { return mName; }
    inline void name(const char* str, size_t length) { mName.assign(str, length); }
    inline const std::string lineage() const { return mLineage; }
    inline std::string& lineage() { return mLineage; }
    inline void lineage(const char* str, size_t length) { mLineage.assign(str, length); }
    inline const std::string passage() const { return mPassage; }
    inline std::string& passage() { return mPassage; }
    inline void passage(const char* str, size_t length) { mPassage.assign(str, length); }
    inline bool has_passage() const { return !mPassage.empty(); }
    inline std::string passage_without_date() const { return passage::without_date(mPassage); }
    inline const std::string reassortant() const { return mReassortant; }
    inline std::string& reassortant() { return mReassortant; }
    inline void reassortant(const char* str, size_t length) { mReassortant.assign(str, length); }
    inline bool is_egg() const { return passage::is_egg(mPassage) || is_reassortant(); } // reassortant is always egg (2016-10-21)
    inline bool is_reassortant() const { return !mReassortant.empty(); }
    inline bool distinct() const { return mAnnotations.distinct(); }
    inline const Annotations& annotations() const { return mAnnotations; }
    inline Annotations& annotations() { return mAnnotations; }
    inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
    inline const std::string semantic() const { return mSemanticAttributes; }
    inline void semantic(const char* str, size_t length) { mSemanticAttributes.assign(str, length); }
    std::string passage_type() const { return is_egg() ? "egg" : "cell"; }
    std::string name_abbreviated(const LocDb& aLocDb) const;
    std::string location_abbreviated(const LocDb& aLocDb) const;

    virtual AntigenSerumMatch match(const AntigenSerum& aNother) const;

//     virtual std::string variant_id() const = 0;

//       // returned cdc abbreviation starts with #
//     inline std::string location() const { return virus_name::location(mName); }
//     inline std::string year() const { return virus_name::year(mName); }

//     inline bool operator == (const AntigenSerum& aNother) const { return name() == aNother.name() && variant_id() == aNother.variant_id(); }
//     inline bool operator < (const AntigenSerum& aNother) const { return name() == aNother.name() ? variant_id() < aNother.variant_id() : name() < aNother.name(); }

 protected:
    virtual AntigenSerumMatch match_passage(const AntigenSerum& aNother) const;

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
    virtual inline std::string full_name() const { return string::join({name(), reassortant(), annotations().join(), passage()}); }
    virtual inline std::string full_name_without_passage() const { return string::join({name(), reassortant(), annotations().join()}); }
    virtual inline std::string full_name_for_seqdb_matching() const { return string::join({name(), reassortant(), passage(), annotations().join()}); } // annotations may part of the passage in seqdb (NIMR ISOLATE 1)
    virtual inline std::string abbreviated_name(const LocDb& aLocDb) const { return string::join({name_abbreviated(aLocDb), reassortant(), annotations().join()}); }
    virtual inline std::string abbreviated_name_with_passage_type(const LocDb& aLocDb) const { return string::join("-", {name_abbreviated(aLocDb), reassortant(), annotations().join(), passage_type()}); }

    // inline void name(const char* str, size_t length) { AntigenSerum::name(str, length); }

    inline const std::string date() const { return mDate; }
    inline void date(const char* str, size_t length) { mDate.assign(str, length); }
    inline bool reference() const { return has_semantic('R'); }
    inline const std::vector<std::string>& lab_id() const { return mLabId; }
    inline std::vector<std::string>& lab_id() { return mLabId; }
    inline bool has_lab_id(std::string aLabId) const { return std::find(mLabId.begin(), mLabId.end(), aLabId) != mLabId.end(); }
    inline const std::vector<std::string>& clades() const { return mClades; }
    inline std::vector<std::string>& clades() { return mClades; }

    using AntigenSerum::match;
    virtual AntigenSerumMatch match(const Antigen& aAntigen) const;
    virtual AntigenSerumMatch match(const Serum& aSerum) const;

 private:
    std::string mDate; // "D"
    std::vector<std::string> mLabId; // "l"
    std::vector<std::string> mClades; // "c"

}; // class Antigen

// ----------------------------------------------------------------------

class Serum : public AntigenSerum
{
 public:
    inline Serum() = default;
    virtual inline std::string full_name() const { return string::join({name(), reassortant(), serum_id(), annotations().join()}); } // serum_id comes before annotations, see hidb chart.cc Serum::variant_id
    virtual inline std::string full_name_without_passage() const { return full_name(); }
    virtual inline std::string abbreviated_name(const LocDb& aLocDb) const { return string::join({name_abbreviated(aLocDb), reassortant(), annotations().join()}); }

    inline const std::string serum_id() const { return mSerumId; }
    inline std::string& serum_id() { return mSerumId; }
    inline void serum_id(const char* str, size_t length) { mSerumId.assign(str, length); }
    inline const std::string serum_species() const { return mSerumSpecies; }
    inline std::string& serum_species() { return mSerumSpecies; }
    inline void serum_species(const char* str, size_t length) { mSerumSpecies.assign(str, length); }

    inline void add_homologous(size_t ag_no)
        {
            auto found = std::find(mHomologous.begin(), mHomologous.end(), ag_no);
            if (found == mHomologous.end())
                mHomologous.push_back(ag_no);
        }

    template <typename No> inline void add_homologous(No ag_no) { add_homologous(static_cast<size_t>(ag_no)); }
    inline bool has_homologous() const { return !mHomologous.empty(); }
    inline const std::vector<size_t>& homologous() const { return mHomologous; }
    inline std::vector<size_t>& homologous() { return mHomologous; }

    using AntigenSerum::match;
    virtual AntigenSerumMatch match(const Serum& aSerum) const;
    virtual AntigenSerumMatch match(const Antigen& aAntigen) const;

 protected:
    virtual AntigenSerumMatch match_passage(const AntigenSerum& aNother) const;

 private:
    std::string mSerumId; // "I"
    std::vector<size_t> mHomologous; // "h"
    std::string mSerumSpecies; // "s"

}; // class Serum

// ----------------------------------------------------------------------

// using AntigenRefs = std::vector<const Antigen*>;

class Antigens : public std::vector<Antigen>
{
 public:
    using ContinentData = std::map<std::string, std::vector<size_t>>; // continent name to the list of antigen indices
    using CountryData = std::map<std::string, std::vector<size_t>>; // country name to the list of antigen indices
    using CladeData = std::map<std::string, std::vector<size_t>>; // clade name to the list of antigen indices

    inline Antigens() {}

    void find_by_name(std::string aName, std::vector<size_t>& aAntigenIndices) const;
    void find_by_name_matching(std::string aName, std::vector<size_t>& aAntigenIndices, string_match::score_t aScoreThreshold = 0, bool aVerbose = false) const;
    void continents(ContinentData& aContinentData, const LocDb& aLocDb) const;
    void countries(CountryData& aCountries, const LocDb& aLocDb) const;
    void country(std::string aCountry, std::vector<size_t>& aAntigenIndices, const LocDb& aLocDb) const;
    void reference_indices(std::vector<size_t>& aAntigenIndices) const;
    void test_indices(std::vector<size_t>& aAntigenIndices) const;
    void date_range_indices(std::string first_date, std::string after_last_date, std::vector<size_t>& aAntigenIndices) const;
    void egg_indices(std::vector<size_t>& aAntigenIndices) const;
    void cell_indices(std::vector<size_t>& aAntigenIndices) const;
    void reassortant_indices(std::vector<size_t>& aAntigenIndices) const;

}; // class Antigens

// ----------------------------------------------------------------------

class Sera : public std::vector<Serum>
{
 public:
    inline Sera() {}

      // returns -1 if not found
    size_t find_by_name_for_exact_matching(std::string aFullName) const;
    void find_by_name_matching(std::string aName, std::vector<size_t>& aSeraIndices, string_match::score_t aScoreThreshold = 0, bool aVerbose = false) const;

}; // class Sera

// ----------------------------------------------------------------------

class MinimumColumnBasis : public std::string
{
 public:
    inline MinimumColumnBasis() : mCached(static_cast<size_t>(-1)) {}
    inline operator size_t() const { if (mCached == static_cast<size_t>(-1)) mCached = *this == "none" || *this == "auto" ? 0 : std::stoul(*this); return mCached; }

 private:
    mutable size_t mCached;

}; // class MinimumColumnBasis

// ----------------------------------------------------------------------

class Projection
{
 public:
    inline Projection() : mStress(-1), mDodgyTiterIsRegular(false), mStressDiffToStop(1e-10) {}

    inline void comment(const char* str, size_t length) { mComment.assign(str, length); }
    inline std::string comment() const { return mComment; }

    inline Layout& layout() { return mLayout; }
    inline const Layout& layout() const { return mLayout; }

    inline void stress(double aStress) { mStress = aStress; }
    inline double stress() const { return mStress; }

    inline void minimum_column_basis(const char* str, size_t length) { mMinimumColumnBasis.assign(str, length); }
    inline const MinimumColumnBasis& minimum_column_basis() const { return mMinimumColumnBasis; }

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

    inline size_t number_of_dimentions() const
        {
            for (const auto& point: mLayout) {
                if (!point.empty())
                    return point.size();
            }
            throw std::runtime_error("Internal: cannot find number_of_dimentions of projection");
        }

 private:
    std::string mComment;                           // "c"
    Layout mLayout;       // "l": [[]] layout, list of lists of doubles, if point is disconnected: emtpy list or ?[NaN, NaN]
      // size_t mNumberOfIterations;                // "i"
    double mStress;                                 // "s"
    MinimumColumnBasis mMinimumColumnBasis;         // "m": "1280", "none" (default)
    std::vector<double> mColumnBases;               // "C"
    Transformation mTransformation;                 // "t": [1.0, 0.0, 0.0, 1.0]
    std::vector<double> mGradientMultipliers;       // "g": [] double for each point
    std::vector<double> mTiterMultipliers;          // "f": [],  antigens_sera_titers_multipliers, double for each point
    bool mDodgyTiterIsRegular;                      // "d": false
    double mStressDiffToStop;                       // "e": 1e-10 - precise, 1e-5 - rough
    std::vector<size_t> mUnmovable;                 // "U": [] list of indices of unmovable points (antigen/serum attribute for stress evaluation)
    std::vector<size_t> mDisconnected;              // "D": [] list of indices of disconnected points (antigen/serum attribute for stress evaluation)
    std::vector<size_t> mUnmovableInLastDimension;  // "u": [] list of indices of points unmovable in the last dimension (antigen/serum attribute for stress evaluation)

}; // class Projection

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
    inline const std::string rbc() const { const std::string ass = assay(); return ass.empty() || ass == "HI" ? merge_text_fields(&ChartInfo::mRbc) : std::string(); }
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

    const std::string make_name() const;

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

    inline std::string& virus() { return mVirus; }
    inline std::string& virus_type() { return mVirusType; }
    inline std::string& assay() { return mAssay; }
    inline std::string& date() { return mDate; }
    inline std::string& lab() { return mLab; }
    inline std::string& rbc() { return mRbc; }
    inline std::string& name() { return mName; }
    inline std::string& subset() { return mSubset; }

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

// ----------------------------------------------------------------------

class Titer : public std::string
{
 public:
    inline Titer() = default;

    inline bool is_regular() const { return !empty() && std::isdigit(front()); }
    inline bool is_more_than() const { return !empty() && front() == '>'; }
    inline bool is_less_than() const { return !empty() && front() == '<'; }
    inline bool is_dont_care() const { return empty() || front() == '*'; }

    inline size_t value() const
        {
            if (empty() || front() == '*')
                return 0;
            else if (!std::isdigit(front()))
                return std::stoul(substr(1));
            else
                return std::stoul(*this);
        }

    inline double similarity() const
        {
            return std::log2(value() / 10.0);
        }

 private:
    friend class ChartTiters;
    inline Titer(std::string source) : std::string(source) {}

}; // class Titer

class ChartTiters
{
 public:
    using List = std::vector<std::vector<std::string>>;
    using Dict = std::vector<std::vector<std::pair<std::string, std::string>>>;
    using Layers = std::vector<Dict>;

    inline ChartTiters() {}

    inline List& list() { return mList; }
    inline Dict& dict() { return mDict; }
    inline Layers& layers() { return mLayers; }

    inline const List& list() const { return mList; }
    inline const Dict& dict() const { return mDict; }
    inline const Layers& layers() const { return mLayers; }

    Titer get(size_t ag_no, size_t sr_no) const;

 private:
    List mList;                 // "l"
    Dict mDict;                 // "d"
    Layers mLayers;             // "L"

}; // class ChartTiters

// ----------------------------------------------------------------------

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
    const std::string make_name() const;

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
    double compute_column_basis(MinimumColumnBasis aMinimumColumnBasis, size_t aSerumNo) const;
    inline void compute_column_bases(MinimumColumnBasis aMinimumColumnBasis, std::vector<double>& aColumnBases) const
        {
            aColumnBases.resize(number_of_sera());
            for (size_t sr_no = 0; sr_no < number_of_sera(); ++sr_no)
                aColumnBases[sr_no] = compute_column_basis(aMinimumColumnBasis, sr_no);
        }
    inline void column_bases(MinimumColumnBasis aMinimumColumnBasis, std::vector<double>& aColumnBases) const
        {
            if (mColumnBases.empty()) {
                compute_column_bases(aMinimumColumnBasis, aColumnBases);
            }
            else {
                aColumnBases.resize(mColumnBases.size());
                std::copy(mColumnBases.begin(), mColumnBases.end(), aColumnBases.begin());
            }
        }
    inline double column_basis(MinimumColumnBasis aMinimumColumnBasis, size_t aSerumNo) const
        {
            if (mColumnBases.empty()) {
                return compute_column_basis(aMinimumColumnBasis, aSerumNo);
            }
            else {
                return mColumnBases.at(aSerumNo);
            }
        }
    inline void column_bases(size_t aProjectionNo, std::vector<double>& aColumnBases) const
        {
            const auto& p = projection(aProjectionNo);
            if (p.column_bases().empty()) {
                column_bases(p.minimum_column_basis(), aColumnBases);
            }
            else {
                aColumnBases.resize(p.column_bases().size());
                std::copy(p.column_bases().begin(), p.column_bases().end(), aColumnBases.begin());
            }
        }
    inline double column_basis(size_t aProjectionNo, size_t aSerumNo) const
        {
            double result = 0.0;
            const auto& p = projection(aProjectionNo);
            if (p.column_bases().empty()) {
                result = column_basis(p.minimum_column_basis(), aSerumNo);
            }
            else {
                result = p.column_bases().at(aSerumNo);
            }
            return result;
        }

    inline std::vector<Projection>& projections() { return mProjections; }
    inline const std::vector<Projection>& projections() const { return mProjections; }
    inline Projection& projection(size_t aProjectionNo) { return mProjections[aProjectionNo]; }
    inline const Projection& projection(size_t aProjectionNo) const { return mProjections[aProjectionNo]; }

    inline const ChartPlotSpec& plot_spec() const { return mPlotSpec; }
    inline ChartPlotSpec& plot_spec() { return mPlotSpec; }

    inline IndexGenerator antigen_indices() const { return {number_of_antigens(), [](size_t) { return true; } }; }
    inline IndexGenerator reference_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].reference(); } }; }
    inline IndexGenerator test_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return !mAntigens[index].reference(); } }; }
    inline IndexGenerator egg_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].is_egg(); } }; }
    inline IndexGenerator reassortant_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].is_reassortant(); } }; }
    inline IndexGenerator serum_indices() const { return {number_of_antigens(), number_of_points(), [](size_t) { return true; } }; }

    void find_homologous_antigen_for_sera();
    inline void find_homologous_antigen_for_sera_const() const { const_cast<Chart*>(this)->find_homologous_antigen_for_sera(); }

      // Negative radius means calculation failed (e.g. no homologous titer)
    double serum_circle_radius(size_t aAntigenNo, size_t aSerumNo, size_t aProjectionNo, bool aVerbose = false) const;
      // aWithin4Fold: indices of antigens within 4fold from homologous titer
      // aOutside4Fold: indices of antigens with titers against aSerumNo outside 4fold distance from homologous titer
    void serum_coverage(size_t aAntigenNo, size_t aSerumNo, std::vector<size_t>& aWithin4Fold, std::vector<size_t>& aOutside4Fold) const;

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

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
