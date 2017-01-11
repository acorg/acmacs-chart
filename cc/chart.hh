#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "acmacs-base/virus-name.hh"
#include "acmacs-base/color.hh"
#include "acmacs-base/string.hh"

namespace hidb { class HiDb; }

// ----------------------------------------------------------------------

class Annotations : public std::vector<std::string>
{
 public:
    inline Annotations() = default;

    inline bool has(std::string anno) const { return std::find(begin(), end(), anno) != end(); }
    inline bool distinct() const { return has("DISTINCT"); }

    inline void sort() { std::sort(begin(), end()); }
    inline void sort() const { const_cast<Annotations*>(this)->sort(); }

    inline std::string join() const { return string::join(" ", *this); }

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
//     virtual bool is_egg() const;
//     inline bool is_reassortant() const { return !mReassortant.empty(); }
//     inline bool distinct() const { return mAnnotations.distinct(); }
    inline const Annotations& annotations() const { return mAnnotations; }
    inline Annotations& annotations() { return mAnnotations; }
//     inline bool has_semantic(char c) const { return mSemanticAttributes.find(c) != std::string::npos; }
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
    virtual std::string full_name() const;

    // inline void name(const char* str, size_t length) { AntigenSerum::name(str, length); }

    inline const std::string date() const { return mDate; }
    inline void date(const char* str, size_t length) { mDate.assign(str, length); }
//     inline bool reference() const { return has_semantic('R'); }
    inline const std::vector<std::string>& lab_id() const { return mLabId; }
    inline std::vector<std::string>& lab_id() { return mLabId; }
    inline bool has_lab_id(std::string aLabId) const { return std::find(mLabId.begin(), mLabId.end(), aLabId) != mLabId.end(); }
    inline const std::vector<std::string>& clades() const { return mClades; }
    inline std::vector<std::string>& clades() { return mClades; }

    void find_in_hidb(const hidb::HiDb& aHiDb) const;

//     virtual std::string variant_id() const;

//     using AntigenSerum::match;
//     virtual AntigenSerumMatch match(const Serum& aNother) const;
//     virtual AntigenSerumMatch match(const Antigen& aNother) const;

 private:
    std::string mDate; // "D"
    std::vector<std::string> mLabId; // "l"
    std::vector<std::string> mClades; // "c"

}; // class Antigen

// ----------------------------------------------------------------------

class Serum : public AntigenSerum
{
 public:
    inline Serum() : mHomologous(-1) {}
      // inline Serum(Chart& aChart) : AntigenSerum(aChart), mHomologous(-1) {}
    inline Serum(const Serum&) = default;
    inline Serum(Serum&&) = default;
    virtual std::string full_name() const;

    inline const std::string serum_id() const { return mSerumId; }
    inline void serum_id(const char* str, size_t length) { mSerumId.assign(str, length); }
    inline const std::string serum_species() const { return mSerumSpecies; }
    inline void serum_species(const char* str, size_t length) { mSerumSpecies.assign(str, length); }

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
    std::string mSerumId; // "I"
    int mHomologous; // "h"
    std::string mSerumSpecies; // "s"

}; // class Serum

// ----------------------------------------------------------------------

class Projection
{
 public:
      // inline Serum(Chart& aChart) : AntigenSerum(aChart), mHomologous(-1) {}
    inline Projection() : mStress(-1), mDodgyTiterIsRegular(false), mStressDiffToStop(1e-10) {}

    inline void comment(const char* str, size_t length) { mComment.assign(str, length); }
    inline std::string comment() const { return mComment; }

    inline std::vector<std::vector<double>>& layout() { return mLayout; }
    inline const std::vector<std::vector<double>>& layout() const { return mLayout; }

    inline void stress(double aStress) { mStress = aStress; }
    inline double stress() const { return mStress; }

    inline void minimum_column_basis(const char* str, size_t length) { mMinimumColumnBasis.assign(str, length); }
    inline std::string minimum_column_basis() const { return mMinimumColumnBasis; }

    inline std::vector<double>& column_bases() { return mColumnBases; }
    inline const std::vector<double>& column_bases() const { return mColumnBases; }

