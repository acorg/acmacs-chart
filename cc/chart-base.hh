#pragma once

#include <string>

#include "acmacs-base/range.hh"
#include "acmacs-chart/layout-base.hh"
#include "acmacs-chart/antigen-serum-match.hh"

class LocDb;

// ----------------------------------------------------------------------

class AntigenSerumBase
{
 public:
    inline AntigenSerumBase() = default;
    inline AntigenSerumBase(const AntigenSerumBase&) = default;
    virtual ~AntigenSerumBase();

    inline AntigenSerumBase& operator=(const AntigenSerumBase&) = default;

    virtual std::string full_name() const = 0;
    virtual std::string full_name_without_passage() const = 0;
    virtual std::string abbreviated_name(const LocDb& aLocDb) const = 0;

    virtual const std::string name() const = 0;
    virtual const std::string lineage() const = 0;
    virtual const std::string passage() const = 0;
    virtual bool has_passage() const = 0;
    virtual std::string passage_without_date() const = 0;
    virtual const std::string reassortant() const = 0;
    virtual bool is_egg() const = 0;
    virtual bool is_reassortant() const = 0;
    virtual bool distinct() const = 0;
    // inline const Annotations& annotations() const { return mAnnotations; }
    // inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
    // inline const std::string semantic() const { return mSemanticAttributes; }
    inline std::string passage_type() const { return is_egg() ? "egg" : "cell"; }

    virtual AntigenSerumMatch match(const AntigenSerumBase& aNother) const = 0;
    virtual AntigenSerumMatch match_passage(const AntigenSerumBase& aNother) const = 0;

//     virtual std::string variant_id() const = 0;

//       // returned cdc abbreviation starts with #
//     inline std::string location() const { return virus_name::location(mName); }
//     inline std::string year() const { return virus_name::year(mName); }

//     inline bool operator == (const AntigenSerum& aNother) const { return name() == aNother.name() && variant_id() == aNother.variant_id(); }
//     inline bool operator < (const AntigenSerum& aNother) const { return name() == aNother.name() ? variant_id() < aNother.variant_id() : name() < aNother.name(); }

}; // class AntigenSerumBase

// ----------------------------------------------------------------------

class AntigenBase : public AntigenSerumBase
{
 public:
    virtual bool reference() const = 0;

}; // class AntigenBase

// ----------------------------------------------------------------------

class SerumBase : public AntigenSerumBase
{
 public:
    virtual const std::vector<size_t>& homologous() const = 0;

}; // class SerumBase

// ----------------------------------------------------------------------

class MinimumColumnBasisBase
{
 public:
    inline MinimumColumnBasisBase() : mCached{static_cast<size_t>(-1)} {}
    inline MinimumColumnBasisBase(const MinimumColumnBasisBase&) : mCached{static_cast<size_t>(-1)} {}
    virtual ~MinimumColumnBasisBase();
    virtual operator size_t() const = 0;
    virtual operator std::string() const = 0;
    inline std::string str() const { return static_cast<std::string>(*this); }

 protected:
    mutable size_t mCached;

}; // class MinimumColumnBasisBase

// ----------------------------------------------------------------------

class ColumnBasesBase
{
 public:
    inline ColumnBasesBase() = default;
    inline ColumnBasesBase(const ColumnBasesBase&) = default;
    virtual ~ColumnBasesBase();
    virtual void operator = (const ColumnBasesBase& aSrc) = 0;

    virtual double operator[](size_t aIndex) const = 0;
    virtual double at(size_t aIndex) const = 0;
    virtual void set(size_t aIndex, double aValue) = 0;
    virtual void clear() = 0;
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    virtual void resize(size_t aNewSize) = 0;

}; // class ColumnBasesBase

// ----------------------------------------------------------------------

class ProjectionBase
{
 public:
    inline ProjectionBase() = default;
    inline ProjectionBase(const ProjectionBase&) = default;
    virtual ~ProjectionBase();

    virtual std::string comment() const = 0;

      // virtual LayoutBase& layout() = 0;
    virtual const LayoutBase& layout() const = 0;

      // virtual void stress(double aStress) = 0;
    virtual double stress() const = 0;

    virtual const MinimumColumnBasisBase& minimum_column_basis() const = 0;

      // virtual ColumnBasesBase& column_bases() = 0;
    virtual const ColumnBasesBase& column_bases() const = 0;

