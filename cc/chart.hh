#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <optional>

#include "acmacs-base/throw.hh"
#include "acmacs-base/range.hh"
#include "acmacs-base/virus-name.hh"
#include "acmacs-base/passage.hh"
#include "locationdb/locdb.hh"

#include "acmacs-chart/layout.hh"
#include "acmacs-chart/chart-plot-spec.hh"
#include "acmacs-chart/chart-base.hh"

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

// ----------------------------------------------------------------------

class Serum;

class Antigen : public AntigenBase
{
 public:
    inline Antigen() = default;
    inline std::string full_name() const override { return string::join({name(), reassortant(), annotations().join(), passage()}); }
    inline std::string full_name_without_passage() const override { return string::join({name(), reassortant(), annotations().join()}); }
    virtual inline std::string full_name_for_seqdb_matching() const { return string::join({name(), reassortant(), passage(), annotations().join()}); } // annotations may part of the passage in seqdb (NIMR ISOLATE 1)
    inline std::string abbreviated_name() const override { return string::join({name_abbreviated(), reassortant(), annotations().join()}); }
    inline std::string abbreviated_name_with_passage_type() const override { return string::join("-", {name_abbreviated(), reassortant(), annotations().join(), passage_type()}); }
    virtual inline std::string abbreviated_location_with_passage_type() const { return string::join("-", {location_abbreviated(), passage_type()}); }

    inline const std::string name() const override { return mName; }
    inline std::string& name() { return mName; }
    inline void name(const char* str, size_t length) { mName.assign(str, length); }
    inline const std::string lineage() const override { return mLineage; }
    inline std::string& lineage() { return mLineage; }
    inline void lineage(const char* str, size_t length) { mLineage.assign(str, length); }
    inline const std::string passage() const override { return mPassage; }
    inline std::string& passage() { return mPassage; }
    inline void passage(const char* str, size_t length) { mPassage.assign(str, length); }
    inline bool has_passage() const override { return !mPassage.empty(); }
    inline std::string passage_without_date() const override { return passage::without_date(mPassage); }
    inline const std::string reassortant() const override { return mReassortant; }
    inline std::string& reassortant() { return mReassortant; }
    inline void reassortant(const char* str, size_t length) { mReassortant.assign(str, length); }
    inline bool is_egg() const override { return passage::is_egg(mPassage) && !is_reassortant(); }
    inline bool is_reassortant() const override { return !mReassortant.empty(); }
    inline bool distinct() const override { return mAnnotations.distinct(); }
    inline const Annotations& annotations() const { return mAnnotations; }
    inline Annotations& annotations() { return mAnnotations; }
    inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
    inline const std::string semantic() const { return mSemanticAttributes; }
    inline void semantic(const char* str, size_t length) { mSemanticAttributes.assign(str, length); }
      // inline std::string passage_type() const { return is_egg() ? "egg" : "cell"; }
    std::string name_abbreviated() const;
    std::string location_abbreviated() const;

      // inline void name(const char* str, size_t length) { AntigenSerum::name(str, length); }

    inline const std::string date() const { return mDate; }
    inline void date(const char* str, size_t length) { mDate.assign(str, length); }
    inline bool within_date_range(std::string first_date, std::string after_last_date) const { return !date().empty() && (first_date.empty() || date() >= first_date) && (after_last_date.empty() || date() < after_last_date); }

    inline bool reference() const override { return has_semantic('R'); }
    inline const std::vector<std::string>& lab_id() const { return mLabId; }
    inline std::vector<std::string>& lab_id() { return mLabId; }
    inline bool has_lab_id(std::string aLabId) const { return std::find(mLabId.begin(), mLabId.end(), aLabId) != mLabId.end(); }
    inline const std::vector<std::string>& clades() const { return mClades; }
    inline std::vector<std::string>& clades() { return mClades; }

    virtual AntigenSerumMatch match(const Antigen& aAntigen) const;
    virtual AntigenSerumMatch match(const Serum& aSerum) const;
    inline AntigenSerumMatch match(const AntigenSerumBase& aNother) const override { return match(static_cast<const Antigen&>(aNother)); }
    AntigenSerumMatch match_passage(const AntigenSerumBase& aNother) const override;

