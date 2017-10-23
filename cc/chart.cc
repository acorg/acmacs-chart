#include <set>
#include <limits>
#include <cmath>

#include "acmacs-base/virus-name.hh"
#include "acmacs-base/range.hh"
#include "acmacs-base/enumerate.hh"

#include "chart.hh"

#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif

// ----------------------------------------------------------------------

static inline std::string name_abbreviated(std::string aName)
{
    try {
        std::string virus_type, host, location, isolation, year, passage;
        virus_name::split(aName, virus_type, host, location, isolation, year, passage);
        return string::join("/", {get_locdb().abbreviation(location), isolation, year.substr(2)});
    }
    catch (virus_name::Unrecognized&) {
        return aName;
    }

} // name_abbreviated

// ----------------------------------------------------------------------

static AntigenSerumMatch antigen_serum_match_passage(const AntigenSerumBase& left, const AntigenSerumBase& right)
{
    AntigenSerumMatch match;
    if (left.has_passage() && right.has_passage()) {
        if (left.passage() != right.passage()) {
            match.add(AntigenSerumMatch::PassageMismatch);
            if (left.passage_without_date() != right.passage_without_date()) {
                match.add(AntigenSerumMatch::PassageWithoutDateMismatch);
                  // std::cerr << "  ?egg " << is_egg() << "  " << full_name() << std::endl;
                  // std::cerr << "  ?egg " << aNother.is_egg() << "  " << aNother.full_name() << std::endl;
                if (left.is_egg() != right.is_egg()) {
                    match.add(AntigenSerumMatch::EggCellMismatch);
                }
            }
        }
    }
    else {
        match.add(AntigenSerumMatch::EggCellUnknown);
    }
    return match;

} // antigen_serum_match_passage

// ----------------------------------------------------------------------

static AntigenSerumMatch antigen_serum_match(const AntigenSerumBase& left, const AntigenSerumBase& right)
{
      // fields not used for matching
      // lineage, semantic-attributes, annotations

    AntigenSerumMatch match;
    if (!left.distinct() && !right.distinct() && left.name() == right.name()) {
        if (left.reassortant() == right.reassortant()) {
            match.add(left.match_passage(right));
        }
        else {
            match.add(AntigenSerumMatch::ReassortantMismatch);
        }
    }
    else {
        match.add(AntigenSerumMatch::NameMismatch);
    }
    return match;

} // antigen_serum_match


// ----------------------------------------------------------------------

std::string Antigen::name_abbreviated() const
{
    return ::name_abbreviated(name());

} // Antigen::name_abbreviated

std::string Serum::name_abbreviated() const
{
    return ::name_abbreviated(name());

} // Serum::name_abbreviated

// ----------------------------------------------------------------------

std::string Antigen::location_abbreviated() const
{
    return get_locdb().abbreviation(virus_name::location(name()));

} // Antigen::location_abbreviated

std::string Serum::location_abbreviated() const
{
    return get_locdb().abbreviation(virus_name::location(name()));

} // Serum::location_abbreviated

// ----------------------------------------------------------------------

