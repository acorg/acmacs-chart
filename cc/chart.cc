#include <sstream>
#include <set>
#include <regex>

#include "acmacs-base/virus-name.hh"
#include "locationdb/locdb.hh"
#include "hidb/hidb.hh"

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

std::string AntigenSerum::passage_without_date() const
{
    if (mPassage.size() > 13 && mPassage[mPassage.size() - 1] == ')' && mPassage[mPassage.size() - 12] == '(' && mPassage[mPassage.size() - 13] == ' ' && mPassage[mPassage.size() - 4] == '-' && mPassage[mPassage.size() - 7] == '-')
        return std::string(mPassage, 0, mPassage.size() - 13);
    else
        return mPassage;

} // AntigenSerum::passage_without_date

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif

bool AntigenSerum::is_egg() const
{
    static std::regex egg_passage{
        R"#(E(\?|[0-9][0-9]?))#"  // passage
        R"#(( (ISOLATE|CLONE) [0-9\-]+)*)#"         // NIMR isolate and/or clone, NIMR H1pdm has CLONE 38-32
        R"#(( *\+[1-9])?)#"         // NIID has +1 at the end of passage
        R"#(( \([12][0129][0-9][0-9]-[01][0-9]-[0-3][0-9]\))?$)#" // passage date
       };
    return std::regex_search(mPassage, egg_passage) || is_reassortant(); // reassortant is always egg (2016-10-21)

} // AntigenSerum::is_egg

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

const hidb::AntigenSerumData<hidb::Antigen>& Antigen::find_in_hidb(const hidb::HiDb& aHiDb) const
{
    const std::string name = full_name();
    try {
        const auto& found = aHiDb.find_antigen_exactly(name);
          // std::cerr << "find_in_hidb: " << full_name() << " --> " << found.most_recent_table().table_id() << " tables:" << found.number_of_tables() << std::endl;
        return found;
    }
    catch (hidb::HiDb::NotFound& err) {
        if (passage() == "X?") {
            try {
                return aHiDb.find_antigen_exactly(full_name_without_passage());
            }
            catch (hidb::HiDb::NotFound&) {
            }
        }
        else if (!err.suggestions().empty()) {
            return find_in_suggestions(name, err.suggestions());
        }
        else {
            // std::cerr << "ERROR: not found and no suggestions for " << name << std::endl
            //           << hidb::report(aHiDb.find_antigens(name), "  ") << std::endl;
        }
        throw;
    }

} // Antigen::find_in_hidb

// ----------------------------------------------------------------------

const hidb::AntigenSerumData<hidb::Antigen>& Antigen::find_in_suggestions(std::string aName, const hidb::AntigenRefs& aSuggestions) const
{
    const std::regex wrongly_converted{" (SECM|VIR)(-)"};
    std::smatch m;

    if (aName.find(" DISTINCT") != std::string::npos) { // DISTINCT antigens are not stored in hidb
        throw hidb::HiDb::NotFound(aName);
    }
    else if (std::regex_search(aName, m, wrongly_converted)) {
        // std::cerr << "SECM Suggestions for " << aName << std::endl
        //           << hidb::report(aSuggestions, "  ") << std::endl;
        std::string name = aName;   // to avoid aName changing
        name[static_cast<size_t>(m[2].first - aName.begin())] = '0';
        for (const auto& e: aSuggestions) {
            if (e->data().full_name() == name)
                return *e;
        }
    }
    else if (aName[2] == ' ') {
          // some cdc names were incorrectly used before, e.g. "CO CO-9-2718" was used as "CO 9-2718"
        std::string fixed = aName.substr(0, 3) + aName.substr(0, 2) + "-" + aName.substr(3);
        // std::cerr << "FIXED: " << fixed << std::endl; // << report(*fk, "  ") << std::endl;
        const auto found = std::find_if(aSuggestions.begin(), aSuggestions.end(), [&fixed](const auto& e) -> bool { return e->data().full_name() == fixed; });
        if (found != aSuggestions.end())
            return **found;
    }

    // std::cerr << "Suggestions for " << aName << std::endl
    //           << hidb::report(aSuggestions, "  "); // << std::endl;
    throw hidb::HiDb::NotFound(aName, aSuggestions);

} // Antigen::find_in_suggestions

// ----------------------------------------------------------------------

const hidb::AntigenSerumData<hidb::Serum>& Serum::find_in_hidb(const hidb::HiDb& aHiDb) const
{
    try {
        const auto& found = aHiDb.find_serum_exactly(full_name());
          // std::cerr << "find_in_hidb: " << full_name() << " --> " << found.most_recent_table().table_id() << " tables:" << found.number_of_tables() << std::endl;
        return found;
    }
    catch (hidb::HiDb::NotFound& err) {
        std::cerr << "ERROR: not found " << err.what() << std::endl;
        std::cerr << hidb::report(aHiDb.find_sera(full_name()), "  ") << std::endl;
        throw;
    }

} // Serum::find_in_hidb

// ----------------------------------------------------------------------

bool Antigen::match_seqdb(const seqdb::Seqdb& aSeqdb) const
{
    bool matched = false;
    const seqdb::SeqdbEntry* entry = aSeqdb.find_by_name(name());
    if (entry) {
        const seqdb::SeqdbSeq* seq = entry->find_by_hi_name(full_name());
        if (seq) {
              // std::cerr << "[Seq] for " << full_name() << std::endl;
            mSeqdbEntrySeq.assign(entry, seq);
            matched = true;
        }
    }
    return matched;

} // Antigen::match_seqdb

