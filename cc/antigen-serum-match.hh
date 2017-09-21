#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <regex>

#include "acmacs-base/string-matcher.hh"

// ----------------------------------------------------------------------

class AntigenSerumMatch
{
 public:
    enum Level : size_t { Perfect=0, SerumSpeciesMismatch=0x1, PassageMismatch=0x2, PassageWithoutDateMismatch=0x4, EggCellUnknown=0x8, EggCellMismatch=0x10,
                          Mismatch=0x20, AnnotationMismatch=0x40, ReassortantMismatch=0x80, SerumIdMismatch=0x100, NameMismatch=0x200 };

    inline AntigenSerumMatch(Level aLevel = Perfect) : mLevel(aLevel) {}

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

#ifdef ACMACS_TARGET_OS
    friend inline std::ostream& operator << (std::ostream& out, const AntigenSerumMatch& m) { return out << "0x" << std::hex << m.mLevel << std::dec; }
#endif

}; // class AntigenSerumMatch

// ----------------------------------------------------------------------

namespace _antigen_serum_match
{
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

    inline static const std::regex sReCell{" (MDCK|SIAT|QMC)[0-9]"};
    inline static const std::regex sReEgg{" (.*/)?E[0-9]\\b"};
    inline static const std::regex sReReassortant{" (NYMC|NIB|[IB]VR|RESVIR|SB|(UW)?RG|VI|CBER|REASSORTANT)(?![A-Z])"};

#pragma GCC diagnostic pop

} // namespace _antigen_serum_match

// ----------------------------------------------------------------------

template <typename Data> class AntigenSerumMatchScore
{
 private:
    static constexpr const string_match::score_t keyword_in_lookup = 1;

 public:
    inline AntigenSerumMatchScore(std::string name, const Data& aAntigen, string_match::score_t aNameScoreThreshold)
        : mAntigen(&aAntigen), mName(0), mFull(0)
        {
            preprocess(name, aNameScoreThreshold);
        }

    inline AntigenSerumMatchScore(std::string name, const Data* aAntigen, string_match::score_t aNameScoreThreshold)
        : mAntigen(aAntigen), mName(0), mFull(0)
        {
            preprocess(name, aNameScoreThreshold);
        }

    inline bool operator < (const AntigenSerumMatchScore& aNother) const
        {
              // if mFull == keyword_in_lookup, move it to the end of the sorting list regardless of mName
            bool result;
            if (mFull == keyword_in_lookup)
                result = false;
            else if (aNother.mFull == keyword_in_lookup)
                result = true;
            else
                result = mName == aNother.mName ? mFull > aNother.mFull : mName > aNother.mName;
            return result;
        }

    inline bool operator == (const AntigenSerumMatchScore& aNother) const { return mName == aNother.mName; }
    inline operator const Data*() const { return mAntigen; }
    inline operator const Data&() const { return *mAntigen; }
      // inline const Data* data() const { return mAntigen; }
    inline operator bool() const { return mName > 0; }
    inline string_match::score_t name_score() const { return mName; }
    inline string_match::score_t full_name_score() const { return mFull; }
    inline std::pair<const Data*, size_t> score() const { return std::make_pair(mAntigen, mFull); }

 private:
    const Data* mAntigen;
    string_match::score_t mName, mFull;


    inline void preprocess(std::string name, string_match::score_t aNameScoreThreshold)
        {
            const auto antigen_name = mAntigen->name();
            mName = string_match::match(antigen_name, name);
            if (aNameScoreThreshold == 0)
                aNameScoreThreshold = static_cast<string_match::score_t>(name.length() * name.length() * 0.05);
            if (mName >= aNameScoreThreshold) {
                using namespace _antigen_serum_match;
                const auto full_name = mAntigen->full_name();
                const auto name_part_size = static_cast<int>(antigen_name.size());
                mFull = std::max({
                        for_subst(full_name, name_part_size, name, " CELL", sReCell, nullptr),
                        for_subst(full_name, name_part_size, name, " EGG", sReEgg, &sReReassortant),
                        for_subst(full_name, name_part_size, name, " REASSORTANT", sReReassortant, nullptr)

                    // for_subst(full_name, antigen_name.size(), name, " CELL", {" MDCK", " SIAT", " QMC"}, {}),
                    // for_subst(full_name, antigen_name.size(), name, " EGG", {" E"}, {"NYMC", "IVR", "NIB", "RESVIR", "RG", "VI", "REASSORTANT"}),
                    // for_subst(full_name, antigen_name.size(), name, " REASSORTANT", {" NYMC", " IVR", " NIB", " RESVIR", " RG", " VI", " REASSORTANT"}, {})
                    });
                if (mFull == 0)
                    mFull = string_match::match(full_name, name);
                  // std::cerr << mName << " " << mFull << " " << full_name << std::endl;
            }
        }

    // inline string_match::score_t for_subst(std::string full_name, size_t name_part_size, std::string name, std::string keyword, std::initializer_list<const char*>&& subst_list, std::initializer_list<const char*>&& negative_list)
    //     {
    //         string_match::score_t score = 0;
    //         const auto pos = name.find(keyword);
    //         if (pos != std::string::npos) { // keyword is in the lookup name
    //             if (!std::any_of(negative_list.begin(), negative_list.end(), [&full_name,name_part_size](const auto& e) -> bool { return full_name.find(e, name_part_size) != std::string::npos; })) {
    //                 for (const auto& subst: subst_list) {
    //                     if (full_name.find(subst + 1, name_part_size) != std::string::npos) { // subst (without leading space) must be present in full_name in the passage part
    //                         std::string substituted(name, 0, pos);
    //                         substituted.append(subst);
    //                         score = std::max(score, string_match::match(full_name, substituted));
    //                     }
    //                     else if (score == 0)
    //                         score = keyword_in_lookup; // to avoid using name-with-keyword-not-replaced for matching
    //                 }
    //             }
    //             else                // string from negative_list present in full_name, ignore this name
    //                 score = keyword_in_lookup;
    //         }
    //         return score;
    //     }

    inline string_match::score_t for_subst(std::string full_name, int name_part_size, std::string name, std::string keyword, const std::regex& positive, const std::regex* negative)
        {
            string_match::score_t score = 0;
            const auto pos = name.find(keyword);
            if (pos != std::string::npos) { // keyword is in the lookup name
                if (!negative || !std::regex_search(full_name.begin() + name_part_size, full_name.end(), *negative)) {
                    std::smatch match;
                    if (std::regex_search(full_name.cbegin() + name_part_size, full_name.cend(), match, positive)) {
                        std::string substituted(name, 0, pos);
                        substituted.append(match.str());
                        score = std::max(score, string_match::match(full_name, substituted));
                    }
                    else if (score == 0)
                        score = keyword_in_lookup; // to avoid using name-with-keyword-not-replaced for matching
                }
                else                // string from negative regex present in full_name, ignore this name
                    score = keyword_in_lookup;
            }
            return score;
        }
};

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
