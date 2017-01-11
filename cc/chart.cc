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

void Antigen::find_in_hidb(const hidb::HiDb& aHiDb) const
{
    std::vector<std::string> p{name(), reassortant(), annotations().join(), passage()};
    p.erase(std::remove(p.begin(), p.end(), std::string()), p.end());
    std::string name_to_look = string::join(" ", p);
    auto r = aHiDb.find_antigens(name_to_look);
    if (r.empty())
        std::cerr << "ERROR: not found " << name_to_look << std::endl;
    std::cerr << "find_in_hidb: " << name_to_look << " --> " << r.size() << std::endl << hidb::report(r, "  ") << std::endl;

} // Antigen::find_in_hidb

// ----------------------------------------------------------------------

std::string ChartInfo::merge_text_fields(std::string ChartInfo::* aMember) const
{
    std::string result = this->*aMember;
    if (result.empty() && !mSources.empty()) {
        std::vector<std::string> data;
        for (const auto& src: mSources)
            data.push_back(src.*aMember);
        std::sort(data.begin(), data.end());
        data.erase(std::unique(data.begin(), data.end()), data.end());
        result = string::join("+", data);
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
        result = string::join("-", std::vector<std::string>{data.front(), data.back()});
    }
    return result;

} // ChartInfo::date

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
