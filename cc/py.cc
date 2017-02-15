#include "acmacs-base/pybind11.hh"
#include "locationdb/locdb.hh"

#include "chart.hh"
#include "ace.hh"

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
            // .def("variant_id", &AntigenSerum::variant_id)
            .def("name", py::overload_cast<>(&AntigenSerum::name, py::const_))
            .def("lineage", py::overload_cast<>(&AntigenSerum::lineage, py::const_))
            .def("passage", py::overload_cast<>(&AntigenSerum::passage, py::const_))
            .def("reassortant", py::overload_cast<>(&AntigenSerum::reassortant, py::const_))
            .def("semantic", py::overload_cast<>(&AntigenSerum::semantic, py::const_))
            .def("annotations", [](const AntigenSerum &as) { py::list list; for (const auto& anno: as.annotations()) { list.append(py::str(anno)); } return list; }, py::doc("returns a copy of the annotation list, modifications to the returned list are not applied"))
            ;

    py::class_<Antigen, AntigenSerum>(m, "Antigen")
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
            ;

    py::class_<Sera>(m, "Sera")
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
            ;

    py::class_<Chart>(m, "Chart")
            .def("number_of_antigens", &Chart::number_of_antigens)
            .def("number_of_sera", &Chart::number_of_sera)
            .def("antigens", py::overload_cast<>(&Chart::antigens), py::return_value_policy::reference)
            .def("sera", py::overload_cast<>(&Chart::sera), py::return_value_policy::reference)
            .def("antigen", &Chart::antigen, py::arg("no"), py::return_value_policy::reference)
            .def("serum", &Chart::serum, py::arg("no"), py::return_value_policy::reference)
            .def("lineage", &Chart::lineage)
            // .def("vaccines", &Chart::vaccines, py::arg("name"), py::arg("hidb"))
            // .def("table_id", &Chart::table_id)
            // .def("find_homologous_antigen_for_sera", &Chart::find_homologous_antigen_for_sera)
            .def("chart_info", py::overload_cast<>(&Chart::chart_info, py::const_), py::return_value_policy::reference)
        ;

    m.def("import_chart", &import_chart, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("import_chart", [](py::bytes data) { return import_chart(data); }, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("export_chart", &export_chart, py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the ace format."));

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
