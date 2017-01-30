#include "chart.hh"
#include "ace.hh"
#include "draw.hh"
#include "hidb/hidb.hh"
#include "acmacs-base/pybind11.hh"

// ----------------------------------------------------------------------

static inline PointStyle& point_style_shape(PointStyle& aStyle, std::string aShape)
{
    if (aShape == "circle")
        return aStyle.shape(PointStyle::Shape::Circle);
    if (aShape == "box")
        return aStyle.shape(PointStyle::Shape::Box);
    if (aShape == "triangle")
        return aStyle.shape(PointStyle::Shape::Triangle);
    throw std::runtime_error("Unrecognized point style shape: " + aShape);
}

// ----------------------------------------------------------------------

PYBIND11_PLUGIN(acmacs_chart_backend)
{
    py::module m("acmacs_chart_backend", "Acmacs chart access plugin");

      // ----------------------------------------------------------------------
      // HiDb
      // ----------------------------------------------------------------------

    py::register_exception<hidb::HiDb::NotFound>(m, "hidb_NotFound");

    py::class_<hidb::PerTable>(m, "hidb_PerTable")
            .def("table_id", py::overload_cast<>(&hidb::PerTable::table_id, py::const_))
            ;

    py::class_<hidb::AntigenData>(m, "hidb_AntigenSerumData_Antigen")
            .def("number_of_tables", &hidb::AntigenData::number_of_tables)
            .def("most_recent_table", &hidb::AntigenData::most_recent_table)
            .def("date", &hidb::AntigenData::date)
            ;

    py::class_<hidb::SerumData>(m, "hidb_AntigenSerumData_Serum")
            .def("number_of_tables", &hidb::SerumData::number_of_tables)
            .def("most_recent_table", &hidb::SerumData::most_recent_table)
            ;

    py::class_<hidb::HiDb>(m, "HiDb")
            ;

    py::class_<hidb::HiDbSet>(m, "HiDbSet")
            .def(py::init<std::string>(), py::arg("hidb_dir"))
            .def("get", &hidb::HiDbSet::get, py::arg("virus_type"), py::return_value_policy::reference)
            ;

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
            .def("find_in_hidb", &Antigen::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

    py::class_<Serum, AntigenSerum>(m, "Serum")
            .def("serum_id", py::overload_cast<>(&Serum::serum_id, py::const_))
            .def("serum_species", py::overload_cast<>(&Serum::serum_species, py::const_))
            .def("homologous", py::overload_cast<>(&Serum::homologous, py::const_))
            .def("find_in_hidb", &Serum::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
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
            .def("antigen", &Chart::antigen, py::arg("no"), py::return_value_policy::reference)
            .def("serum", &Chart::serum, py::arg("no"), py::return_value_policy::reference)
            .def("lineage", &Chart::lineage)
            .def("vaccines", &Chart::vaccines, py::arg("name"), py::arg("hidb"))
            // .def("table_id", &Chart::table_id)
            // .def("find_homologous_antigen_for_sera", &Chart::find_homologous_antigen_for_sera)
            .def("chart_info", py::overload_cast<>(&Chart::chart_info, py::const_), py::return_value_policy::reference)
        ;

    m.def("import_chart", &import_chart, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("export_chart", &export_chart, py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the ace format."));

      // ----------------------------------------------------------------------

    py::class_<Vaccines::HomologousSerum>(m, "Vaccines_HomologousSerum")
            .def_readonly("serum", &Vaccines::HomologousSerum::serum)
            .def_readonly("most_recent_table", &Vaccines::HomologousSerum::most_recent_table_date)
            .def("number_of_tables", &Vaccines::HomologousSerum::number_of_tables)
            ;

    py::class_<Vaccines::Entry>(m, "Vaccines_Entry")
            .def_readonly("antigen", &Vaccines::Entry::antigen)
            .def_readonly("homologous_sera", &Vaccines::Entry::homologous_sera, py::return_value_policy::reference)
            ;

    py::class_<Vaccines>(m, "Vaccines")
            .def("report", &Vaccines::report)
            .def("egg", &Vaccines::egg, py::return_value_policy::reference)
            .def("cell", &Vaccines::cell, py::return_value_policy::reference)
            .def("reassortant", &Vaccines::reassortant, py::return_value_policy::reference)
            ;

      // ----------------------------------------------------------------------
      // Draw
      // ----------------------------------------------------------------------

    py::class_<Color>(m, "Color")
            .def(py::init<std::string>(), py::arg("color") = "black")
            ;

    py::class_<PointStyle> point_style(m, "PointStyle");

    py::enum_<enum PointStyle::Empty>(point_style, "PointStyle_Empty").value("Empty", PointStyle::Empty).export_values();

    point_style
            .def(py::init<enum PointStyle::Empty>(), py::arg("_") = PointStyle::Empty)
            .def("show", &PointStyle::show)
            .def("hide", &PointStyle::hide)
            .def("shape", &point_style_shape, py::arg("shape"))
            .def("fill", [](PointStyle& style, std::string color) -> PointStyle& { return style.fill(color); }, py::arg("fill"))
            .def("fill", &PointStyle::fill, py::arg("fill"))
            .def("outline", [](PointStyle& style, std::string color) -> PointStyle& { return style.outline(color); }, py::arg("outline"))
            .def("outline", &PointStyle::outline, py::arg("outline"))
            .def("size", [](PointStyle& style, double aSize) -> PointStyle& { return style.size(Pixels{aSize}); }, py::arg("size"))
            .def("outline_width", &PointStyle::outline_width, py::arg("outline_width"))
            .def("aspect", py::overload_cast<double>(&PointStyle::aspect), py::arg("aspect"))
            .def("rotation", py::overload_cast<double>(&PointStyle::rotation), py::arg("rotation"))
            .def("scale", &PointStyle::scale, py::arg("scale"))
            .def("scale_outline", &PointStyle::scale_outline, py::arg("scale"))
            ;

    py::class_<ChartDraw>(m, "ChartDraw")
            .def(py::init<Chart&, size_t>(), py::arg("chart"), py::arg("projection_no") = 0)
            .def("prepare", &ChartDraw::prepare)
            .def("draw", py::overload_cast<std::string, double>(&ChartDraw::draw), py::arg("filename"), py::arg("size"))
            .def("mark_egg_antigens", &ChartDraw::mark_egg_antigens)
            .def("mark_reassortant_antigens", &ChartDraw::mark_reassortant_antigens)
            .def("all_grey", &ChartDraw::mark_all_grey, py::arg("color") = Color("grey80"))
            .def("scale_points", &ChartDraw::scale_points, py::arg("scale"), py::arg("outline_scale") = 1.0, py::doc("outline_scale=0 means use point scale for outline too"))
            .def("modify_point_by_index", &ChartDraw::modify_point_by_index, py::arg("index"), py::arg("style"))
            ;

      // ----------------------------------------------------------------------

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