    inline std::vector<double>& transformation() { return mTransformation; }
    inline const std::vector<double>& transformation() const { return mTransformation; }

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

class LabelStyle
{
 public:
    enum class Slant {Normal, Italic, Oblique};
    enum class Weight {Normal, Bold};

    inline LabelStyle() : mShown(true), mSlant(Slant::Normal), mWeight(Weight::Normal), mSize(1), mColor(0), mRotation(0), mInterline(0.2) {}

    inline void shown(bool aShown) { mShown = aShown; }
    inline bool shown() const { return mShown; }

    inline std::vector<double>& position() { return mPosition; }
    inline const std::vector<double>& position() const { return mPosition; }

    inline void text(const char* str, size_t length) { mText.assign(str, length); }
    inline std::string text() const { return mText; }

    inline void face(const char* str, size_t length) { mFace.assign(str, length); }
    inline std::string face() const { return mFace; }

    inline void slant(const char* str, size_t length) { mSlant = slant_from_string(str, length); }
    inline Slant slant() const { return mSlant; }
    inline std::string slant_as_stirng() const
        {
            switch (mSlant) {
              case Slant::Normal:
                  return "normal";
              case Slant::Italic:
                  return "italic";
              case Slant::Oblique:
                  return "oblique";
            }
            return "?";
        }

    inline void weight(const char* str, size_t length) { mWeight = weight_from_string(str, length); }
    inline Weight weight() const { return mWeight; }
    inline std::string weight_as_stirng() const
        {
            switch (mWeight) {
              case Weight::Normal:
                  return "normal";
              case Weight::Bold:
                  return "bold";
            }
            return "?";
        }

    inline void size(double aSize) { mSize = aSize; }
    inline double size() const { return mSize; }

    inline void color(const char* str, size_t length) { mColor.from_string(str, length); }
    inline std::string color() const { return mColor; }

    inline void rotation(double aRotation) { mRotation = aRotation; }
    inline double rotation() const { return mRotation; }

    inline void interline(double aInterline) { mInterline = aInterline; }
    inline double interline() const { return mInterline; }

 private:
    bool mShown;                   // "+"
    std::vector<double> mPosition; // "p": [0.0, 1.0] label position (2D only), list of two doubles, default is [0, 1] means under point
    std::string mText;             // "t": "label text if forced by user",
    std::string mFace;             // "f": "font face",
    Slant mSlant;                  // "S": "normal OR italic OR oblique", // font slant, default normal
    Weight mWeight;                // "W": "normal OR bold", // font weight, default normal
    double mSize;                  // "s": 1.0,           // size, default 1.0
    Color mColor;                  // "c": "black",   // color, default black
    double mRotation;              // "r": 0.0,       // rotation, default 0.0
    double mInterline;             // "i": 0.2, // addtional interval between lines as a fraction of line height, default - 0.2

    inline Slant slant_from_string(const char* str, size_t length) const
        {
            const std::string s{str, length};
            if (s == "normal")
                return Slant::Normal;
            if (s == "italic")
                return Slant::Italic;
            if (s == "oblique")
                return Slant::Oblique;
            throw std::runtime_error("Unrecognized font slant: " + s);
        }

    inline Weight weight_from_string(const char* str, size_t length) const
        {
            const std::string s{str, length};
            if (s == "normal")
                return Weight::Normal;
            if (s == "bold")
                return Weight::Bold;
            throw std::runtime_error("Unrecognized font weight: " + s);
        }

}; // class LabelStyle

// ----------------------------------------------------------------------

class ChartPlotSpecStyle
{
 public:
    enum Shape {Circle, Box, Triangle};

    inline ChartPlotSpecStyle() : mShown(true), mFillColor("transparent"), mOutlineColor("black"), mOutlineWidth(1), mShape(Circle), mSize(1), mRotation(0), mAspect(1) {}

    inline void shown(bool aShown) { mShown = aShown; }
    inline bool shown() const { return mShown; }

    inline void fill_color(const char* str, size_t length) { mFillColor.from_string(str, length); }
    inline std::string fill_color() const { return mFillColor; }

    inline void outline_color(const char* str, size_t length) { mOutlineColor.from_string(str, length); }
    inline std::string outline_color() const { return mOutlineColor; }

