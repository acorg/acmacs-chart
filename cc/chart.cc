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
            std::cerr << "ERROR: not found and no suggestions for " << name << std::endl
                      << hidb::report(aHiDb.find_antigens(name), "  ") << std::endl;
        }
        throw;
    }

} // Antigen::find_in_hidb

// ----------------------------------------------------------------------

const hidb::AntigenSerumData<hidb::Antigen>& Antigen::find_in_suggestions(std::string aName, const hidb::AntigenRefs& aSuggestions) const
{
    const std::regex wrongly_converted{" (SECM|VIR)(-)"};
    std::smatch m;
    if (std::regex_search(aName, m, wrongly_converted)) {
        std::string name = aName;   // to avoid aName changing
        name[static_cast<size_t>(m[2].first - aName.begin())] = '0';
        for (const auto& e: aSuggestions) {
            if (e->data().full_name() == name)
                return *e;
        }
    }

    std::cerr << "Suggestions for " << aName << std::endl
              << hidb::report(aSuggestions, "  ") << std::endl;

    throw hidb::HiDb::NotFound(aName);

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
