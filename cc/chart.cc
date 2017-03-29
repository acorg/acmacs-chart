#include <set>
#include <limits>
#include <cmath>

#include "acmacs-base/virus-name.hh"
#include "acmacs-base/range.hh"
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

std::string AntigenSerum::name_abbreviated(const LocDb& aLocDb) const
{
    try {
        std::string virus_type, host, location, isolation, year, passage;
        virus_name::split(name(), virus_type, host, location, isolation, year, passage);
        return string::join("/", {aLocDb.abbreviation(location), isolation, year.substr(2)});
    }
    catch (virus_name::Unrecognized&) {
        return name();
    }

} // AntigenSerum::name_abbreviated

// ----------------------------------------------------------------------

std::string AntigenSerum::location_abbreviated(const LocDb& aLocDb) const
{
    return aLocDb.abbreviation(virus_name::location(name()));

} // AntigenSerum::location_abbreviated

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

template <typename AgSr> static void find_by_name_matching_ag_sr(const std::vector<AgSr>& aAgSr, std::string aName, std::vector<size_t>& aIndices, string_match::score_t aScoreThreshold, bool /*aVerbose*/)
{
    using Score = AntigenSerumMatchScore<AgSr>;

    std::vector<std::pair<size_t, string_match::score_t>> index_score;
    string_match::score_t score_threshold = aScoreThreshold;
    for (auto ag = aAgSr.begin(); ag != aAgSr.end(); ++ag) {
        Score score{aName, *ag, score_threshold};
        score_threshold = std::max(score.name_score(), score_threshold);
        if (score.full_name_score())
            index_score.emplace_back(static_cast<size_t>(ag - aAgSr.begin()), score.full_name_score());
    }
    std::sort(index_score.begin(), index_score.end(), [](const auto& a, const auto& b) -> bool { return a.second > b.second; });
    for (const auto& is: index_score) {
        if (is.second < index_score.front().second)
            break;
        aIndices.push_back(is.first);
        // if (aVerbose)
        //     std::cerr << "DEBUG: find_by_name_matching index:" << is.first << " score:" << is.second << std::endl;
    }
}

void Antigens::find_by_name_matching(std::string aName, std::vector<size_t>& aAntigenIndices, string_match::score_t aScoreThreshold, bool aVerbose) const
{
    find_by_name_matching_ag_sr(*this, aName, aAntigenIndices, aScoreThreshold, aVerbose);

} // Antigens::find_by_name_matching

// ----------------------------------------------------------------------

void Sera::find_by_name_matching(std::string aName, std::vector<size_t>& aSeraIndices, string_match::score_t aScoreThreshold, bool aVerbose) const
{
    find_by_name_matching_ag_sr(*this, aName, aSeraIndices, aScoreThreshold, aVerbose);

} // Sera::find_by_name_matching

// ----------------------------------------------------------------------