 private:
    std::string mName; // "N" "[VIRUS_TYPE/][HOST/]LOCATION/ISOLATION/YEAR" or "CDC_ABBR NAME" or "NAME"
    std::string mLineage; // "L"
    std::string mPassage; // "P"
    std::string mReassortant; // "R"
    Annotations mAnnotations; // "a"
    std::string mSemanticAttributes;       // string of single letter semantic boolean attributes: R - reference, V - current vaccine, v - previous vaccine, S - vaccine surrogate
    std::string mDate; // "D"
    std::vector<std::string> mLabId; // "l"
    std::vector<std::string> mClades; // "c"

}; // class Antigen

// ----------------------------------------------------------------------

class Serum : public SerumBase
{
 public:
    inline Serum() = default;
    inline std::string full_name() const override { return string::join({name(), reassortant(), serum_id(), annotations().join()}); } // serum_id comes before annotations, see hidb chart.cc Serum::variant_id
    inline std::string full_name_without_passage() const override { return full_name(); }
    inline std::string abbreviated_name() const override { return string::join({name_abbreviated(), reassortant(), annotations().join()}); }
    inline std::string abbreviated_name_with_passage_type() const override { return abbreviated_name(); }

    inline const std::string name() const override { return mName; }
    inline std::string& name() { return mName; }
    inline void name(const char* str, size_t length) { mName.assign(str, length); }
    inline const std::string lineage() const override { return mLineage; }
    inline std::string& lineage() { return mLineage; }
    inline void lineage(const char* str, size_t length) { mLineage.assign(str, length); }
    inline const std::string passage() const override { return mPassage; }
    inline std::string& passage() { return mPassage; }
    inline void passage(const char* str, size_t length) { mPassage.assign(str, length); }
    inline bool has_passage() const override { return !mPassage.empty(); }
    inline std::string passage_without_date() const override { return passage::without_date(mPassage); }
    inline const std::string reassortant() const override { return mReassortant; }
    inline std::string& reassortant() { return mReassortant; }
    inline void reassortant(const char* str, size_t length) { mReassortant.assign(str, length); }
    inline bool is_egg() const override { return passage::is_egg(mPassage) || is_reassortant(); } // reassortant is always egg (2016-10-21)
    inline bool is_reassortant() const override { return !mReassortant.empty(); }
    inline bool distinct() const override { return mAnnotations.distinct(); }
    inline const Annotations& annotations() const { return mAnnotations; }
    inline Annotations& annotations() { return mAnnotations; }
    inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
    inline const std::string semantic() const { return mSemanticAttributes; }
    inline void semantic(const char* str, size_t length) { mSemanticAttributes.assign(str, length); }
      // inline std::string passage_type() const { return is_egg() ? "egg" : "cell"; }
    std::string name_abbreviated() const;
    std::string location_abbreviated() const;

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
    inline const std::vector<size_t>& homologous() const override { return mHomologous; }
    inline std::vector<size_t>& homologous() { return mHomologous; }

    virtual AntigenSerumMatch match(const Serum& aSerum) const;
    virtual AntigenSerumMatch match(const Antigen& aAntigen) const;
    inline AntigenSerumMatch match(const AntigenSerumBase& aNother) const override { return match(static_cast<const Serum&>(aNother)); }
    AntigenSerumMatch match_passage(const AntigenSerumBase& aNother) const override;

 private:
    std::string mName; // "N" "[VIRUS_TYPE/][HOST/]LOCATION/ISOLATION/YEAR" or "CDC_ABBR NAME" or "NAME"
    std::string mLineage; // "L"
    std::string mPassage; // "P"
    std::string mReassortant; // "R"
    Annotations mAnnotations; // "a"
    std::string mSemanticAttributes;       // string of single letter semantic boolean attributes: R - reference, V - current vaccine, v - previous vaccine, S - vaccine surrogate
    std::string mSerumId; // "I"
    std::vector<size_t> mHomologous; // "h"
    std::string mSerumSpecies; // "s"

}; // class Serum

