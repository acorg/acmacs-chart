#include "ace.hh"
#include "chart.hh"
#include "acmacs-base/read-file.hh"
#include "acmacs-base/xz.hh"

#include "acmacs-base/json-writer.hh"
#include "acmacs-base/json-importer.hh"
namespace jsi = json_importer;

// ----------------------------------------------------------------------
// ~/ac/acmacs/docs/ace-format.json
// ----------------------------------------------------------------------

static const char* ACE_DUMP_VERSION = "acmacs-ace-v1";

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
            if (mVersion != ACE_DUMP_VERSION)
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

static jsi::data<Antigen> antigen_data = {
    {"N", jsi::field<Antigen>(&Antigen::name)},
    {"D", jsi::field(&Antigen::date)},
    {"L", jsi::field<Antigen>(&Antigen::lineage)},
    {"P", jsi::field<Antigen>(&Antigen::passage)},
    {"R", jsi::field<Antigen>(&Antigen::reassortant)},
    {"l", jsi::field(&Antigen::lab_id)},
    {"S", jsi::field<Antigen>(&Antigen::semantic)},
    {"a", jsi::field<std::string, Antigen>(&Antigen::annotations)},
    {"c", jsi::field(&Antigen::clades)},
};

static jsi::data<Serum> serum_data = {
    {"N", jsi::field<Serum>(&Serum::name)},
    {"L", jsi::field<Serum>(&Serum::lineage)},
    {"P", jsi::field<Serum>(&Serum::passage)},
    {"R", jsi::field<Serum>(&Serum::reassortant)},
    {"I", jsi::field(&Serum::serum_id)},
    {"S", jsi::field<Serum>(&Serum::semantic)},
    {"h", jsi::field(&Serum::homologous)},
    {"a", jsi::field<std::string, Serum>(&Serum::annotations)},
    {"s", jsi::field(&Serum::serum_species)},
};