void Antigens::continents(ContinentData& aContinentData, const LocDb& aLocDb, bool aExcludeReference) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        if (!aExcludeReference || !ag->reference()) {
            try {
                const std::string location = virus_name::location(ag->name());
                const std::string continent = aLocDb.continent(location);
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

void Antigens::countries(CountryData& aCountries, const LocDb& aLocDb, bool aExcludeReference) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        if (!aExcludeReference || !ag->reference()) {
            try {
                const std::string location = virus_name::location(ag->name());
                const std::string country = aLocDb.country(location);
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

void Antigens::country(std::string aCountry, std::vector<size_t>& aAntigenIndices, const LocDb& aLocDb) const
{
    for (auto ag = begin(); ag != end(); ++ag) {
        try {
            const std::string location = virus_name::location(ag->name());
            const std::string country = aLocDb.country(location);
            if (country == aCountry)
                aAntigenIndices.push_back(static_cast<size_t>(ag - begin()));
        }
        catch (virus_name::Unrecognized&) {
        }
        catch (LocationNotFound&) {
        }
    }

} // Antigens::country

// ----------------------------------------------------------------------

void Antigens::reference_indices(std::vector<size_t>& aAntigenIndices) const
{
    for (auto a = begin(); a != end(); ++a) {
        if (a->reference())
            aAntigenIndices.push_back(static_cast<size_t>(a - begin()));
    }

} // Antigens::reference_indices

// ----------------------------------------------------------------------

void Antigens::test_indices(std::vector<size_t>& aAntigenIndices) const
{
    for (auto a = begin(); a != end(); ++a) {
        if (!a->reference())
            aAntigenIndices.push_back(static_cast<size_t>(a - begin()));
    }

} // Antigens::test_indices

// ----------------------------------------------------------------------

void Antigens::date_range_indices(std::string first_date, std::string after_last_date, std::vector<size_t>& aAntigenIndices) const
{
    for (auto a = begin(); a != end(); ++a) {
        const std::string date = a->date();
        if (!date.empty() && (first_date.empty() || date >= first_date) && (after_last_date.empty() || date < after_last_date))
            aAntigenIndices.push_back(static_cast<size_t>(a - begin()));
    }

} // Antigens::date_range_indices

// ----------------------------------------------------------------------

void Antigens::egg_indices(std::vector<size_t>& aAntigenIndices) const
{
    for (auto a = begin(); a != end(); ++a) {
        if (a->is_egg())
            aAntigenIndices.push_back(static_cast<size_t>(a - begin()));
    }

} // Antigens::egg_indices

// ----------------------------------------------------------------------

void Antigens::cell_indices(std::vector<size_t>& aAntigenIndices) const
{
    for (auto a = begin(); a != end(); ++a) {
        if (!a->is_egg())
            aAntigenIndices.push_back(static_cast<size_t>(a - begin()));
    }

} // Antigens::cell_indices

// ----------------------------------------------------------------------

void Antigens::reassortant_indices(std::vector<size_t>& aAntigenIndices) const
{
    for (auto a = begin(); a != end(); ++a) {
        if (a->is_reassortant())
            aAntigenIndices.push_back(static_cast<size_t>(a - begin()));
    }

} // Antigens::reassortant_indices

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

const std::string Chart::make_name() const
{
    const ChartInfo& info = chart_info();
    std::string n = info.name();
    if (n.empty())
        n = string::join({info.lab(), info.virus_type(), lineage(), info.assay(), info.rbc(), info.date()});
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

double Chart::compute_column_basis(MinimumColumnBasis aMinimumColumnBasis, size_t aSerumNo) const
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

class SerumCircleRadiusCalculationError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

double Chart::serum_circle_radius(size_t aAntigenNo, size_t aSerumNo, size_t aProjectionNo, bool aVerbose) const
{
    // if (aVerbose) std::cerr << "INFO: serum_circle_radius" << std::endl;
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
        // if (aVerbose) std::cerr << "INFO: serum_circle_radius protection_boundary_titer: " << protection_boundary_titer << std::endl;

          // sort antigen indices by antigen distance from serum, closest first
        std::vector<size_t> antigens_by_distances(Range<size_t>::begin(number_of_antigens()), Range<size_t>::end());
        std::sort(antigens_by_distances.begin(), antigens_by_distances.end(), [&titers_and_distances](size_t a, size_t b) -> bool { return titers_and_distances[a].distance < titers_and_distances[b].distance; });
        // std::cerr << "antigens_by_distances " << antigens_by_distances << std::endl;

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
                            std::cerr << "DEBUG: AG " << ag_no << " radius:" << radius << " distance:" << titers_and_distances[ag_no].distance << " prev:" << previous << " protected_outside:" << protected_outside << " not_protected_inside:" << not_protected_inside << " best_sum:" << best_sum << std::endl;
                        sum_radii += radius;
                        ++num_radii;
                    }
                    else {
                        if (aVerbose)
                            std::cerr << "======================================================================" << std::endl
                                      << "DEBUG: AG " << ag_no << " radius:" << radius << " distance:" << titers_and_distances[ag_no].distance << " prev:" << previous << " protected_outside:" << protected_outside << " not_protected_inside:" << not_protected_inside << " best_sum:" << best_sum << std::endl;
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