      // virtual Transformation& transformation()  = 0;
    virtual const Transformation& transformation() const = 0;

    // inline std::vector<double>& gradient_multipliers() { return mGradientMultipliers; }
    // inline const std::vector<double>& gradient_multipliers() const { return mGradientMultipliers; }

    // inline std::vector<double>& titer_multipliers() { return mTiterMultipliers; }
    // inline const std::vector<double>& titer_multipliers() const { return mTiterMultipliers; }

    // inline void dodgy_titer_is_regular(bool aDodgyTiterIsRegular) { mDodgyTiterIsRegular = aDodgyTiterIsRegular; }
    virtual bool dodgy_titer_is_regular() const = 0 ;

    // virtual void stress_diff_to_stop(double aStressDiffToStop) = 0;
    virtual double stress_diff_to_stop() const = 0;

    // inline std::vector<size_t>& unmovable() { return mUnmovable; }
    // inline const std::vector<size_t>& unmovable() const { return mUnmovable; }

    // inline std::vector<size_t>& disconnected() { return mDisconnected; }
    // inline const std::vector<size_t>& disconnected() const { return mDisconnected; }

    // inline std::vector<size_t>& unmovable_in_last_dimension() { return mUnmovableInLastDimension; }
    // inline const std::vector<size_t>& unmovable_in_last_dimension() const { return mUnmovableInLastDimension; }

    inline size_t number_of_dimensions() const { return layout().number_of_dimensions(); }

}; // class ProjectionBase

// ----------------------------------------------------------------------

class ChartInfoBase
{
 public:
    inline ChartInfoBase() = default;
    inline ChartInfoBase(const ChartInfoBase&) = default;
    virtual ~ChartInfoBase();
    inline ChartInfoBase& operator=(const ChartInfoBase&) = default;

    virtual const std::string virus() const = 0;
    virtual const std::string virus_type() const = 0;
    virtual const std::string assay() const = 0;
    virtual const std::string lab() const = 0;
    virtual const std::string rbc() const = 0;
    virtual const std::string name() const = 0;
    virtual const std::string subset() const = 0;
    virtual const std::string date() const = 0;
    virtual const std::string make_name() const = 0;

}; // class ChartInfoBase

// ----------------------------------------------------------------------

class ChartBase
{
 public:
    inline ChartBase() {}
    inline ChartBase(const ChartBase&) = default;
    inline ChartBase& operator=(const ChartBase&) = default;
    virtual ~ChartBase();

    // inline std::string virus_type() const { return mInfo.virus_type(); }

    virtual size_t number_of_antigens() const = 0;
    virtual size_t number_of_sera() const = 0;
    inline size_t number_of_points() const { return number_of_antigens() + number_of_sera(); }
    // std::string lineage() const;
    // const std::string make_name(size_t aProjectionNo = static_cast<size_t>(-1)) const;

    virtual const ChartInfoBase& chart_info() const = 0;

    // inline const Antigens& antigens() const { return mAntigens; }
    // inline Antigens& antigens() { return mAntigens; }
    virtual const AntigenBase& antigen(size_t ag_no) const  = 0;
    // inline Antigen& antigen(size_t ag_no) { return mAntigens[ag_no]; }

    // inline const Sera& sera() const { return mSera; }
    // inline Sera& sera() { return mSera; }
    virtual const SerumBase& serum(size_t sr_no) const = 0;
    // inline Serum& serum(size_t sr_no) { return mSera[sr_no]; }

    // inline const ChartTiters& titers() const { return mTiters; }
    // inline ChartTiters& titers() { return mTiters; }

    // inline const auto& column_bases() const { return mColumnBases; }
    // inline auto& column_bases() { return mColumnBases; }

