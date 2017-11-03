#pragma once

#include <string>
#include <vector>

// ----------------------------------------------------------------------

class Chart;
class PointStyle;
namespace acmacs { class Transformation; }

void export_chart_lispmds(std::string aFilename, const Chart& aChart);
void export_chart_lispmds(std::string aFilename, const Chart& aChart, const std::vector<PointStyle>& aPointStyles, const acmacs::Transformation& aTransformation);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
