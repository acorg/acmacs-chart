#include <set>

#include "acmacs-base/virus-name.hh"
#include "locationdb/locdb.hh"

#include "chart.hh"

// ----------------------------------------------------------------------

Chart::~Chart()
{
}

// ----------------------------------------------------------------------

AntigenSerum::~AntigenSerum()
{
}

// ----------------------------------------------------------------------

AntigenSerumMatch AntigenSerum::match(const AntigenSerum& aNother) const
{
      // fields not used for matching
      // lineage, semantic-attributes, annotations

    AntigenSerumMatch match;
    if (!distinct() && !aNother.distinct() && name() == aNother.name()) {
        if (reassortant() == aNother.reassortant()) {
            match.add(match_passage(aNother));
        }
        else {
            match.add(AntigenSerumMatch::ReassortantMismatch);
        }
    }
    else {
        match.add(AntigenSerumMatch::NameMismatch);
    }
    return match;

} // AntigenSerum::match

// ----------------------------------------------------------------------

AntigenSerumMatch AntigenSerum::match_passage(const AntigenSerum& aNother) const
{
    AntigenSerumMatch match;
    if (has_passage() && aNother.has_passage()) {
        if (passage() != aNother.passage()) {
            match.add(AntigenSerumMatch::PassageMismatch);
            if (passage_without_date() != aNother.passage_without_date()) {
                match.add(AntigenSerumMatch::PassageWithoutDateMismatch);
                  // std::cerr << "  ?egg " << is_egg() << "  " << full_name() << std::endl;
                  // std::cerr << "  ?egg " << aNother.is_egg() << "  " << aNother.full_name() << std::endl;
                if (is_egg() != aNother.is_egg()) {
                    match.add(AntigenSerumMatch::EggCellMismatch);
                }
            }
        }
    }
    else {
        match.add(AntigenSerumMatch::EggCellUnknown);
    }
    return match;

} // AntigenSerum::match_passage

// ----------------------------------------------------------------------

AntigenSerumMatch Antigen::match(const Antigen& aNother) const
{
      // fields not used for matching
      // date, clades, lab_id

    AntigenSerumMatch m = AntigenSerum::match(aNother);
    if (annotations() != aNother.annotations()) {
        m.add(AntigenSerumMatch::AnnotationMismatch);
    }
    return m;

} // Antigen::match

// ----------------------------------------------------------------------

AntigenSerumMatch Antigen::match(const Serum& aNother) const
{
    return aNother.match(*this);

} // Antigen::match

// ----------------------------------------------------------------------

AntigenSerumMatch Serum::match(const Serum& aNother) const
{
      // fields not used for matching
      // homologous

    AntigenSerumMatch m = AntigenSerum::match(aNother);
    if (m < AntigenSerumMatch::Mismatch) {
        if (annotations() != aNother.annotations()) {
            m.add(AntigenSerumMatch::AnnotationMismatch);
        }
        else {
            if (serum_id() != aNother.serum_id())
                m.add(AntigenSerumMatch::SerumIdMismatch);
            if (serum_species() != aNother.serum_species())
                m.add(AntigenSerumMatch::SerumSpeciesMismatch);
        }
    }
    return m;

} // Serum::match

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif

AntigenSerumMatch Serum::match(const Antigen& aNother) const
{
    AntigenSerumMatch m = AntigenSerum::match(aNother);
    if (m < AntigenSerumMatch::Mismatch) {
                  // ignore serum specific annotations (CONC*, BOOSTED*, *BLEED)
        Annotations self_filtered;
        static std::regex serum_specific {"(CONC|BOOSTED|BLEED)"};
        static auto filter = [](const auto& anno) -> bool { return !std::regex_search(anno, serum_specific); };
        std::copy_if(annotations().begin(), annotations().end(), std::back_inserter(self_filtered), filter);
        if (self_filtered != aNother.annotations()) {
            m.add(AntigenSerumMatch::AnnotationMismatch);
        }
    }
    return m;

} // Serum::match

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

