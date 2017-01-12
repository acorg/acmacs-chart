#include <set>

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

std::string Antigen::full_name() const
{
    std::vector<std::string> p{name(), reassortant(), annotations().join(), passage()};
    return string::join(" ", p.begin(), std::remove(p.begin(), p.end(), std::string()));

} // Antigen::full_name

// ----------------------------------------------------------------------

const hidb::AntigenSerumData<hidb::Antigen>& Antigen::find_in_hidb(const hidb::HiDb& aHiDb) const
{
    try {
        const auto& found = aHiDb.find_antigen_exactly(full_name());
          // std::cerr << "find_in_hidb: " << full_name() << " --> " << found.most_recent_table().table_id() << " tables:" << found.number_of_tables() << std::endl;
        return found;
    }
    catch (hidb::HiDb::NotFound& err) {
        std::cerr << "ERROR: not found " << err.what() << std::endl;
        std::cerr << hidb::report(aHiDb.find_antigens(full_name()), "  ") << std::endl;
        throw;
    }

} // Antigen::find_in_hidb

// ----------------------------------------------------------------------

std::string Serum::full_name() const
{
    std::vector<std::string> p{name(), reassortant(), annotations().join(), serum_id()};
    return string::join(" ", p.begin(), std::remove(p.begin(), p.end(), std::string()));

} // Serum::full_name

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