    // double compute_column_basis(MinimumColumnBasis aMinimumColumnBasis, size_t aSerumNo) const;
    // inline void compute_column_bases(MinimumColumnBasis aMinimumColumnBasis, std::vector<double>& aColumnBases) const
    //     {
    //         aColumnBases.resize(number_of_sera());
    //         for (size_t sr_no = 0; sr_no < number_of_sera(); ++sr_no)
    //             aColumnBases[sr_no] = compute_column_basis(aMinimumColumnBasis, sr_no);
    //     }
    // inline void column_bases(MinimumColumnBasis aMinimumColumnBasis, std::vector<double>& aColumnBases) const
    //     {
    //         if (mColumnBases.empty()) {
    //             compute_column_bases(aMinimumColumnBasis, aColumnBases);
    //         }
    //         else {
    //             aColumnBases.resize(mColumnBases.size());
    //             std::copy(mColumnBases.begin(), mColumnBases.end(), aColumnBases.begin());
    //         }
    //     }
    // inline double column_basis(MinimumColumnBasis aMinimumColumnBasis, size_t aSerumNo) const
    //     {
    //         if (mColumnBases.empty()) {
    //             return compute_column_basis(aMinimumColumnBasis, aSerumNo);
    //         }
    //         else {
    //             return mColumnBases.at(aSerumNo);
    //         }
    //     }
    // inline void column_bases(size_t aProjectionNo, std::vector<double>& aColumnBases) const
    //     {
    //         const auto& p = projection(aProjectionNo);
    //         if (p.column_bases().empty()) {
    //             column_bases(p.minimum_column_basis(), aColumnBases);
    //         }
    //         else {
    //             aColumnBases.resize(p.column_bases().size());
    //             std::copy(p.column_bases().begin(), p.column_bases().end(), aColumnBases.begin());
    //         }
    //     }
    // inline double column_basis(size_t aProjectionNo, size_t aSerumNo) const
    //     {
    //         double result = 0.0;
    //         const auto& p = projection(aProjectionNo);
    //         if (p.column_bases().empty()) {
    //             result = column_basis(p.minimum_column_basis(), aSerumNo);
    //         }
    //         else {
    //             result = p.column_bases().at(aSerumNo);
    //         }
    //         return result;
    //     }

    // inline std::vector<Projection>& projections() { return mProjections; }
    // inline const std::vector<Projection>& projections() const { return mProjections; }
    // virtual ProjectionBase& projection(size_t aProjectionNo) = 0;
    virtual const ProjectionBase& projection(size_t aProjectionNo) const = 0;
    virtual size_t number_of_projections() const = 0;

    // inline const ChartPlotSpec& plot_spec() const { return mPlotSpec; }
    // inline ChartPlotSpec& plot_spec() { return mPlotSpec; }

    inline IndexGenerator antigen_indices() const { return {number_of_antigens(), [](size_t) { return true; } }; }
    inline IndexGenerator reference_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return antigen(index).reference(); } }; }
    inline IndexGenerator test_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return !antigen(index).reference(); } }; }
    inline IndexGenerator egg_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return antigen(index).is_egg(); } }; }
    inline IndexGenerator reassortant_antigen_indices() const { return {number_of_antigens(), [this](size_t index) { return antigen(index).is_reassortant(); } }; }
    inline IndexGenerator serum_indices() const { return {number_of_antigens(), number_of_points(), [](size_t) { return true; } }; }

    // inline IndexGenerator antigens_not_found_in(const Chart& aNother) const
    //     {
    //         auto filter = [this,&aNother](size_t aIndex) -> bool {
    //             const size_t found = aNother.antigens().find_by_name_for_exact_matching(this->antigens()[aIndex].full_name());
    //             return found == static_cast<size_t>(-1);
    //         };
    //         return {number_of_antigens(), filter};
    //     }

    // void find_homologous_antigen_for_sera();
    // inline void find_homologous_antigen_for_sera_const() const { const_cast<Chart*>(this)->find_homologous_antigen_for_sera(); }

    //   // Negative radius means calculation failed (e.g. no homologous titer)
    // double serum_circle_radius(size_t aAntigenNo, size_t aSerumNo, size_t aProjectionNo, bool aVerbose = false) const;
    //   // aWithin4Fold: indices of antigens within 4fold from homologous titer
    //   // aOutside4Fold: indices of antigens with titers against aSerumNo outside 4fold distance from homologous titer
    // void serum_coverage(size_t aAntigenNo, size_t aSerumNo, std::vector<size_t>& aWithin4Fold, std::vector<size_t>& aOutside4Fold) const;

    // inline bool operator < (const Chart& aNother) const { return table_id() < aNother.table_id(); }

 private:
    // ChartInfo mInfo;                       // "i"
    // Antigens mAntigens;                    // "a"
    // Sera mSera;                            // "s"
    // ChartTiters mTiters;                   // "t"
    // std::vector <double> mColumnBases;     // "C"
    // std::vector<Projection> mProjections;  // "P"
    // ChartPlotSpec mPlotSpec;               // "p"

}; // class ChartBase

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