// ----------------------------------------------------------------------

size_t Antigens::match_seqdb(const seqdb::Seqdb& aSeqdb) const
{
    size_t matched = 0;
    for (const auto& antigen: *this) {
        if (antigen.match_seqdb(aSeqdb))
            ++matched;
    }
    return matched;

} // Antigens::match_seqdb

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

Vaccines* Chart::vaccines(std::string aName, const hidb::HiDb& aHiDb) const
{
    Vaccines* result = new Vaccines();
    std::vector<size_t> by_name;
    antigens().find_by_name(aName, by_name);
    for (size_t ag_no: by_name) {
          // std::cerr << ag->full_name() << std::endl;
        try {
            const Antigen& ag = antigens()[ag_no];
            const auto& data = ag.find_in_hidb(aHiDb);
            std::vector<Vaccines::HomologousSerum> homologous_sera;
            for (const auto* sd: aHiDb.find_homologous_sera(data)) {
                const size_t sr_no = sera().find_by_name_for_exact_matching(sd->data().name_for_exact_matching());
                  // std::cerr << "   " << sd->data().name_for_exact_matching() << " " << (serum ? "Y" : "N") << std::endl;
                if (sr_no != static_cast<size_t>(-1))
                    homologous_sera.emplace_back(sr_no, &sera()[sr_no], sd, aHiDb.charts()[sd->most_recent_table().table_id()].chart_info().date());
            }
            result->add(ag_no, ag, &data, std::move(homologous_sera), aHiDb.charts()[data.most_recent_table().table_id()].chart_info().date());
        }
        catch (hidb::HiDb::NotFound&) {
        }
    }
    result->sort();
    result->report();
    return result;

} // Chart::vaccines

// ----------------------------------------------------------------------

inline bool Vaccines::Entry::operator < (const Vaccines::Entry& a) const
{
    const auto a_nt = a.antigen_data->number_of_tables(), t_nt = antigen_data->number_of_tables();
    return t_nt == a_nt ? most_recent_table_date > a.most_recent_table_date : t_nt > a_nt;
}

// ----------------------------------------------------------------------

bool Vaccines::HomologousSerum::operator < (const Vaccines::HomologousSerum& a) const
{
    bool result = true;
    if (serum->serum_species() == "SHEEP") { // avoid using sheep serum as homologous (NIMR)
        result = false;
    }
    else {
        const auto s_nt = a.serum_data->number_of_tables(), t_nt = serum_data->number_of_tables();
        result = t_nt == s_nt ? most_recent_table_date > a.most_recent_table_date : t_nt > s_nt;
    }
    return result;

} // Vaccines::HomologousSerum::operator <

// ----------------------------------------------------------------------

size_t Vaccines::HomologousSerum::number_of_tables() const
{
    return serum_data->number_of_tables();

} // Vaccines::HomologousSerum::number_of_tables

// ----------------------------------------------------------------------

void Vaccines::add(size_t aAntigenIndex, const Antigen& aAntigen, const hidb::AntigenSerumData<hidb::Antigen>* aAntigenData, std::vector<HomologousSerum>&& aSera, std::string aMostRecentTableDate)
{
    if (aAntigen.is_reassortant())
        mReassortant.emplace_back(aAntigenIndex, &aAntigen, aAntigenData, std::move(aSera), aMostRecentTableDate);
    else if (aAntigen.is_egg())
        mEgg.emplace_back(aAntigenIndex, &aAntigen, aAntigenData, std::move(aSera), aMostRecentTableDate);
    else
        mCell.emplace_back(aAntigenIndex, &aAntigen, aAntigenData, std::move(aSera), aMostRecentTableDate);

} // Vaccines::add

// ----------------------------------------------------------------------

void Vaccines::sort()
{
    std::sort(mCell.begin(), mCell.end());
    std::sort(mEgg.begin(), mEgg.end());
    std::sort(mReassortant.begin(), mReassortant.end());

} // Vaccines::sort

// ----------------------------------------------------------------------

std::string Vaccines::report() const
{
    std::ostringstream out;
    auto entry_report = [&out](const auto& entry) {
        out << "  " << entry.antigen->full_name() << " tables:" << entry.antigen_data->number_of_tables() << " recent:" << entry.antigen_data->most_recent_table().table_id() << std::endl;
        for (const auto& hs: entry.homologous_sera)
            out << "    " << hs.serum->full_name() << " tables:" << hs.serum_data->number_of_tables() << " recent:" << hs.serum_data->most_recent_table().table_id() << std::endl;
    };

    if (!mCell.empty()) {
        out << "CELL" << std::endl;
        std::for_each(mCell.begin(), mCell.end(), entry_report);
    }

    if (!mEgg.empty()) {
        out << "EGG" << std::endl;
        std::for_each(mEgg.begin(), mEgg.end(), entry_report);
    }

    if (!mReassortant.empty()) {
        out << "REASSORTANT" << std::endl;
        std::for_each(mReassortant.begin(), mReassortant.end(), entry_report);
    }
    return out.str();

} // Vaccines::report

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
