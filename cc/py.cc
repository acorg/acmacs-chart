#include "acmacs-base/pybind11.hh"
#include "locationdb/locdb.hh"

#include "chart.hh"
#include "ace.hh"
#include "lispmds.hh"
#include "point-style.hh"

// ----------------------------------------------------------------------

// static inline PointStyle& point_style_shape(PointStyle& aStyle, std::string aShape)
// {
//     if (aShape == "circle")
//         return aStyle.shape(PointStyle::Shape::Circle);
//     if (aShape == "box")
//         return aStyle.shape(PointStyle::Shape::Box);
//     if (aShape == "triangle")
//         return aStyle.shape(PointStyle::Shape::Triangle);
//     throw std::runtime_error("Unrecognized point style shape: " + aShape);
// }

// ----------------------------------------------------------------------

// static inline PointStyle* point_style_kw(PointStyle& aStyle, py::args args, py::kwargs kwargs)
// {
//     PointStyle* obj = new (&aStyle) PointStyle(PointStyle::Empty);
//     std::cerr << "point_style_kw" << std::endl;
//     return obj;
// }

// static inline PointStyle* point_style_modify_kw(PointStyle* aStyle, py::args args, py::kwargs kwargs)
// {
//     std::cerr << "point_style_modify_kw" << std::endl;
//     return aStyle;
// }

// static inline PointStyle* new_point_style_kw(py::args args, py::kwargs kwargs)
// {
//     PointStyle* style = new PointStyle(PointStyle::Empty);
//     std::cerr << "new_point_style_kw" << std::endl;
//     return style;
// }

// ----------------------------------------------------------------------

