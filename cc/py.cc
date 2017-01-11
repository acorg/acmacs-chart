#include "chart.hh"
#include "ace.hh"
#include "acmacs-base/pybind11.hh"

// ----------------------------------------------------------------------

PYBIND11_PLUGIN(acmacs_chart_backend)
{
    py::module m("acmacs_chart_backend", "Acmacs chart access plugin");

      // ----------------------------------------------------------------------
      // Antigen, Serum
      // ----------------------------------------------------------------------

    py::class_<AntigenSerum>(m, "AntigenSerum")
            // .def("full_name", &AntigenSerum::full_name)
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
            ;

    py::class_<Serum, AntigenSerum>(m, "Serum")
            .def("serum_id", static_cast<const std::string (Serum::*)() const>(&Serum::serum_id))
            .def("serum_species", static_cast<const std::string (Serum::*)() const>(&Serum::serum_species))
            .def("homologous", static_cast<int (Serum::*)() const>(&Serum::homologous))
            ;

      // ----------------------------------------------------------------------
      // Chart
      // ----------------------------------------------------------------------

    py::class_<Chart>(m, "Chart")
            .def("number_of_antigens", &Chart::number_of_antigens)
            .def("number_of_sera", &Chart::number_of_sera)
            .def("antigen", &Chart::antigen, py::arg("no"))
            .def("serum", &Chart::serum, py::arg("no"))
            // .def("table_id", &Chart::table_id)
            // .def("find_homologous_antigen_for_sera", &Chart::find_homologous_antigen_for_sera)
            ;

    m.def("import_chart", &import_chart, py::arg("data"), py::doc("Imports chart from a buffer or file in the ace format."));
    m.def("export_chart", &export_chart, py::arg("filename"), py::arg("chart"), py::doc("Exports chart into a file in the ace format."));

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
