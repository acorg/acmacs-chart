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
    std::cerr << "find_in_hidb: " << name_to_look << " --> " << r.size() << std::endl;

} // Antigen::find_in_hidb

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
