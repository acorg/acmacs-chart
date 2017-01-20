#include "chart.hh"
#include "ace.hh"
#include "hidb/hidb.hh"
#include "acmacs-base/pybind11.hh"

// ----------------------------------------------------------------------

PYBIND11_PLUGIN(acmacs_chart_backend)
{
    py::module m("acmacs_chart_backend", "Acmacs chart access plugin");

      // ----------------------------------------------------------------------
      // HiDb
      // ----------------------------------------------------------------------

    py::register_exception<hidb::HiDb::NotFound>(m, "hidb_NotFound");

    py::class_<hidb::PerTable>(m, "hidb_PerTable")
            .def("table_id", static_cast<const std::string (hidb::PerTable::*)() const>(&hidb::PerTable::table_id))
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
            .def("name", static_cast<const std::string (AntigenSerum::*)() const>(&AntigenSerum::name))
            .def("lineage", static_cast<const std::string (AntigenSerum::*)() const>(&AntigenSerum::lineage))
            .def("passage", static_cast<const std::string (AntigenSerum::*)() const>(&AntigenSerum::passage))
            .def("reassortant", static_cast<const std::string (AntigenSerum::*)() const>(&AntigenSerum::reassortant))
            .def("semantic", static_cast<const std::string (AntigenSerum::*)() const>(&AntigenSerum::semantic))
            .def("annotations", [](const AntigenSerum &as) { py::list list; for (const auto& anno: as.annotations()) { list.append(py::str(anno)); } return list; }, py::doc("returns a copy of the annotation list, modifications to the returned list are not applied"))
            ;

    py::class_<Antigen, AntigenSerum>(m, "Antigen")
            .def("date", static_cast<const std::string (Antigen::*)() const>(&Antigen::date))
            .def("lab_id", [](const Antigen &a) { py::list list; for (const auto& li: a.lab_id()) { list.append(py::str(li)); } return list; }, py::doc("returns a copy of the lab_id list, modifications to the returned list are not applied"))
            .def("find_in_hidb", &Antigen::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

    py::class_<Serum, AntigenSerum>(m, "Serum")
            .def("serum_id", static_cast<const std::string (Serum::*)() const>(&Serum::serum_id))
            .def("serum_species", static_cast<const std::string (Serum::*)() const>(&Serum::serum_species))
            .def("homologous", static_cast<int (Serum::*)() const>(&Serum::homologous))
            .def("find_in_hidb", &Serum::find_in_hidb, py::arg("hidb"), py::return_value_policy::reference)
            ;

      // ----------------------------------------------------------------------
      // Chart
      // ----------------------------------------------------------------------

    py::class_<ChartInfo>(m, "ChartInfo")
            .def("virus", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::virus))
            .def("virus_type", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::virus_type))
            .def("assay", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::assay))
            .def("date", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::date))
            .def("lab", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::lab))
            .def("rbc", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::rbc))
            .def("name", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::name))
            .def("subset", static_cast<const std::string (ChartInfo::*)() const>(&ChartInfo::subset))
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
            .def("chart_info", static_cast<const ChartInfo& (Chart::*)() const>(&Chart::chart_info), py::return_value_policy::reference)
        ;

    m.def("import_chart", &import_chart, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("export_chart", &export_chart, py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the ace format."));

      // ----------------------------------------------------------------------

    py::class_<Vaccines::Entry>(m, "Vaccines_Entry")
            .def_readonly("antigen", &Vaccines::Entry::antigen)
            .def_readonly("homologous_sera", &Vaccines::Entry::homologous_sera)
            ;

    py::class_<Vaccines>(m, "Vaccines")
            .def("report", &Vaccines::report)
            .def("egg", &Vaccines::egg, py::return_value_policy::reference)
            .def("cell", &Vaccines::cell, py::return_value_policy::reference)
            .def("reassortant", &Vaccines::reassortant, py::return_value_policy::reference)
            ;

      // ----------------------------------------------------------------------

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
