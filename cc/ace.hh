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
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
