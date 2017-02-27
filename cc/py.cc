#include "acmacs-base/pybind11.hh"
#include "locationdb/locdb.hh"

#include "chart.hh"
#include "ace.hh"
#include "lispmds.hh"

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

PYBIND11_PLUGIN(acmacs_chart_backend)
{
    py::module m("acmacs_chart_backend", "Acmacs chart access plugin");

      // ----------------------------------------------------------------------
      // Antigen, Serum
      // ----------------------------------------------------------------------

    py::class_<AntigenSerum>(m, "AntigenSerum")
            .def("full_name", &AntigenSerum::full_name)
            .def("abbreviated_name", &AntigenSerum::abbreviated_name, py::arg("locdb"), py::doc("includes passage, reassortant, annotations"))
            .def("name_abbreviated", &AntigenSerum::name_abbreviated, py::arg("locdb"), py::doc("just name without passage, reassortant, annotations"))
            .def("location_abbreviated", &AntigenSerum::location_abbreviated, py::arg("locdb"))
            .def("name", py::overload_cast<>(&AntigenSerum::name, py::const_))
            .def("lineage", py::overload_cast<>(&AntigenSerum::lineage, py::const_))
            .def("passage", py::overload_cast<>(&AntigenSerum::passage, py::const_))
            .def("passage_type", &AntigenSerum::passage_type)
            .def("reassortant", py::overload_cast<>(&AntigenSerum::reassortant, py::const_))
            .def("semantic", py::overload_cast<>(&AntigenSerum::semantic, py::const_))
            .def("annotations", [](const AntigenSerum &as) { py::list list; for (const auto& anno: as.annotations()) { list.append(py::str(anno)); } return list; }, py::doc("returns a copy of the annotation list, modifications to the returned list are not applied"))
            ;

    py::class_<Antigen, AntigenSerum>(m, "Antigen")
            .def("abbreviated_name_with_passage_type", &Antigen::abbreviated_name_with_passage_type, py::arg("locdb"))
            .def("date", py::overload_cast<>(&Antigen::date, py::const_))
            .def("lab_id", [](const Antigen &a) { py::list list; for (const auto& li: a.lab_id()) { list.append(py::str(li)); } return list; }, py::doc("returns a copy of the lab_id list, modifications to the returned list are not applied"))
            // .def("find_in_hidb", &Antigen::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

    py::class_<Serum, AntigenSerum>(m, "Serum")
            .def("serum_id", py::overload_cast<>(&Serum::serum_id, py::const_))
            .def("serum_species", py::overload_cast<>(&Serum::serum_species, py::const_))
            .def("homologous", py::overload_cast<>(&Serum::homologous, py::const_))
            // .def("find_in_hidb", &Serum::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

    py::class_<LocDb>(m, "LocDb")
            .def(py::init<>())
            .def("import_from", &LocDb::importFrom, py::arg("filename"))
            ;

    py::class_<Antigens>(m, "Antigens")
            .def("continents", [](const Antigens& antigens, const LocDb& aLocDb) { Antigens::ContinentData data; antigens.continents(data, aLocDb); return data; })
            .def("countries", [](const Antigens& antigens, const LocDb& aLocDb) { Antigens::CountryData data; antigens.countries(data, aLocDb); return data; })
            .def("country", [](const Antigens& antigens, std::string aCountry, const LocDb& aLocDb) { std::vector<size_t> indices; antigens.country(aCountry, indices, aLocDb); return indices; })
            .def("find_by_name_matching", [](const Antigens& antigens, std::string aName) { std::vector<size_t> indices; antigens.find_by_name_matching(aName, indices); return indices; })
            .def("reference_indices", [](const Antigens& antigens) { std::vector<size_t> indices; antigens.reference_indices(indices); return indices; })
            .def("test_indices", [](const Antigens& antigens) { std::vector<size_t> indices; antigens.test_indices(indices); return indices; })
            .def("date_range_indices", [](const Antigens& antigens, std::string first_date, std::string after_last_date) { std::vector<size_t> indices; antigens.date_range_indices(first_date, after_last_date, indices); return indices; }, py::arg("first") = std::string(), py::arg("after_last") = std::string())
            .def("egg_indices", [](const Antigens& antigens) { std::vector<size_t> indices; antigens.egg_indices(indices); return indices; })
            .def("cell_indices", [](const Antigens& antigens) { std::vector<size_t> indices; antigens.cell_indices(indices); return indices; })
            .def("reassortant_indices", [](const Antigens& antigens) { std::vector<size_t> indices; antigens.reassortant_indices(indices); return indices; })
            ;

    py::class_<Sera>(m, "Sera")
            .def("find_by_name_matching", [](const Sera& sera, std::string aName) { std::vector<size_t> indices; sera.find_by_name_matching(aName, indices); return indices; })
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

    py::class_<Chart>(m, "Chart")
            .def("number_of_antigens", &Chart::number_of_antigens)
            .def("number_of_sera", &Chart::number_of_sera)
            .def("antigens", py::overload_cast<>(&Chart::antigens), py::return_value_policy::reference)
            .def("sera", py::overload_cast<>(&Chart::sera), py::return_value_policy::reference)
            .def("antigen", &Chart::antigen, py::arg("no"), py::return_value_policy::reference)
            .def("serum", &Chart::serum, py::arg("no"), py::return_value_policy::reference)
            .def("lineage", &Chart::lineage)
            .def("make_name", &Chart::make_name)
            // .def("vaccines", &Chart::vaccines, py::arg("name"), py::arg("hidb"))
            // .def("table_id", &Chart::table_id)
            .def("find_homologous_antigen_for_sera", &Chart::find_homologous_antigen_for_sera)
            .def("chart_info", py::overload_cast<>(&Chart::chart_info, py::const_), py::return_value_policy::reference)
        ;

    m.def("import_chart", &import_chart, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("import_chart", [](py::bytes data) { return import_chart(data); }, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("export_chart", &export_chart, py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the ace format."));
    m.def("export_chart_lispmds", &export_chart_lispmds, py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the lispmds save format."));

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