// ----------------------------------------------------------------------

constexpr static const size_t AntigenSerumNotFound = static_cast<size_t>(-1);

namespace acmacs_chart_internal
{
    using Indices = std::vector<size_t>;

    template <typename AgSr> inline Indices find_by_name(const AgSr& aAgSr, std::string aName)
    {
        auto name_match = [&](size_t index) -> bool { return aAgSr[index].name().find(aName) != std::string::npos; };
        Indices result(aAgSr.size());
        result.erase(std::copy_if(acmacs::incrementer<size_t>::begin(0), acmacs::incrementer<size_t>::end(aAgSr.size()), result.begin(), name_match), result.end());
        return result;
    }

    template <typename AgSr> inline std::optional<size_t> find_by_full_name(const AgSr& aAgSr, std::string aFullName)
    {
        auto name_match = [&](size_t index) -> bool { return aAgSr[index].full_name() == aFullName; };
        const auto found = std::find_if(acmacs::incrementer<size_t>::begin(0), acmacs::incrementer<size_t>::end(aAgSr.size()), name_match);
        if (*found == aAgSr.size())
            return {};
        else
            return *found;
    }
}

// ----------------------------------------------------------------------

template <typename AgSr> class AntigensSera : public std::vector<AgSr>
{
 public:
    using Indices = std::vector<size_t>;
    using std::vector<AgSr>::at;
    using std::vector<AgSr>::begin;
    using std::vector<AgSr>::end;

    inline AntigensSera() {}

    inline Indices find_by_name(std::string aName) const { return acmacs_chart_internal::find_by_name(*this, aName); }
    inline std::optional<size_t> find_by_full_name(std::string aFullName) const { return acmacs_chart_internal::find_by_full_name(*this, aFullName); }
    void find_by_name_matching(std::string aName, Indices& aIndices, string_match::score_t aScoreThreshold = 0, bool aVerbose = false) const;

    inline Indices all_indices() const { return acmacs::filled_with_indexes<Indices::value_type>(this->size()); }
    void filter_country(Indices& aIndices, std::string aCountry) const;
    void filter_continent(Indices& aIndices, std::string aContinent) const;

    inline Indices country(std::string aCountry) const  { auto indices = all_indices(); filter_country(indices, aCountry); return indices; }
    inline Indices continent(std::string aContinent) const  { auto indices = all_indices(); filter_country(indices, aContinent); return indices; }

 protected:
    inline void remove(Indices& aIndices, std::function<bool (const AgSr&)> aFilter) const
        {
            aIndices.erase(std::remove_if(aIndices.begin(), aIndices.end(), [&aFilter, this](auto index) -> bool { return aFilter((*this)[index]); }), aIndices.end());
        }

}; // class AntigensSera<AgSr>

extern template class AntigensSera<Antigen>;
extern template class AntigensSera<Serum>;

// ----------------------------------------------------------------------

// using AntigenRefs = std::vector<const Antigen*>;

class Antigens : public AntigensSera<Antigen>
{
 public:
    using CladeData = std::map<std::string, Indices>; // clade name to the list of antigen indices
    using ContinentData = std::map<std::string, Indices>; // continent name to the list of antigen/serum indices
    using CountryData = std::map<std::string, Indices>; // country name to the list of antigen/serum indices

    void find_by_lab_id(std::string aLabId, Indices& aAntigenIndices) const;
    void continents(ContinentData& aContinentData, bool aExcludeReference = true) const;
    void countries(CountryData& aCountries, bool aExcludeReference = true) const;