static jsi::data<ChartInfo> chart_info_data = {
    {"v", jsi::field(&ChartInfo::virus)},
    {"V", jsi::field(&ChartInfo::virus_type)},
    {"A", jsi::field(&ChartInfo::assay)},
    {"D", jsi::field(&ChartInfo::date)},
    {"N", jsi::field(&ChartInfo::name)},
    {"l", jsi::field(&ChartInfo::lab)},
    {"r", jsi::field(&ChartInfo::rbc)},
    {"s", jsi::field(&ChartInfo::subset)},
    {"T", jsi::field(&ChartInfo::type)},
    {"S", jsi::field(&ChartInfo::sources, chart_info_data)},
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

static jsi::data<LabelStyle> label_style_data = {
    {"+", jsi::field(&LabelStyle::shown)},
    {"p", jsi::field(&LabelStyle::position)},
    {"t", jsi::field(&LabelStyle::text)},
    {"f", jsi::field(&LabelStyle::face)},
    {"s", jsi::field(&LabelStyle::slant)},
    {"w", jsi::field(&LabelStyle::weight)},
    {"s", jsi::field(&LabelStyle::size)},
    {"c", jsi::field(&LabelStyle::color)},
    {"r", jsi::field(&LabelStyle::rotation)},
    {"i", jsi::field(&LabelStyle::interline)},
};

static jsi::data<ChartPlotSpecStyle> plot_spec_style_data = {
    {"+", jsi::field(&ChartPlotSpecStyle::shown)},
    {"F", jsi::field(&ChartPlotSpecStyle::fill_color)},
    {"O", jsi::field(&ChartPlotSpecStyle::outline_color)},
    {"o", jsi::field(&ChartPlotSpecStyle::outline_width)},
    {"S", jsi::field(&ChartPlotSpecStyle::shape)},
    {"s", jsi::field(&ChartPlotSpecStyle::size)},
    {"r", jsi::field(&ChartPlotSpecStyle::rotation)},
    {"a", jsi::field(&ChartPlotSpecStyle::aspect)},
    {"l", jsi::field(&ChartPlotSpecStyle::label, label_style_data)},
};

static jsi::data<ChartPlotSpec> plot_spec_data = {
    {"d", jsi::field(&ChartPlotSpec::drawing_order)},
    // {"E", jsi::field(&ChartPlotSpec::error_lines_positive)},
    // {"e", jsi::field(&ChartPlotSpec::error_lines_negative)},
    // {"g", jsi::field(&ChartPlotSpec::grid)},
    {"p", jsi::field(&ChartPlotSpec::style_for_point)},
    {"P", jsi::field(&ChartPlotSpec::styles, plot_spec_style_data)},
    // {"l", jsi::field(&ChartPlotSpec::style_for_procrustes_line)},
    // {"L", jsi::field(&ChartPlotSpec::procrustes_line_styles, procrustes_line_styles_data)},
    {"s", jsi::field(&ChartPlotSpec::shown_on_all)},
      // {"t", jsi::field(&ChartPlotSpec::title_style, title_style_data)},
};

// ----------------------------------------------------------------------

// std::vector< std::vector< std::pair<std::string, std::string>>>>
// table        row                    column       titer

class TiterDictStorer : public jsi::StorerBase
{
 public:
    using Base = jsi::StorerBase;

    inline TiterDictStorer(ChartTiters::Dict& aTarget, bool aStarted = false) : mTarget(aTarget), mStarted(aStarted) {}

    inline virtual Base* StartArray()
        {
            if (mStarted)
                throw Base::Failure(typeid(*this).name() + std::string(": unexpected StartArray event"));
            mTarget.clear(); // erase all old elements
            mStarted = true;
            return nullptr;
        }

    inline virtual Base* EndArray()
        {
            throw Base::Pop();
        }

    inline virtual Base* StartObject()
        {
            if (!mStarted)
                throw Base::Failure(typeid(*this).name() + std::string(": unexpected StartObject event"));
            mTarget.emplace_back(); // add row
            return nullptr;
        }

    inline virtual Base* EndObject()
        {
            return nullptr;
        }

    inline virtual Base* Key(const char* str, rapidjson::SizeType length)
        {
            mTarget.back().emplace_back(std::string{str, length}, std::string{}); // add column and empty titer
            return nullptr;
        }

    inline virtual Base* String(const char* str, rapidjson::SizeType length)
        {
            if (!mTarget.back().back().second.empty())
                throw Base::Failure(typeid(*this).name() + std::string(": unexpected String event: titer is there: ") + mTarget.back().back().second);
            mTarget.back().back().second.assign(str, length);
            return nullptr;
        }

 private:
    ChartTiters::Dict& mTarget;
    bool mStarted;

}; // class TiterDictStorer

// std::vector< std::vector< std::vector< std::pair<std::string, std::string>>>>
// layers       layer        row                    column       titer

class TiterLayersStorer : public jsi::StorerBase
{
 public:
    using Base = jsi::StorerBase;

    inline TiterLayersStorer(ChartTiters::Layers& aTarget) : mTarget(aTarget), mStarted(false) {}

    inline virtual Base* StartArray()
        {
            Base* result = nullptr;
            if (!mStarted) {
                mTarget.clear(); // erase all old elements
                mStarted = true;
            }
            else {
                mTarget.emplace_back(); // add layer
                result = new TiterDictStorer(mTarget.back(), true);
            }
            return result;
        }

    inline virtual Base* EndArray()
        {
            throw Base::Pop();
        }

 private:
    ChartTiters::Layers& mTarget;
    bool mStarted;

}; // class TiterLayersStorer


static jsi::data<ChartTiters> titers_data = {
    {"L", jsi::field<TiterLayersStorer, ChartTiters, ChartTiters::Layers>(&ChartTiters::layers)},
    {"l", jsi::field(&ChartTiters::list)},
    {"d", jsi::field<TiterDictStorer, ChartTiters, ChartTiters::Dict>(&ChartTiters::dict)},
};

// ----------------------------------------------------------------------

static jsi::data<Chart> chart_data = {
    {"C", jsi::field(&Chart::column_bases)},
    {"P", jsi::field(&Chart::projections, projection_data)},
    {"a", jsi::field(&Chart::antigens, antigen_data)},
    {"i", jsi::field(&Chart::chart_info, chart_info_data)},
    {"p", jsi::field(&Chart::plot_spec, plot_spec_data)},
    {"s", jsi::field(&Chart::sera, serum_data)},
    {"t", jsi::field(&Chart::titers, titers_data)},
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
// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const Chart& aChart)
{
    return writer << StartObject
                  << JsonObjectKey("  version") << ACE_DUMP_VERSION
                  << JsonObjectKey("c") << StartObject
                  << JsonObjectKey("C") << aChart.column_bases()
                  // << JsonObjectKey("P") << aChart.projections()
                  // << JsonObjectKey("a") << aChart.antigens()
                  // << JsonObjectKey("i") << aChart.chart_info()
                  // << JsonObjectKey("p") << aChart.plot_spec()
                  // << JsonObjectKey("s") << aChart.sera()
                  // << JsonObjectKey("t") << aChart.titers()
                  << EndObject
                  << EndObject;
}

// ----------------------------------------------------------------------

void export_chart(std::string aFilename, const Chart& aChart)
{
    export_to_json(aChart, ACE_DUMP_VERSION, aFilename, 2);

} // export_chart

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
