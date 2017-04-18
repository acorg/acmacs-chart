#include <map>

#include "vaccines.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

static std::map<std::string, std::vector<Vaccine>> sVaccines = {
    {"A(H1N1)", {
            {"CALIFORNIA/7/2009",        Vaccine::Previous},
            {"MICHIGAN/45/2015",         Vaccine::Current},
        }},
    {"A(H3N2)", {
            {"BRISBANE/10/2007",         Vaccine::Previous},
            {"PERTH/16/2009",            Vaccine::Previous},
            {"VICTORIA/361/2011",        Vaccine::Previous},
            {"TEXAS/50/2012",            Vaccine::Previous},
            {"SWITZERLAND/9715293/2013", Vaccine::Previous},
            {"HONG KONG/4801/2014",      Vaccine::Current},
            {"SAITAMA/103/2014",         Vaccine::Surrogate},
            {"HONG KONG/7295/2014",      Vaccine::Surrogate},
        }},
    {"BVICTORIA", {
            {"MALAYSIA/2506/2004",       Vaccine::Previous},
            {"BRISBANE/60/2008",         Vaccine::Current},
            {"PARIS/1762/2009",          Vaccine::Current},
            {"SOUTH AUSTRALIA/81/2012",  Vaccine::Surrogate},
        }},
    {"BYAMAGATA", {
            {"FLORIDA/4/2006",           Vaccine::Previous},
            {"WISCONSIN/1/2010",         Vaccine::Previous},
            {"MASSACHUSETTS/2/2012",     Vaccine::Previous},
            {"PHUKET/3073/2013",         Vaccine::Current},
        }},
};

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

const std::vector<Vaccine>& vaccines(std::string aSubtype, std::string aLineage)
{
    return sVaccines.at(aSubtype + aLineage);

} // vaccines

// ----------------------------------------------------------------------

const std::vector<Vaccine>& vaccines(const Chart& aChart)
{
    return vaccines(aChart.chart_info().virus_type(), aChart.lineage());

} // vaccines

// ----------------------------------------------------------------------

std::string Vaccine::type_as_string() const
{
    switch (type) {
      case Previous:
          return "previous";
      case Current:
          return "current";
      case Surrogate:
          return "surrogate";
    }
    return {};

} // Vaccine::type_as_string

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
