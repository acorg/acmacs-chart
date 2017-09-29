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
#include "acmacs-base/timeit.hh"

// ----------------------------------------------------------------------

class AceChartReadError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

Chart* import_chart(std::string data, report_time timer = report_time::No);
void export_chart(std::string aFilename, const Chart& aChart, report_time timer = report_time::No);
// void export_chart(std::string aFilename, const Chart& aChart, const std::vector<PointStyle>& aPointStyles);

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