AntigenSerumMatch Antigen::match(const Antigen& aNother) const
{
      // fields not used for matching
      // date, clades, lab_id

    AntigenSerumMatch m = antigen_serum_match(*this, aNother);
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

AntigenSerumMatch Antigen::match_passage(const AntigenSerumBase& aNother) const
{
    return antigen_serum_match_passage(*this, aNother);

} // Antigen::match_passage

// ----------------------------------------------------------------------

AntigenSerumMatch Serum::match(const Serum& aNother) const
{
      // fields not used for matching
      // homologous

    AntigenSerumMatch m = antigen_serum_match(*this, aNother);
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

AntigenSerumMatch Serum::match(const Antigen& aNother) const
{
    AntigenSerumMatch m = antigen_serum_match(*this, aNother);
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

// ----------------------------------------------------------------------

AntigenSerumMatch Serum::match_passage(const AntigenSerumBase& aNother) const
{
    AntigenSerumMatch match;
    if (!has_passage() && aNother.has_passage()) {
        if (is_egg() != aNother.is_egg()) {
            match.add(AntigenSerumMatch::EggCellMismatch);
        }
    }
    else {
        match = antigen_serum_match_passage(*this, aNother);
    }
    return match;

} // Serum::match_passage

// ----------------------------------------------------------------------

template <typename AgSr> void AntigensSera<AgSr>::find_by_name_matching(std::string aName, Indices& aIndices, string_match::score_t aScoreThreshold, bool aVerbose) const
{
    using Score = AntigenSerumMatchScore<AgSr>;

    std::vector<std::pair<size_t, string_match::score_t>> index_score;
    string_match::score_t score_threshold = aScoreThreshold;
    for (auto ag = begin(); ag != end(); ++ag) {
        Score score{aName, *ag, score_threshold};
        score_threshold = std::max(score.name_score(), score_threshold);
        if (score.full_name_score())
            index_score.emplace_back(static_cast<size_t>(ag - begin()), score.full_name_score());
    }
    std::sort(index_score.begin(), index_score.end(), [](const auto& a, const auto& b) -> bool { return a.second > b.second; });
    for (const auto& is: index_score) {
        if (is.second < index_score.front().second)
            break;
          // if name contains CELL, EGG, WILDTYPE - match only corresponding passage/reassortant
        if (! ((aName.find("CELL") != std::string::npos && at(is.first).is_egg()) || (aName.find("EGG") != std::string::npos && !at(is.first).is_egg()) || (aName.find("WILDTYPE") != std::string::npos && at(is.first).is_reassortant()))) {
            aIndices.push_back(is.first);
            if (aVerbose)
                std::cerr << "DEBUG: find_by_name_matching \"" << aName << "\" --> " << is.first << " \"" << at(is.first).full_name() << "\" egg:" << at(is.first).is_egg() << " score:" << is.second << std::endl;
        }
    }

} // AntigensSera<AgSr>::find_by_name_matching

// ----------------------------------------------------------------------

template <typename AgSr> void AntigensSera<AgSr>::filter_country(Indices& aIndices, std::string aCountry) const
{
    auto not_in_country = [&](const auto& entry) -> bool {
        try {
            return get_locdb().country(virus_name::location(entry.name())) != aCountry;
        }
        catch (virus_name::Unrecognized&) {
        }
        catch (LocationNotFound&) {
        }
        return true;
    };
    remove(aIndices, not_in_country);

} // AntigensSera<AgSr>::filter_country

// ----------------------------------------------------------------------

template <typename AgSr> class ContinentToIndices : public std::map<std::string, std::vector<size_t>>
{
 public:
    ContinentToIndices(const AntigensSera<AgSr>& aAgSr)
        {
            auto add = [this](size_t aIndex, std::string aContinent) {
                const auto [iter, added] = this->emplace(aContinent, mapped_type{});
                iter->second.push_back(aIndex);
            };

            const auto& locdb = get_locdb(report_time::Yes);
            for (const auto [index, entry]: acmacs::enumerate(aAgSr)) {
                try {
                    add(index, locdb.continent(virus_name::location(entry.name())));
                }
                catch (std::exception&) {
                    add(index, "UNKNOWN");
                }
            }
        }

}; // class ContinentToIndices<>

template <typename AgSr> void AntigensSera<AgSr>::filter_continent(Indices& aIndices, std::string aContinent) const
{
    static const ContinentToIndices<AgSr> continent_to_indices(*this);
    const auto continent_indices = continent_to_indices.find(aContinent);
    if (continent_indices == continent_to_indices.end()) {
        aIndices.clear();
    }
    else {
        auto not_in_continent = [&](size_t aIndex) -> bool { return std::find(std::begin(continent_indices->second), std::end(continent_indices->second), aIndex) == std::end(continent_indices->second); };
        aIndices.erase(std::remove_if(aIndices.begin(), aIndices.end(), not_in_continent), aIndices.end());
    }

} // AntigensSera<AgSr>::filter_continent

// template <typename AgSr> void AntigensSera<AgSr>::filter_continent(Indices& aIndices, std::string aContinent) const
// {
//     auto not_in_continent = [&](const auto& entry) -> bool {
//         try {
//             return get_locdb(report_time::Yes).continent(virus_name::location(entry.name())) != aContinent;
//         }
//         catch (virus_name::Unrecognized&) {
//         }
//         catch (LocationNotFound&) {
//         }
//         return true;
//     };
//     remove(aIndices, not_in_continent);

// } // AntigensSera<AgSr>::filter_continent

// ----------------------------------------------------------------------

template class AntigensSera<Antigen>;
template class AntigensSera<Serum>;

// ----------------------------------------------------------------------

void Antigens::find_by_lab_id(std::string aLabId, Antigens::Indices& aAntigenIndices) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        if (ag->has_lab_id(aLabId))
            aAntigenIndices.push_back(static_cast<size_t>(ag - begin()));
    }

} // Antigens::find_by_lab_id

// ----------------------------------------------------------------------

void Antigens::continents(ContinentData& aContinentData, bool aExcludeReference) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        if (!aExcludeReference || !ag->reference()) {
            try {
                const std::string location = virus_name::location(ag->name());
                const std::string continent = get_locdb().continent(location);
                aContinentData[continent].push_back(static_cast<size_t>(ag - begin()));
            }
            catch (virus_name::Unrecognized&) {
            }
            catch (LocationNotFound&) {
            }
        }
    }

} // Antigens::continents