    inline void filter_reference(Indices& aIndices) const { remove(aIndices, [](const auto& entry) -> bool { return !entry.reference(); }); }
    inline void filter_test(Indices& aIndices) const { remove(aIndices, [](const auto& entry) -> bool { return entry.reference(); }); }
    inline void filter_egg(Indices& aIndices) const { remove(aIndices, [](const auto& entry) -> bool { return !entry.is_egg(); }); }
    inline void filter_cell(Indices& aIndices) const { remove(aIndices, [](const auto& entry) -> bool { return !entry.is_cell(); }); }
    inline void filter_reassortant(Indices& aIndices) const { remove(aIndices, [](const auto& entry) -> bool { return !entry.is_reassortant(); }); }
    inline void filter_date_range(Indices& aIndices, std::string first_date, std::string after_last_date) const { remove(aIndices, [=](const auto& entry) -> bool { return !entry.within_date_range(first_date, after_last_date); }); }
    inline void filter_found_in(Indices& aIndices, const Antigens& aNother) const { remove(aIndices, [&](const auto& entry) -> bool { return !aNother.find_by_full_name(entry.full_name()); }); }
    inline void filter_not_found_in(Indices& aIndices, const Antigens& aNother) const { remove(aIndices, [&](const auto& entry) -> bool { return aNother.find_by_full_name(entry.full_name()).has_value(); }); }

    inline Indices reference_indices() const { auto indices = all_indices(); filter_reference(indices); return indices; }
    inline Indices test_indices() const { auto indices = all_indices(); filter_test(indices); return indices; }
    inline Indices egg_indices() const { auto indices = all_indices(); filter_egg(indices); return indices; }
    inline Indices cell_indices() const { auto indices = all_indices(); filter_cell(indices); return indices; }
    inline Indices reassortant_indices() const { auto indices = all_indices(); filter_reassortant(indices); return indices; }
    inline Indices date_range_indices(std::string first_date, std::string after_last_date) const { auto indices = all_indices(); filter_date_range(indices, first_date, after_last_date); return indices; }

}; // class Antigens

// ----------------------------------------------------------------------

class Sera : public AntigensSera<Serum>
{
 public:
    inline void filter_serum_id(Indices& aIndices, std::string aSerumId) const { remove(aIndices, [&aSerumId](const auto& entry) -> bool { return entry.serum_id() != aSerumId; }); }

}; // class Sera

// ----------------------------------------------------------------------

class MinimumColumnBasis : public MinimumColumnBasisBase
{
 public:
    inline MinimumColumnBasis() : mValue{"none"}, mCached{static_cast<size_t>(-1)} {}
    inline MinimumColumnBasis(const MinimumColumnBasis& aSrc) : MinimumColumnBasisBase{aSrc}, mValue{aSrc.mValue}, mCached{static_cast<size_t>(-1)} {}
    inline operator size_t() const override { if (mCached == static_cast<decltype(mCached)>(-1)) mCached = mValue == "none" || mValue == "auto" ? 0 : std::stoul(mValue); return mCached; }
    inline operator std::string() const override { return mValue; }
    inline void assign(const char* str, size_t length) { mValue.assign(str, length); mCached = static_cast<decltype(mCached)>(-1); }

    inline std::string data() const { return mValue; }

 private:
    std::string mValue;
    mutable size_t mCached;

}; // class MinimumColumnBasis

// ----------------------------------------------------------------------

class ColumnBases : public ColumnBasesBase
{
 public:
    using ColumnBasesBase::ColumnBasesBase;
    inline void operator = (const ColumnBasesBase& aSrc) override { mColumnBases = DYNAMIC_CAST(const ColumnBases&, (aSrc)).mColumnBases; }

    inline double operator[](size_t aIndex) const override { return mColumnBases[aIndex]; }
    inline double at(size_t aIndex) const override { return mColumnBases.at(aIndex); }
    inline void set(size_t aIndex, double aValue) override { mColumnBases[aIndex] = aValue; }
    inline void clear() override { mColumnBases.clear(); }
    inline bool empty() const override { return mColumnBases.empty(); }
    inline size_t size() const override { return mColumnBases.size(); }
    inline void resize(size_t aNewSize) override { mColumnBases.resize(aNewSize); }

    inline auto& data() { return mColumnBases; }
    inline const auto& data() const { return mColumnBases; }

 private:
    std::vector<double> mColumnBases;

}; // class ColumnBases

// ----------------------------------------------------------------------

class Projection : public ProjectionBase
{
 public:
    inline Projection() : mStress(-1), mDodgyTiterIsRegular(false), mStressDiffToStop(1e-10) {}

