#include <set>
#include <regex>

#include "chart.hh"
#include "hidb/hidb.hh"

// ----------------------------------------------------------------------

Chart::~Chart()
{
}

// ----------------------------------------------------------------------

AntigenSerum::~AntigenSerum()
{
}

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

    std::cerr << "Suggestions for " << aName << std::endl
              << hidb::report(aSuggestions, "  "); // << std::endl;
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


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
