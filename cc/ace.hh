#pragma once

#include <string>

// ----------------------------------------------------------------------

class Chart;

// ----------------------------------------------------------------------

Chart* import_chart(std::string data);
void export_chart(std::string aFilename, const Chart& aChart);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