    inline void comment(const char* str, size_t length) { mComment.assign(str, length); }
    inline std::string comment() const override { return mComment; }

    inline LayoutBase& layout() override { return mLayout; }
    inline const LayoutBase& layout() const override { return mLayout; }
    inline std::vector<std::vector<double>>& layout_for_json() { return reinterpret_cast<std::vector<std::vector<double>>&>(mLayout.data()); }
    inline const std::vector<Coordinates>& layout_for_json() const { return mLayout.data(); }

    inline void stress(double aStress) { mStress = aStress; }
    inline double stress() const override { return mStress; }

    inline void minimum_column_basis(const char* str, size_t length) { mMinimumColumnBasis.assign(str, length); }
    inline const MinimumColumnBasisBase& minimum_column_basis() const override { return mMinimumColumnBasis; }
    inline std::string minimum_column_basis_for_json() const { return mMinimumColumnBasis.data(); }

    inline ColumnBasesBase& column_bases() { return mColumnBases; }
    inline const ColumnBasesBase& column_bases() const override { return mColumnBases; }
    inline std::vector<double>& column_bases_for_json() { return mColumnBases.data(); }
    inline const std::vector<double>& column_bases_for_json() const { return mColumnBases.data(); }

    inline Transformation& transformation() { return mTransformation; } // required by json_importer
    inline const Transformation& transformation() const override { return mTransformation; }
    inline void transformation(const Transformation& aTransformation) override { mTransformation = aTransformation; }

    inline std::vector<double>& gradient_multipliers() { return mGradientMultipliers; }
    inline const std::vector<double>& gradient_multipliers() const { return mGradientMultipliers; }

    inline std::vector<double>& titer_multipliers() { return mTiterMultipliers; }
    inline const std::vector<double>& titer_multipliers() const { return mTiterMultipliers; }

    inline void dodgy_titer_is_regular(bool aDodgyTiterIsRegular) { mDodgyTiterIsRegular = aDodgyTiterIsRegular; }
    inline bool dodgy_titer_is_regular() const override { return mDodgyTiterIsRegular; }

    inline void stress_diff_to_stop(double aStressDiffToStop) { mStressDiffToStop = aStressDiffToStop; }
    inline double stress_diff_to_stop() const override { return mStressDiffToStop; }

    inline std::vector<size_t>& unmovable() { return mUnmovable; }
    inline const std::vector<size_t>& unmovable() const { return mUnmovable; }

    inline std::vector<size_t>& disconnected() { return mDisconnected; }
    inline const std::vector<size_t>& disconnected() const { return mDisconnected; }

    inline std::vector<size_t>& unmovable_in_last_dimension() { return mUnmovableInLastDimension; }
    inline const std::vector<size_t>& unmovable_in_last_dimension() const { return mUnmovableInLastDimension; }

    // inline size_t number_of_dimensions() const
    //     {
    //         for (const auto& point: mLayout) {
    //             if (!point.empty())
    //                 return point.size();
    //         }
    //         THROW(std::runtime_error("Internal: cannot find number_of_dimensions of projection"), 0);
    //     }

 private:
    std::string mComment;                           // "c"
    Layout mLayout;       // "l": [[]] layout, list of lists of doubles, if point is disconnected: emtpy list or ?[NaN, NaN]
      // size_t mNumberOfIterations;                // "i"
    double mStress;                                 // "s"
    MinimumColumnBasis mMinimumColumnBasis;         // "m": "1280", "none" (default)
    ColumnBases mColumnBases;                       // "C"
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

class ChartInfo : public ChartInfoBase
{
 public:
    enum TableType {Antigenic, Genetic};
    inline ChartInfo() : mType(Antigenic) {}
    inline ChartInfo(const ChartInfo&) = default;
    inline ChartInfo& operator=(const ChartInfo&) = default;
//     std::string table_id(std::string lineage) const;