AntigenSerumMatch Serum::match_passage(const AntigenSerum& aNother) const
{
    AntigenSerumMatch match;
    if (!has_passage() && aNother.has_passage()) {
        if (is_egg() != aNother.is_egg()) {
            match.add(AntigenSerumMatch::EggCellMismatch);
        }
    }
    else {
        match = AntigenSerum::match_passage(aNother);
    }
    return match;

} // Serum::match_passage

// ----------------------------------------------------------------------

// std::string AntigenSerum::passage_without_date() const
// {
//     if (mPassage.size() > 13 && mPassage[mPassage.size() - 1] == ')' && mPassage[mPassage.size() - 12] == '(' && mPassage[mPassage.size() - 13] == ' ' && mPassage[mPassage.size() - 4] == '-' && mPassage[mPassage.size() - 7] == '-')
//         return std::string(mPassage, 0, mPassage.size() - 13);
//     else
//         return mPassage;

// } // AntigenSerum::passage_without_date

// ----------------------------------------------------------------------

// const hidb::AntigenSerumData<hidb::Antigen>& Antigen::find_in_hidb(const hidb::HiDb& aHiDb) const
// {
//     const std::string name = full_name();
//     try {
//         const auto& found = aHiDb.find_antigen_exactly(name);
//           // std::cerr << "find_in_hidb: " << full_name() << " --> " << found.most_recent_table().table_id() << " tables:" << found.number_of_tables() << std::endl;
//         return found;
//     }
//     catch (hidb::HiDb::NotFound& err) {
//         if (passage() == "X?") {
//             try {
//                 return aHiDb.find_antigen_exactly(full_name_without_passage());
//             }
//             catch (hidb::HiDb::NotFound&) {
//             }
//         }
//         else if (!err.suggestions().empty()) {
//             return find_in_suggestions(name, err.suggestions());
//         }
//         else {
//             // std::cerr << "ERROR: not found and no suggestions for " << name << std::endl
//             //           << hidb::report(aHiDb.find_antigens(name), "  ") << std::endl;
//         }
//         throw;
//     }

// } // Antigen::find_in_hidb

// ----------------------------------------------------------------------

// const hidb::AntigenSerumData<hidb::Antigen>& Antigen::find_in_suggestions(std::string aName, const hidb::AntigenRefs& aSuggestions) const
// {
//     const std::regex wrongly_converted{" (SECM|VIR)(-)"};
//     std::smatch m;

//     if (aName.find(" DISTINCT") != std::string::npos) { // DISTINCT antigens are not stored in hidb
//         throw hidb::HiDb::NotFound(aName);
//     }
//     else if (std::regex_search(aName, m, wrongly_converted)) {
//         // std::cerr << "SECM Suggestions for " << aName << std::endl
//         //           << hidb::report(aSuggestions, "  ") << std::endl;
//         std::string name = aName;   // to avoid aName changing
//         name[static_cast<size_t>(m[2].first - aName.begin())] = '0';
//         for (const auto& e: aSuggestions) {
//             if (e->data().full_name() == name)
//                 return *e;
//         }
//     }
//     else if (aName[2] == ' ') {
//           // some cdc names were incorrectly used before, e.g. "CO CO-9-2718" was used as "CO 9-2718"
//         std::string fixed = aName.substr(0, 3) + aName.substr(0, 2) + "-" + aName.substr(3);
//         // std::cerr << "FIXED: " << fixed << std::endl; // << report(*fk, "  ") << std::endl;
//         const auto found = std::find_if(aSuggestions.begin(), aSuggestions.end(), [&fixed](const auto& e) -> bool { return e->data().full_name() == fixed; });
//         if (found != aSuggestions.end())
//             return **found;
//     }

//     // std::cerr << "Suggestions for " << aName << std::endl
//     //           << hidb::report(aSuggestions, "  "); // << std::endl;
//     throw hidb::HiDb::NotFound(aName, aSuggestions);

// } // Antigen::find_in_suggestions

