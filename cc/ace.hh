#pragma once

#include <string>
#include <vector>

namespace json_writer
{
    template <typename RW> class writer;
}
namespace jsw = json_writer;

template <typename RW> jsw::writer<RW>& operator <<(jsw::writer<RW>&, const std::vector<std::pair<std::string, std::string>>&);

#include "acmacs-base/json-writer.hh"

#include "acmacs-chart/chart.hh"

// ----------------------------------------------------------------------

Chart* import_chart(std::string data);
void export_chart(std::string aFilename, const Chart& aChart);

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const std::vector<std::pair<std::string, std::string>>& aVectorOfStringPairs)
{
    writer << jsw::start_object;
    for (const auto& e: aVectorOfStringPairs)
        writer << jsw::key(e.first) << e.second;
    writer << jsw::end_object;
    return writer;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Antigen& aAntigen)
{
    return writer << jsw::start_object
                  << jsw::key("N") << aAntigen.name()
                  << jsw::if_not_empty("D", aAntigen.date())
                  << jsw::if_not_empty("L", aAntigen.lineage())
                  << jsw::if_not_empty("P", aAntigen.passage())
                  << jsw::if_not_empty("R", aAntigen.reassortant())
                  << jsw::if_not_empty("l", aAntigen.lab_id())
                  << jsw::if_not_empty("S", aAntigen.semantic())
                  << jsw::if_not_empty("a", aAntigen.annotations())
                  << jsw::if_not_empty("c", aAntigen.clades())
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Serum& aSerum)
{
    return writer << jsw::start_object
                  << jsw::key("N") << aSerum.name()
                  << jsw::if_not_empty("I", aSerum.serum_id())
                  << jsw::if_not_empty("L", aSerum.lineage())
                  << jsw::if_not_empty("P", aSerum.passage())
                  << jsw::if_not_empty("R", aSerum.reassortant())
                  << jsw::if_not_empty("S", aSerum.semantic())
                  << jsw::if_not_empty("a", aSerum.annotations())
                  << jsw::if_non_negative("h", aSerum.homologous())
                  << jsw::if_not_empty("s", aSerum.serum_species())
                  << jsw::end_object;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
