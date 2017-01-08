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

using ASS = void (Antigen::*)(const char*, size_t);

template <typename Parent> inline std::shared_ptr<jsi::readers::makers::Base<Parent>> field_annotations()
{
    using Accessor = Annotations& (Parent::*)();
    Accessor accessor = &Parent::annotations;
    using Storer = decltype(jsi::storers::type_detector<jsi::storers::ArrayElement<std::string>>(std::string()));
    return std::make_shared<jsi::readers::makers::ArrayOfValuesAccessor<Parent, std::string, decltype(accessor), Storer>>(accessor);
}

static jsi::data<Antigen> antigen_data = {
    {"N", jsi::field(static_cast<ASS>(&Antigen::name))},
    {"D", jsi::field(&Antigen::date)},
    {"L", jsi::field(static_cast<ASS>(&Antigen::lineage))},
    {"P", jsi::field(static_cast<ASS>(&Antigen::passage))},
    {"R", jsi::field(static_cast<ASS>(&Antigen::reassortant))},
    {"l", jsi::field(&Antigen::lab_id)},
    {"S", jsi::field(static_cast<ASS>(&Antigen::semantic))},
    {"a", field_annotations<Antigen>()},
    {"c", jsi::field(&Antigen::clades)},
};

using SSS = void (Serum::*)(const char*, size_t);

static jsi::data<Serum> serum_data = {
    {"N", jsi::field(static_cast<SSS>(&Serum::name))},
    {"L", jsi::field(static_cast<SSS>(&Serum::lineage))},
    {"P", jsi::field(static_cast<SSS>(&Serum::passage))},
    {"R", jsi::field(static_cast<SSS>(&Serum::reassortant))},
    {"I", jsi::field(&Serum::serum_id)},
    {"S", jsi::field(static_cast<SSS>(&Serum::semantic))},
    {"h", jsi::field(&Serum::homologous)},
    {"a", field_annotations<Serum>()},
    {"s", jsi::field(&Serum::serum_species)},
};

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
    {"U", jsi::field(&Projection::unmovable)},
    {"u", jsi::field(&Projection::unmovable_in_last_dimension)},
    {"d", jsi::field(&Projection::dodgy_titer_is_regular)},
};

static jsi::data<Chart> chart_data = {
    {"P", jsi::field(&Chart::projections, projection_data)},
    {"a", jsi::field(&Chart::antigens, antigen_data)},
    {"s", jsi::field(&Chart::sera, serum_data)},
    // {"t", jsi::field(&Chart::titers, titers_data)},
    {"C", jsi::field(&Chart::column_bases)},
    // {"i", jsi::field(&Chart::chart_info, chart_info_data)},
    // {"p", jsi::field(&Chart::plot_spec, plot_spec_data)},
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