// // ----------------------------------------------------------------------

// const hidb::AntigenSerumData<hidb::Serum>& Serum::find_in_hidb(const hidb::HiDb& aHiDb) const
// {
//     try {
//         const auto& found = aHiDb.find_serum_exactly(full_name());
//           // std::cerr << "find_in_hidb: " << full_name() << " --> " << found.most_recent_table().table_id() << " tables:" << found.number_of_tables() << std::endl;
//         return found;
//     }
//     catch (hidb::HiDb::NotFound& err) {
//         std::cerr << "ERROR: not found " << err.what() << std::endl;
//         std::cerr << hidb::report(aHiDb.find_sera(full_name()), "  ") << std::endl;
//         throw;
//     }

// } // Serum::find_in_hidb

// ----------------------------------------------------------------------

void Antigens::find_by_name(std::string aName, std::vector<size_t>& aAntigenIndices) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        if (ag->name().find(aName) != std::string::npos)
            aAntigenIndices.push_back(static_cast<size_t>(ag - begin()));
    }

} // Antigens::find_by_name

// ----------------------------------------------------------------------

size_t Sera::find_by_name_for_exact_matching(std::string aFullName) const
{
    auto found = std::find_if(begin(), end(), [&aFullName](const auto& e) -> bool { return e.full_name() == aFullName; });
    return found == end() ? static_cast<size_t>(-1) : static_cast<size_t>(found - begin());

} // Sera::find_by_name_for_exact_matching

// ----------------------------------------------------------------------

void Antigens::continents(ContinentData& aContinentData, const LocDb& aLocDb) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        const std::string location = virus_name::location(ag->name());
        const std::string continent = aLocDb.continent(location);
        aContinentData[continent].push_back(static_cast<size_t>(ag - begin()));
    }

} // Antigens::continents

// ----------------------------------------------------------------------

void Antigens::country(std::string aCountry, std::vector<size_t>& aAntigenIndices, const LocDb& aLocDb) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        const std::string location = virus_name::location(ag->name());
        const std::string country = aLocDb.country(location);
        if (country == aCountry)
            aAntigenIndices.push_back(static_cast<size_t>(ag - begin()));
    }

} // Antigens::country

// ----------------------------------------------------------------------

std::string ChartInfo::merge_text_fields(std::string ChartInfo::* aMember) const
{
    std::string result = this->*aMember;
    if (result.empty() && !mSources.empty()) {
        std::vector<std::string> data;
        for (const auto& src: mSources)
            data.push_back(src.*aMember);
        std::sort(data.begin(), data.end());
        result = string::join("+", data.begin(), std::unique(data.begin(), data.end()));
    }
    return result;

} // ChartInfo::merge_text_fields

// ----------------------------------------------------------------------

const std::string ChartInfo::date() const
{
    std::string result = mDate;
    if (result.empty() && !mSources.empty()) {
        std::vector<std::string> data;
        for (const auto& src: mSources)
            data.push_back(src.mDate);
        std::sort(data.begin(), data.end());
        result = data.front() + "-" + data.back();
    }
    return result;

} // ChartInfo::date

// ----------------------------------------------------------------------

std::string Chart::lineage() const
{
    std::set<std::string> lineages;
    for (const auto& antigen: mAntigens) {
        if (!antigen.lineage().empty())
            lineages.insert(antigen.lineage());
    }
    return string::join("+", lineages);

} // Chart::lineage

// ----------------------------------------------------------------------