    inline const std::string virus() const override { return merge_text_fields(&ChartInfo::mVirus); }
    inline const std::string virus_type() const override { return merge_text_fields(&ChartInfo::mVirusType); }
    inline const std::string assay() const override { return merge_text_fields(&ChartInfo::mAssay); }
    const std::string date() const override;
    inline const std::string lab() const override { return merge_text_fields(&ChartInfo::mLab); }
    inline const std::string rbc() const override { const std::string ass = assay(); return ass.empty() || ass == "HI" ? merge_text_fields(&ChartInfo::mRbc) : std::string(); }
    inline const std::string name() const override { return merge_text_fields(&ChartInfo::mName); }
    inline const std::string subset() const override { return merge_text_fields(&ChartInfo::mSubset); }
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

    const std::string make_name() const override;

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
                THROW_OR_VOID(std::runtime_error("ChartInfo: unrecognized table type: " + std::string(str, length)));
            switch (*str) {
              case 'A':
                  mType = Antigenic;
                  break;
              case 'G':
                  mType = Genetic;
                  break;
              default:
                  THROW_OR_VOID(std::runtime_error("ChartInfo: unrecognized table type: " + std::string(str, length)));
            }
        }

    inline std::string& virus_ref() { return mVirus; }
    inline std::string& virus_type_ref() { return mVirusType; }
    inline std::string& assay_ref() { return mAssay; }
    inline std::string& date_ref() { return mDate; }
    inline std::string& lab_ref() { return mLab; }
    inline std::string& rbc_ref() { return mRbc; }
    inline std::string& name_ref() { return mName; }
    inline std::string& subset_ref() { return mSubset; }

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
    inline Titer() : std::string() {}
    inline Titer(const char* s) : std::string(s) {}
    inline Titer(std::string source) : std::string(source) {}

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

    inline double similarity_with_thresholded() const
        {
            double result = similarity();
            if (is_more_than())
                ++result;
            else if (is_less_than())
                --result;
            return result;
        }

    inline size_t value_for_sorting() const
        {
            if (empty() || front() == '*')
                return 0;
            else if (front() == '<')
                return std::stoul(substr(1)) - 1;
            else if (front() == '>')
                return std::stoul(substr(1)) + 1;
            else
                return std::stoul(*this);
        }

    inline bool operator<(const Titer& aNother) const { return value_for_sorting() < aNother.value_for_sorting(); }
    inline bool operator==(const Titer& aNother) const { return static_cast<const std::string&>(*this) == static_cast<const std::string&>(aNother) || (is_dont_care() && aNother.is_dont_care()); }

}; // class Titer

// ----------------------------------------------------------------------

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

    inline size_t number_of_antigens() const { return !mList.empty() ? mList.size() : mDict.size(); }
    Titer get(size_t ag_no, size_t sr_no) const;
    Titer max_for_serum(size_t sr_no) const;

 private:
    List mList;                 // "l"
    Dict mDict;                 // "d"
    Layers mLayers;             // "L"

}; // class ChartTiters

// ----------------------------------------------------------------------

class Chart : public ChartBase
{
 public:
    using ChartBase::ChartBase;
    // inline Chart() {}
    // inline Chart(const Chart&) = default;
    // inline Chart& operator=(const Chart&) = default;
    // ~Chart();

    // inline std::string virus_type() const { return mInfo.virus_type(); }

    inline size_t number_of_antigens() const override { return mAntigens.size(); }
    inline size_t number_of_sera() const override { return mSera.size(); }
    inline size_t number_of_points() const { return number_of_antigens() + number_of_sera(); }
    std::string lineage() const;
    const std::string make_name(size_t aProjectionNo = static_cast<size_t>(-1)) const;

    inline const ChartInfoBase& chart_info() const override { return mInfo; }
    inline const ChartInfo& chart_info_for_json() const { return mInfo; }
    inline ChartInfo& chart_info() { return mInfo; }

    inline const Antigens& antigens() const { return mAntigens; }
    inline Antigens& antigens() { return mAntigens; }
    inline const AntigenBase& antigen(size_t ag_no) const override { return mAntigens[ag_no]; }
    // inline Antigen& antigen(size_t ag_no) { return mAntigens[ag_no]; }