// ----------------------------------------------------------------------

void Antigens::countries(CountryData& aCountries, bool aExcludeReference) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        if (!aExcludeReference || !ag->reference()) {
            try {
                const std::string location = virus_name::location(ag->name());
                const std::string country = get_locdb().country(location);
                aCountries[country].push_back(static_cast<size_t>(ag - begin()));
            }
            catch (virus_name::Unrecognized&) {
            }
            catch (LocationNotFound&) {
            }
        }
    }

} // Antigens::countries

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

const std::string ChartInfo::make_name() const
{
    std::string n = name();
    if (n.empty())
        n = string::join({lab(), virus_type(), assay(), rbc(), date()});
    return n;

} // ChartInfo::make_name

// ----------------------------------------------------------------------

const std::string Chart::make_name(size_t aProjectionNo) const
{
    const auto& info = chart_info();
    std::string n = info.make_name();
    if (aProjectionNo != static_cast<size_t>(-1) && aProjectionNo < projections().size()) {
        n += " >=" + projection(aProjectionNo).minimum_column_basis().str();
        n += " " + std::to_string(projection(aProjectionNo).stress());
    }
    return n;

} // Chart::make_name

// ----------------------------------------------------------------------

Titer ChartTiters::get(size_t ag_no, size_t sr_no) const
{
    std::string result = "*";
    if (!mList.empty()) {
        result = mList[ag_no][sr_no];
    }
    else if (!mDict.empty()) {
        const auto& ag = mDict[ag_no];
        const std::string sr_no_s = std::to_string(sr_no);
        auto entry = std::find_if(ag.begin(), ag.end(), [&sr_no_s](const auto& e) -> bool { return e.first == sr_no_s; });
        if (entry != ag.end())
            result = entry->second;
    }
    return result;

} // ChartTiters::get

// ----------------------------------------------------------------------