PYBIND11_MODULE(acmacs_chart_backend, m)
{
    m.doc() = "Acmacs chart access plugin";

      // ----------------------------------------------------------------------
      // Antigen, Serum
      // ----------------------------------------------------------------------

    py::class_<Antigen>(m, "Antigen")
            .def("full_name", &Antigen::full_name)
            .def("abbreviated_name", &Antigen::abbreviated_name, py::arg("locdb"), py::doc("includes passage, reassortant, annotations"))
            .def("name_abbreviated", &Antigen::name_abbreviated, py::arg("locdb"), py::doc("just name without passage, reassortant, annotations"))
            .def("location_abbreviated", &Antigen::location_abbreviated, py::arg("locdb"))
            .def("name", py::overload_cast<>(&Antigen::name, py::const_))
            .def("lineage", py::overload_cast<>(&Antigen::lineage, py::const_))
            .def("passage", py::overload_cast<>(&Antigen::passage, py::const_))
            .def("passage_type", &Antigen::passage_type)
            .def("reassortant", py::overload_cast<>(&Antigen::reassortant, py::const_))
            .def("semantic", py::overload_cast<>(&Antigen::semantic, py::const_))
            .def("annotations", [](const Antigen &as) { py::list list; for (const auto& anno: as.annotations()) { list.append(py::str(anno)); } return list; }, py::doc("returns a copy of the annotation list, modifications to the returned list are not applied"))
            .def("abbreviated_name_with_passage_type", &Antigen::abbreviated_name_with_passage_type, py::arg("locdb"))
            .def("date", py::overload_cast<>(&Antigen::date, py::const_))
            .def("lab_id", [](const Antigen &a) { py::list list; for (const auto& li: a.lab_id()) { list.append(py::str(li)); } return list; }, py::doc("returns a copy of the lab_id list, modifications to the returned list are not applied"))
            // .def("find_in_hidb", &Antigen::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

    py::class_<Serum>(m, "Serum")
            .def("full_name", &Serum::full_name)
            .def("abbreviated_name", &Serum::abbreviated_name, py::arg("locdb"), py::doc("includes passage, reassortant, annotations"))
            .def("name_abbreviated", &Serum::name_abbreviated, py::arg("locdb"), py::doc("just name without passage, reassortant, annotations"))
            .def("location_abbreviated", &Serum::location_abbreviated, py::arg("locdb"))
            .def("name", py::overload_cast<>(&Serum::name, py::const_))
            .def("lineage", py::overload_cast<>(&Serum::lineage, py::const_))
            .def("passage", py::overload_cast<>(&Serum::passage, py::const_))
            .def("passage_type", &Serum::passage_type)
            .def("reassortant", py::overload_cast<>(&Serum::reassortant, py::const_))
            .def("semantic", py::overload_cast<>(&Serum::semantic, py::const_))
            .def("annotations", [](const Serum &as) { py::list list; for (const auto& anno: as.annotations()) { list.append(py::str(anno)); } return list; }, py::doc("returns a copy of the annotation list, modifications to the returned list are not applied"))
            .def("serum_id", py::overload_cast<>(&Serum::serum_id, py::const_))
            .def("serum_species", py::overload_cast<>(&Serum::serum_species, py::const_))
            .def("homologous", py::overload_cast<>(&Serum::homologous, py::const_))
            // .def("find_in_hidb", &Serum::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

    py::class_<Antigens>(m, "Antigens")
            .def("continents", [](const Antigens& antigens, const LocDb& aLocDb, bool aExcludeReference) { Antigens::ContinentData data; antigens.continents(data, aLocDb, aExcludeReference); return data; }, py::arg("locdb"), py::arg("exclude_reference") = true)
            .def("countries", [](const Antigens& antigens, const LocDb& aLocDb) { Antigens::CountryData data; antigens.countries(data, aLocDb); return data; })
            .def("country", &Antigens::country, py::arg("country"), py::arg("loc_db"))
            .def("continent", &Antigens::continent, py::arg("continent"), py::arg("loc_db"))
            .def("find_by_name_matching", [](const Antigens& antigens, std::string aName, string_match::score_t aScoreThreshold, bool aVerbose) { std::vector<size_t> indices; antigens.find_by_name_matching(aName, indices, aScoreThreshold, aVerbose); return indices; }, py::arg("name"), py::arg("score_threshold") = 0, py::arg("verbose") = false)
            .def("find_by_lab_id", [](const Antigens& antigens, std::string aLabId) { std::vector<size_t> indices; antigens.find_by_lab_id(aLabId, indices); return indices; }, py::arg("lab_id"))
            .def("find_by_lab_ids", [](const Antigens& antigens, std::vector<std::string> aLabIds) { std::vector<size_t> indices; for (const auto& lab_id: aLabIds) { antigens.find_by_lab_id(lab_id, indices); } return indices; }, py::arg("lab_ids"))
            .def("reference_indices", &Antigens::reference_indices)
            .def("test_indices", &Antigens::test_indices)
            .def("egg_indices", &Antigens::egg_indices)
            .def("cell_indices", &Antigens::cell_indices)
            .def("reassortant_indices", &Antigens::reassortant_indices)
            .def("date_range_indices", &Antigens::date_range_indices, py::arg("first") = std::string(), py::arg("after_last") = std::string())
            .def("__getitem__", [](const Antigens& antigens, int aIndex) -> const Antigen& { if (aIndex >= 0) return antigens[static_cast<size_t>(aIndex)]; else return antigens[static_cast<size_t>(static_cast<int>(antigens.size()) + aIndex)]; })
            ;

    py::class_<Sera>(m, "Sera")
            .def("find_by_name_matching", [](const Sera& sera, std::string aName, string_match::score_t aScoreThreshold, bool aVerbose) { std::vector<size_t> indices; sera.find_by_name_matching(aName, indices, aScoreThreshold, aVerbose); return indices; }, py::arg("name"), py::arg("score_threshold") = 0, py::arg("verbose") = false)
            .def("__getitem__", [](const Sera& sera, int aIndex) -> const Serum& { if (aIndex >= 0) return sera[static_cast<size_t>(aIndex)]; else return sera[static_cast<size_t>(static_cast<int>(sera.size()) + aIndex)]; })
            ;

      // ----------------------------------------------------------------------
      // Chart
      // ----------------------------------------------------------------------

    py::class_<ChartInfo>(m, "ChartInfo")
            .def("virus", py::overload_cast<>(&ChartInfo::virus, py::const_))
            .def("virus_type", py::overload_cast<>(&ChartInfo::virus_type, py::const_))
            .def("assay", py::overload_cast<>(&ChartInfo::assay, py::const_))
            .def("date", py::overload_cast<>(&ChartInfo::date, py::const_))
            .def("lab", py::overload_cast<>(&ChartInfo::lab, py::const_))
            .def("rbc", py::overload_cast<>(&ChartInfo::rbc, py::const_))
            .def("name", py::overload_cast<>(&ChartInfo::name, py::const_))
            .def("subset", py::overload_cast<>(&ChartInfo::subset, py::const_))
            .def("type", &ChartInfo::type_as_string)
            .def("make_name", &ChartInfo::make_name)
            ;

    py::class_<Transformation>(m, "Transformation")
            .def("transform", [](const Transformation& aTransformation, double x, double y) -> std::vector<double> { const auto [x1, y1] = aTransformation.transform(x, y); return {x1, y1}; }, py::arg("x"), py::arg("y"))
            ;

    py::class_<LayoutBase>(m, "LayoutBase")
            .def("number_of_points", &LayoutBase::number_of_points)
            .def("number_of_dimensions", &LayoutBase::number_of_dimensions)
            .def("__getitem__", [](const LayoutBase& aLayout, size_t aIndex) -> std::vector<double> { return aLayout[aIndex]; }, py::arg("index"))
            ;

    py::class_<Projection>(m, "Projection")
            .def("stress", py::overload_cast<>(&Projection::stress, py::const_))
            .def("minimum_column_basis", &Projection::minimum_column_basis_for_json)
            .def("layout", py::overload_cast<>(&Projection::layout, py::const_), py::return_value_policy::reference)
            .def("transformation", py::overload_cast<>(&Projection::transformation, py::const_), py::return_value_policy::reference)
            ;

    py::class_<Titer>(m, "Titer")
            .def("__str__", [](const Titer& aTiter) -> std::string { return aTiter; })
            ;

    py::class_<ChartTiters>(m, "ChartTiters")
            .def("get", &ChartTiters::get, py::arg("ag_no"), py::arg("sr_no"))
            .def("max_for_serum", &ChartTiters::max_for_serum, py::arg("sr_no"))
            ;

    py::class_<ChartPlotSpecStyle>(m, "ChartPlotSpecStyle")
            .def("shown", py::overload_cast<>(&ChartPlotSpecStyle::shown, py::const_))
            .def("fill_color", py::overload_cast<>(&ChartPlotSpecStyle::fill_color, py::const_))
            .def("outline_color", py::overload_cast<>(&ChartPlotSpecStyle::outline_color, py::const_))
            .def("outline_width", py::overload_cast<>(&ChartPlotSpecStyle::outline_width, py::const_))
            .def("shape", py::overload_cast<>(&ChartPlotSpecStyle::shape_as_string, py::const_))
            .def("size", py::overload_cast<>(&ChartPlotSpecStyle::size, py::const_))
            .def("rotation", py::overload_cast<>(&ChartPlotSpecStyle::rotation, py::const_))
            .def("aspect", py::overload_cast<>(&ChartPlotSpecStyle::aspect, py::const_))
            ;

    py::class_<ChartPlotSpec>(m, "ChartPlotSpec")
            .def("style_for", &ChartPlotSpec::style_for, py::arg("point_no") = 0, py::return_value_policy::reference)
            ;

    py::class_<Chart>(m, "Chart")
            .def("number_of_antigens", &Chart::number_of_antigens)
            .def("number_of_sera", &Chart::number_of_sera)
            .def("number_of_projections", &Chart::number_of_projections)
            .def("antigens", py::overload_cast<>(&Chart::antigens), py::return_value_policy::reference)
            .def("sera", py::overload_cast<>(&Chart::sera), py::return_value_policy::reference)
            .def("antigen", py::overload_cast<size_t>(&Chart::antigen, py::const_), py::arg("no"), py::return_value_policy::reference)
            .def("serum", py::overload_cast<size_t>(&Chart::serum, py::const_), py::arg("no"), py::return_value_policy::reference)
            .def("lineage", &Chart::lineage)
            .def("make_name", &Chart::make_name, py::arg("projection_no") = -1)
            // .def("vaccines", &Chart::vaccines, py::arg("name"), py::arg("hidb"))
            // .def("table_id", &Chart::table_id)
            .def("find_homologous_antigen_for_sera", &Chart::find_homologous_antigen_for_sera)
            .def("chart_info", py::overload_cast<>(&Chart::chart_info, py::const_), py::return_value_policy::reference)
            .def("titers", py::overload_cast<>(&Chart::titers, py::const_), py::return_value_policy::reference)
            .def("serum_circle_radius", &Chart::serum_circle_radius, py::arg("antigen_no"), py::arg("serum_no"), py::arg("projection_no") = 0, py::arg("verbose") = false)
            .def("serum_coverage", [](const Chart& aChart, size_t aAntigenNo, size_t aSerumNo) -> std::vector<std::vector<size_t>> { std::vector<size_t> within, outside; aChart.serum_coverage(aAntigenNo, aSerumNo, within, outside); return {within, outside}; } , py::arg("antigen_no"), py::arg("serum_no"))
            .def("antigens_not_found_in", [](const Chart& aChart, const Chart& aNother) -> std::vector<size_t> { auto gen = aChart.antigens_not_found_in(aNother); return {gen.begin(), gen.end()}; }, py::arg("another_chart"))
            .def("projection", py::overload_cast<size_t>(&Chart::projection, py::const_), py::arg("projection_no") = 0, py::return_value_policy::reference)
            .def("plot_spec", py::overload_cast<>(&Chart::plot_spec, py::const_), py::return_value_policy::reference)
        ;

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif
    static py::exception<AceChartReadError> ace_chart_read_error(m, "AceChartReadError");
#pragma GCC diagnostic pop
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p)
                std::rethrow_exception(p);
        }
        catch (const AceChartReadError& e) {
            std::cerr << "AceChartReadError: " << e.what() << '\n';
            ace_chart_read_error(e.what());
        }
    });

    m.def("import_chart", [](std::string data, bool timer) { return import_chart(data, timer ? report_time::Yes : report_time::No); }, py::arg("data"), py::arg("timer") = false, py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("import_chart", [](py::bytes data, bool timer) { return import_chart(data, timer ? report_time::Yes : report_time::No); }, py::arg("data"), py::arg("timer") = false, py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("export_chart", [](std::string filename, const Chart& chart, bool timer) { export_chart(filename, chart, timer ? report_time::Yes : report_time::No); }, py::arg("filename"), py::arg("chart"), py::arg("timer") = false, py::doc("Exports chart into a file in the ace format."));
      // m.def("export_chart", py::overload_cast<std::string, const Chart&, const std::vector<PointStyle>&>(&export_chart), py::arg("filename"), py::arg("chart"), py::arg("point_styles"), py::doc("Exports chart into a file in the ace format."));
    m.def("export_chart_lispmds", py::overload_cast<std::string, const Chart&>(&export_chart_lispmds), py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the lispmds save format."));
    m.def("export_chart_lispmds", py::overload_cast<std::string, const Chart&, const std::vector<PointStyle>&, const Transformation&>(&export_chart_lispmds), py::arg("filename"), py::arg("chart"), py::arg("point_styles"), py::arg("transformation"), py::doc("Exports chart into a file in the lispmds save format."));

      // ----------------------------------------------------------------------
      //
      // ----------------------------------------------------------------------

    m.def("virus_name_match_threshold", &virus_name::match_threshold, py::arg("name"), py::doc("Extracts virus name without passage, reassortant, extra, etc. and calculates match threshold (to use with antigens.find_by_name_matching), match threshold is a square of virus name length."));
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