void Chart::find_homologous_antigen_for_sera()
{
    for (auto& serum: mSera) {
        if (!serum.has_homologous()) { // it can be already set in .ace, e.g. manually during source excel sheet parsing
              // std::cout << serum.full_name() << std::endl;
            std::vector<std::pair<size_t, AntigenSerumMatch>> antigen_match;
            for (auto antigen = mAntigens.begin(); antigen != mAntigens.end(); ++antigen) {
                AntigenSerumMatch match{serum.match(*antigen)};
                if (match.name_match())
                    antigen_match.emplace_back(static_cast<size_t>(antigen - mAntigens.begin()), std::move(match));
            }
            switch (antigen_match.size()) {
              case 0:
                  std::cerr << "Warning: No homologous antigen (no name match at all) for " << serum.full_name() << std::endl;
                  break;
              case 1:
                  if (antigen_match.front().second.reassortant_match()) {
                      serum.set_homologous(antigen_match.front().first);
                  }
                  else {
                      std::cerr << "Warning: No homologous antigen for " << serum.full_name() << std::endl;
                      std::cerr << "    the only name match: " << mAntigens[antigen_match.front().first].full_name() << " Level:" << antigen_match.front().second << std::endl;
                  }
                  break;
              default:
                  // sort by AntigenSerumMatch but prefer reference antigens if matches are equal
                std::sort(antigen_match.begin(), antigen_match.end(), [this](const auto& a, const auto& b) -> bool { return a.second == b.second ? mAntigens[a.first].reference() > mAntigens[b.first].reference() : a.second < b.second; });
                if (antigen_match.front().second.homologous_match()) {
                    serum.set_homologous(antigen_match.front().first);
                    if (antigen_match.size() > 1 && antigen_match[0].second == antigen_match[1].second && mAntigens[antigen_match[0].first].reference() == mAntigens[antigen_match[1].first].reference()) {
                        std::cerr << "Warning: Multiple homologous antigen candidates for " << serum.full_name() << " (the first one chosen)" << std::endl;
                        for (const auto ag: antigen_match) {
                            if (ag.second != antigen_match.front().second)
                                break;
                            std::cerr << "    " << mAntigens[ag.first].full_name() << " Ref:" << mAntigens[ag.first].reference() << " Level:" << ag.second << std::endl;
                        }
                    }
                }
                else {
                    std::cerr << "Warning: No homologous antigen for " << serum.full_name() << std::endl;
                      // std::cerr << "    best match (of " << antigen_match.size() << "): " << mAntigens[antigen_match.front().first].full_name() << " Level:" << antigen_match.front().second << std::endl;
                    for (const auto ag: antigen_match) {
                        if (ag.second != antigen_match.front().second)
                            break;
                        std::cerr << "    " << mAntigens[ag.first].full_name() << " Ref:" << mAntigens[ag.first].reference() << " Level:" << ag.second << std::endl;
                    }
                }
                break;
            }
        }
    }

} // Chart::find_homologous_antigen_for_sera

// ----------------------------------------------------------------------

// Vaccines* Chart::vaccines(std::string aName, const hidb::HiDb& aHiDb) const
// {
//     Vaccines* result = new Vaccines();
//     std::vector<size_t> by_name;
//     antigens().find_by_name(aName, by_name);
//     for (size_t ag_no: by_name) {
//           // std::cerr << ag->full_name() << std::endl;
//         try {
//             const Antigen& ag = antigens()[ag_no];
//             const auto& data = ag.find_in_hidb(aHiDb);
//             std::vector<Vaccines::HomologousSerum> homologous_sera;
//             for (const auto* sd: aHiDb.find_homologous_sera(data)) {
//                 const size_t sr_no = sera().find_by_name_for_exact_matching(sd->data().name_for_exact_matching());
//                   // std::cerr << "   " << sd->data().name_for_exact_matching() << " " << (serum ? "Y" : "N") << std::endl;
//                 if (sr_no != static_cast<size_t>(-1))
//                     homologous_sera.emplace_back(sr_no, &sera()[sr_no], sd, aHiDb.charts()[sd->most_recent_table().table_id()].chart_info().date());
//             }
//             result->add(ag_no, ag, &data, std::move(homologous_sera), aHiDb.charts()[data.most_recent_table().table_id()].chart_info().date());
//         }
//         catch (hidb::HiDb::NotFound&) {
//         }
//     }
//     result->sort();
//     result->report();
//     return result;

// } // Chart::vaccines

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