Titer ChartTiters::max_for_serum(size_t sr_no) const
{
    Titer result;
    for (size_t ag_no = 0; ag_no < number_of_antigens(); ++ag_no) {
        const Titer titer = get(ag_no, sr_no);
        if (titer.value() > result.value())
            result = titer;
    }
    return result;

} // ChartTiters::max_for_serum

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
                      serum.add_homologous(antigen_match.front().first);
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
                    for (const auto& match: antigen_match) {
                        if (match.second == antigen_match.front().second)
                            serum.add_homologous(match.first);
                    }
                    // if (antigen_match.size() > 1 && antigen_match[0].second == antigen_match[1].second && mAntigens[antigen_match[0].first].reference() == mAntigens[antigen_match[1].first].reference()) {
                    //     std::cerr << "Warning: Multiple homologous antigen candidates for " << serum.full_name() << " (the first one chosen)" << std::endl;
                    //     for (const auto ag: antigen_match) {
                    //         if (ag.second != antigen_match.front().second)
                    //             break;
                    //         std::cerr << "    " << mAntigens[ag.first].full_name() << " Ref:" << mAntigens[ag.first].reference() << " Level:" << ag.second << std::endl;
                    //     }
                    // }
                }
                else {
                    std::cerr << "Warning: No homologous antigen for " << serum.full_name() << std::endl;
                      // std::cerr << "    best match (of " << antigen_match.size() << "): " << mAntigens[antigen_match.front().first].full_name() << " Level:" << antigen_match.front().second << std::endl;
                    for (const auto& ag: antigen_match) {
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

double Chart::compute_column_basis(const MinimumColumnBasisBase& aMinimumColumnBasis, size_t aSerumNo) const
{
    size_t max_titer = 0;
    for (size_t ag_no = 0; ag_no < number_of_antigens(); ++ag_no) {
        const Titer titer = titers().get(ag_no, aSerumNo);
        if (!titer.is_dont_care()) {
            size_t value = titer.value();
            if (titer.is_more_than())
                value *= 2;
            if (value > max_titer)
                max_titer = value;
        }
    }
    max_titer = std::max(max_titer, static_cast<size_t>(aMinimumColumnBasis));
    return std::log2(max_titer / 10.0);

} // Chart::compute_column_basis

// ----------------------------------------------------------------------

class TiterDistance
{
 public:
    inline TiterDistance(Titer aTiter, double aColumnBase, double aDistance)
        : titer(aTiter), similarity(aTiter.is_dont_care() ? 0.0 : (aTiter.similarity() + (aTiter.is_more_than() ? 1.0 : 0.0))),
          final_similarity(std::min(aColumnBase, similarity)), distance(aDistance) {}
    inline TiterDistance() : similarity(0), final_similarity(0), distance(std::numeric_limits<double>::quiet_NaN()) {}
    inline operator bool() const { return !titer.is_dont_care(); }

    Titer titer;
    double similarity;
    double final_similarity;
    double distance;
};

inline std::ostream& operator << (std::ostream& out, const TiterDistance& td)
{
    if (td)
        return out << "t:" << td.titer << " s:" << td.similarity << " f:" << td.final_similarity << " d:" << td.distance << std::endl;
    else
        return out << "dont-care" << std::endl;
}

class SerumCircleRadiusCalculationError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

double Chart::serum_circle_radius(size_t aAntigenNo, size_t aSerumNo, size_t aProjectionNo, bool aVerbose) const
{
    if (aVerbose)
        std::cerr << "DEBUG: serum_circle_radius for [sr:" << aSerumNo << ' ' << serum(aSerumNo).full_name() << "] [ag:" << aAntigenNo << ' ' << antigen(aAntigenNo).full_name() << ']' << std::endl;
    try {
        const auto& layout = projection(aProjectionNo).layout();
        const double cb = column_basis(aProjectionNo, aSerumNo);
        std::vector<TiterDistance> titers_and_distances(number_of_antigens());
        size_t max_titer_for_serum_ag_no = 0;
        for (size_t ag_no = 0; ag_no < number_of_antigens(); ++ag_no) {
            const Titer titer = titers().get(ag_no, aSerumNo);
            if (!titer.is_dont_care()) {
                  // TODO: antigensSeraTitersMultipliers (acmacs/plot/serum_circle.py:113)
                titers_and_distances[ag_no] = TiterDistance(titer, cb, layout.distance(ag_no, aSerumNo + number_of_antigens()));
                if (max_titer_for_serum_ag_no != ag_no && titers_and_distances[max_titer_for_serum_ag_no].final_similarity < titers_and_distances[ag_no].final_similarity)
                    max_titer_for_serum_ag_no = ag_no;
            }
            else if (ag_no == aAntigenNo)
                throw SerumCircleRadiusCalculationError("no homologous titer");
        }
        const double protection_boundary_titer = titers_and_distances[aAntigenNo].final_similarity - 2.0;
        if (protection_boundary_titer < 1.0)
            throw SerumCircleRadiusCalculationError("titer is too low, protects everything");
        // if (aVerbose) std::cerr << "DEBUG: titers_and_distances: " << titers_and_distances << std::endl;
        if (aVerbose) std::cerr << "DEBUG: serum_circle_radius protection_boundary_titer: " << protection_boundary_titer << std::endl;

          // sort antigen indices by antigen distance from serum, closest first
        auto antigens_by_distances_sorting = [&titers_and_distances](size_t a, size_t b) -> bool {
            const auto& aa = titers_and_distances[a];
            if (aa) {
                const auto& bb = titers_and_distances[b];
                return bb ? aa.distance < bb.distance : true;
            }
            else
                return false;
        };
        std::vector<size_t> antigens_by_distances(acmacs::incrementer<size_t>::begin(0), acmacs::incrementer<size_t>::end(number_of_antigens()));
        std::sort(antigens_by_distances.begin(), antigens_by_distances.end(), antigens_by_distances_sorting);
          //if (aVerbose) std::cerr << "DEBUG: antigens_by_distances " << antigens_by_distances << std::endl;

        constexpr const size_t None = static_cast<size_t>(-1);
        size_t best_sum = None;
        size_t previous = None;
        double sum_radii = 0;
        size_t num_radii = 0;
        for (size_t ag_no: antigens_by_distances) {
            if (!titers_and_distances[ag_no])
                break;
            if (!std::isnan(titers_and_distances[ag_no].distance)) {
                const double radius = previous == None ? titers_and_distances[ag_no].distance : (titers_and_distances[ag_no].distance + titers_and_distances[previous].distance) / 2.0;
                size_t protected_outside = 0, not_protected_inside = 0; // , protected_inside = 0, not_protected_outside = 0;
                for (const auto& protection_data: titers_and_distances) {
                    if (protection_data) {
                        const bool inside = protection_data.distance <= radius;
                        const bool protectd = protection_data.titer.is_regular() ? protection_data.final_similarity >= protection_boundary_titer : protection_data.final_similarity > protection_boundary_titer;
                        if (protectd && !inside)
                            ++protected_outside;
                        else if (!protectd && inside)
                            ++not_protected_inside;
                    }
                }
                const size_t summa = protected_outside + not_protected_inside;
                if (best_sum == None || best_sum >= summa) { // if sums are the same, choose the smaller radius (found earlier)
                    if (best_sum == summa) {
                        if (aVerbose)
                            std::cerr << "DEBUG: AG " << ag_no << " radius:" << radius << " distance:" << titers_and_distances[ag_no].distance << " prev:" << static_cast<int>(previous) << " protected_outside:" << protected_outside << " not_protected_inside:" << not_protected_inside << " best_sum:" << best_sum << std::endl;
                        sum_radii += radius;
                        ++num_radii;
                    }
                    else {
                        if (aVerbose)
                            std::cerr << "======================================================================" << std::endl
                                      << "DEBUG: AG " << ag_no << " radius:" << radius << " distance:" << titers_and_distances[ag_no].distance << " prev:" << static_cast<int>(previous) << " protected_outside:" << protected_outside << " not_protected_inside:" << not_protected_inside << " best_sum:" << best_sum << std::endl;
                        sum_radii = radius;
                        num_radii = 1;
                        best_sum = summa;
                    }
                }
                  // std::cerr << "AG " << ag_no << " radius:" << radius << " protected_outside:" << protected_outside << " not_protected_inside:" << not_protected_inside << " best_sum:" << best_sum << std::endl;
                previous = ag_no;
            }
        }
        return sum_radii / num_radii;
    }
    catch (SerumCircleRadiusCalculationError& err) {
        std::cerr << "WARNING: " << "Cannot calculate serum projection radius for sr " << aSerumNo << " ag " << aAntigenNo << ": " << err.what() << std::endl;
        return -1;
    }

} // Chart::serum_circle_radius

// ----------------------------------------------------------------------

void Chart::serum_coverage(size_t aAntigenNo, size_t aSerumNo, std::vector<size_t>& aWithin4Fold, std::vector<size_t>& aOutside4Fold) const
{
    const Titer homologous_titer = titers().get(aAntigenNo, aSerumNo);
    if (!homologous_titer.is_regular())
        throw std::runtime_error("serum_coverage: cannot handle non-regular homologous titer: " + homologous_titer);
    const double titer_threshold = homologous_titer.similarity() - 2;
    if (titer_threshold <= 0)
        throw std::runtime_error("serum_coverage: homologous titer is too low: " + homologous_titer);
    for (size_t ag_no = 0; ag_no < number_of_antigens(); ++ag_no) {
        const Titer titer = titers().get(ag_no, aSerumNo);
        double value = -1;
        if (titer.is_more_than())
            value = titer.similarity() + 1;
        else if (!titer.is_dont_care())
            value = titer.similarity();
        if (value >= titer_threshold)
            aWithin4Fold.push_back(ag_no);
        else if (value >= 0 && value < titer_threshold)
            aOutside4Fold.push_back(ag_no);
    }
    if (aWithin4Fold.empty())
        throw std::runtime_error("serum_coverage: no antigens within 4fold from homologous titer (for serum coverage)"); // BUG? at least homologous antigen must be there!

} // Chart::serum_coverage

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