    inline const Sera& sera() const { return mSera; }
    inline Sera& sera() { return mSera; }
    inline const SerumBase& serum(size_t sr_no) const override { return mSera[sr_no]; }
    // inline Serum& serum(size_t sr_no) { return mSera[sr_no]; }

    inline const ChartTiters& titers() const { return mTiters; }
    inline ChartTiters& titers() { return mTiters; }

    inline const auto& column_bases() const { return mColumnBases; }
    inline auto& column_bases() { return mColumnBases; }
    inline std::vector<double>& column_bases_for_json() { return mColumnBases.data(); }
    inline const std::vector<double>& column_bases_for_json() const { return mColumnBases.data(); }
    double compute_column_basis(const MinimumColumnBasisBase& aMinimumColumnBasis, size_t aSerumNo) const;
    inline void compute_column_bases(const MinimumColumnBasisBase& aMinimumColumnBasis, ColumnBases& aColumnBases) const
        {
            aColumnBases.resize(number_of_sera());
            for (size_t sr_no = 0; sr_no < number_of_sera(); ++sr_no)
                aColumnBases.set(sr_no, compute_column_basis(aMinimumColumnBasis, sr_no));
        }
    inline void column_bases(const MinimumColumnBasisBase& aMinimumColumnBasis, ColumnBases& aColumnBases) const
        {
            if (mColumnBases.empty()) {
                compute_column_bases(aMinimumColumnBasis, aColumnBases);
            }
            else {
                aColumnBases = mColumnBases;
                // aColumnBases.resize(mColumnBases.size());
                // std::copy(mColumnBases.begin(), mColumnBases.end(), aColumnBases.begin());
            }
        }
    inline double column_basis(const MinimumColumnBasisBase& aMinimumColumnBasis, size_t aSerumNo) const
        {
            if (mColumnBases.empty()) {
                return compute_column_basis(aMinimumColumnBasis, aSerumNo);
            }
            else {
                return mColumnBases.at(aSerumNo);
            }
        }
    inline void column_bases(size_t aProjectionNo, ColumnBases& aColumnBases) const
        {
            const auto& p = projection(aProjectionNo);
            if (p.column_bases().empty()) {
                column_bases(p.minimum_column_basis(), aColumnBases);
            }
            else {
                aColumnBases = mColumnBases;
                // aColumnBases.resize(p.column_bases().size());
                // std::copy(p.column_bases().begin(), p.column_bases().end(), aColumnBases.begin());
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
    inline ProjectionBase& projection(size_t aProjectionNo) { return mProjections[aProjectionNo]; }
    inline const ProjectionBase& projection(size_t aProjectionNo) const override { return mProjections[aProjectionNo]; }
    inline size_t number_of_projections() const override { return mProjections.size(); }

    inline const ChartPlotSpec& plot_spec() const { return mPlotSpec; }
    inline ChartPlotSpec& plot_spec() { return mPlotSpec; }

    // inline acmacs::IndexGenerator antigen_indices() const { return {number_of_antigens(), [](size_t) { return true; } }; }
    // inline acmacs::IndexGenerator reference_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].reference(); } }; }
    // inline acmacs::IndexGenerator test_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return !mAntigens[index].reference(); } }; }
    // inline acmacs::IndexGenerator egg_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].is_egg(); } }; }
    // inline acmacs::IndexGenerator reassortant_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return mAntigens[index].is_reassortant(); } }; }
    // inline acmacs::IndexGenerator serum_indices() const { return {number_of_antigens(), number_of_points(), [](size_t) { return true; } }; }

    inline acmacs::IndexGenerator antigens_not_found_in(const Chart& aNother) const
        {
            auto filter = [this,&aNother](size_t aIndex) -> bool {
                return !aNother.antigens().find_by_full_name(this->antigens()[aIndex].full_name());
            };
            return {number_of_antigens(), filter};
        }

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
    ColumnBases mColumnBases;              // "C"
    std::vector<Projection> mProjections;  // "P"
    ChartPlotSpec mPlotSpec;               // "p"

}; // class Chart

// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
