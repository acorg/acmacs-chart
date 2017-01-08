#include <typeinfo>
#include <functional>
#include <map>

#include "ace.hh"
#include "chart.hh"
#include "acmacs-base/read-file.hh"
#include "acmacs-base/xz.hh"

#include "json-importer.hh"
namespace jsi = json_importer;

// ----------------------------------------------------------------------
// ~/ac/acmacs/docs/ace-format.json
// ----------------------------------------------------------------------

class Ace
{
 public:
    inline Ace(Chart& aChar) : mChart(aChar) {}

    inline void indentation(const char* str, size_t length)
        {
            mIndentation.assign(str, length);
              // std::cerr << "Indentation: " << mIndentation << std::endl;
        }
    inline std::string indentation() const { return mIndentation; }
    inline void version(const char* str, size_t length)
        {
            mVersion.assign(str, length);
            if (mVersion != "acmacs-ace-v1")
                throw std::runtime_error("Unsupported data version: \"" + mVersion + "\"");
        }
    inline std::string version() const { return mVersion; }

    inline Chart& chart() { return mChart; }

 private:
    std::string mIndentation;
    std::string mVersion;
    Chart& mChart;
};

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

static jsi::data<Projection> projection_data = {
    {"C", jsi::field(&Projection::column_bases)},
    {"D", jsi::field(&Projection::disconnected)},
    {"c", jsi::field(&Projection::comment)},
    {"e", jsi::field(&Projection::stress_diff_to_stop)},
    {"f", jsi::field(&Projection::titer_multipliers)},
    {"g", jsi::field(&Projection::gradient_multipliers)},
    {"l", jsi::field(&Projection::layout)},
    {"m", jsi::field(&Projection::minimum_column_basis)},
    {"s", jsi::field(&Projection::stress)},
    {"t", jsi::field(&Projection::transformation)},
};

static jsi::data<Chart> chart_data = {
    {"P", jsi::field(&Chart::projections, projection_data)},
};

static jsi::data<Ace> ace_data = {
    {"_", jsi::field(&Ace::indentation)},
    {"  version", jsi::field(&Ace::version)},
    {"c", jsi::field(&Ace::chart, chart_data)},
};

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

Chart* import_chart(std::string buffer)
{
    if (buffer == "-")
        buffer = acmacs_base::read_stdin();
    else if (buffer[0] != '{')
        buffer = acmacs_base::read_file(buffer);
    Chart* chart = nullptr;
    if (buffer[0] == '{') {
        chart = new Chart{};
        Ace ace(*chart);
        jsi::import(buffer, ace, ace_data);
    }
    else
        throw std::runtime_error("cannot import chart: unrecognized source format");
    return chart;

} // import_chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