    inline void outline_width(double aOutlineWidth) { mOutlineWidth = aOutlineWidth; }
    inline double outline_width() const { return mOutlineWidth; }

    inline void shape(const char* str, size_t length) { mShape = shape_from_string(str, length); }
    inline Shape shape() const { return mShape; }
    inline std::string shape_as_string() const
        {
            switch(mShape) {
              case Circle:
                  return "C";
              case Box:
                  return "B";
              case Triangle:
                  return "T";
            }
            return "?";
        }

    inline void size(double aSize) { mSize = aSize; }
    inline double size() const { return mSize; }

    inline void rotation(double aRotation) { mRotation = aRotation; }
    inline double rotation() const { return mRotation; }

    inline void aspect(double aAspect) { mAspect = aAspect; }
    inline double aspect() const { return mAspect; }

    inline LabelStyle& label() { return mLabel; }
    inline const LabelStyle& label() const { return mLabel; }

 private:
    bool mShown;          // "+"
    Color mFillColor;     //  "F": "fill color: #FF0000 or T[RANSPARENT] or color name (red, green, blue, etc.), default is transparent",
    Color mOutlineColor;  // "O": "outline color: #000000 or T[RANSPARENT] or color name (red, green, blue, etc.), default is black",
    double mOutlineWidth; // "o": 1.0,             // outline width, default 1.0
    Shape mShape;         // "S": "shape: C[IRCLE], B[OX], T[RIANGLE], default is circle",
    double mSize;         // "s": 1.0,             // size, default is 1.0
    double mRotation;     // "r": 0.0,             // rotation in radians, default is 0.0
    double mAspect;       // "a": 1.0,              // aspect ratio, default is 1.0
    LabelStyle mLabel;    // "l"

    inline Shape shape_from_string(const char* str, size_t length) const
        {
            switch (*str) {
              case 'C':
              case 'c':
                  return Circle;
              case 'B':
              case 'b':
                  return Box;
              case 'T':
              case 't':
                  return Triangle;
            }
            throw std::runtime_error("Unrecognized point shape: " + std::string(str, length));
        }

}; // class ChartPlotSpecStyle

// ----------------------------------------------------------------------

class ChartPlotSpec
{
 public:
    inline ChartPlotSpec() {}

    inline std::vector<size_t>& drawing_order() { return mDrawingOrder; }
    inline const std::vector<size_t>& drawing_order() const { return mDrawingOrder; }

    inline std::vector<size_t>& style_for_point() { return mStyleForPoint; }
    inline const std::vector<size_t>& style_for_point() const { return mStyleForPoint; }

    inline std::vector<ChartPlotSpecStyle>& styles() { return mStyles; }
    inline const std::vector<ChartPlotSpecStyle>& styles() const { return mStyles; }

    inline std::vector<size_t>& shown_on_all() { return mShownOnAll; }
    inline const std::vector<size_t>& shown_on_all() const { return mShownOnAll; }

    inline bool empty() const { return mDrawingOrder.empty() && mStyleForPoint.empty() && mStyles.empty() && mShownOnAll.empty(); }

 private:
    std::vector<size_t> mDrawingOrder;       // "d"
    std::vector<size_t> mStyleForPoint;      // "p"
    std::vector<ChartPlotSpecStyle> mStyles; // "P"
    std::vector<size_t> mShownOnAll;         // "s"

}; // class ChartPlotSpec

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

    inline const ChartPlotSpec& plot_spec() const { return mPlotSpec; }
    inline ChartPlotSpec& plot_spec() { return mPlotSpec; }

    // void find_homologous_antigen_for_sera();
    // inline void find_homologous_antigen_for_sera_const() const { const_cast<Chart*>(this)->find_homologous_antigen_for_sera(); }

    // inline bool operator < (const Chart& aNother) const { return table_id() < aNother.table_id(); }

 private:
    ChartInfo mInfo;                       // "i"
    std::vector<Antigen> mAntigens;        // "a"
    std::vector<Serum> mSera;              // "s"
    ChartTiters mTiters;                   // "t"
    std::vector <double> mColumnBases;     // "C"
    std::vector<Projection> mProjections;  // "P"
    ChartPlotSpec mPlotSpec;               // "p"

}; // class Chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
