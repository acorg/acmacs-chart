#include "acmacs-base/read-file.hh"
#include "acmacs-base/xz.hh"
#include "acmacs-base/date.hh"
#include "acmacs-base/string.hh"
#include "acmacs-base/float.hh"

#include "lispmds.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

static std::string make_lispmds(const Chart& aChart);
static std::string layout(const Projection& aProjection, const Chart& aChart);
static std::string acmacs_b1_data(const Chart& aChart);
static std::string encode(std::string aSource);
static std::string convert_titer(std::string aSource);
static std::string double_to_string_lisp(double aValue);

// ----------------------------------------------------------------------

void export_chart_lispmds(std::string aFilename, const Chart& aChart)
{
    acmacs_base::write_file(aFilename, make_lispmds(aChart));

} // export_chart_lispmds

// ----------------------------------------------------------------------

std::string make_lispmds(const Chart& aChart)
{
    std::string output = ";; MDS configuration file (version 0.5). -*- Lisp -*-\n;; Created by acmacsd/acmacs-chart at ";
    output += time_format("%Y-%m-%d %H:%M %Z\n");
    output += ";; Table: " + aChart.make_name() + "\n";
    if (!aChart.projections().empty()) {
        output += ";; stresses";
        for (const auto& projection: aChart.projections())
            output += " " + std::to_string(projection.stress());
        output += "\n";
    }
    output += "\n";

    const auto& antigens = aChart.antigens();
    const auto& sera = aChart.sera();
    const auto& projections = aChart.projections();

    output += "(MAKE-MASTER-MDS-WINDOW\n    (HI-IN '(\n";
    for (const auto& antigen: antigens)
        output += "             " + encode(antigen.full_name()) + "\n";
    output += "            )\n            '(\n";
    for (const auto& serum: sera)
        output += "             " + encode(serum.full_name()) + "\n";
    output += "            )\n            '(\n";
      // titers
    for (size_t ag_no = 0; ag_no < aChart.number_of_antigens(); ++ag_no) {
        output += "              (";
        for (size_t sr_no = 0; sr_no < aChart.number_of_sera(); ++sr_no) {
            if ((sr_no % 20) == 19)
                output += "\n                  ";
            if (sr_no)
                output += " ";
            output += convert_titer(aChart.titers().get(ag_no, sr_no));
        }
        output += ")\n";
    }
    output += "            )\n";
    output += "            '" + encode(aChart.make_name()) + "\n";
    output += "            )\n";

    std::vector<size_t> reference_antigens;
    antigens.reference_indices(reference_antigens);
    if (!reference_antigens.empty()) {
        output += "    :REFERENCE-ANTIGENS '(\n";
        for (size_t index: reference_antigens)
            output += "        " + encode(antigens[index].full_name()) + "\n";
        output += "    )\n";
    }
    if (!projections.empty()) {
        output += "    :STARTING-COORDSS '(\n";
        output += layout(projections[0], aChart);

        output += "    :BATCH-RUNS '(\n";
        const size_t number_of_dimentions = projections[0].number_of_dimentions();
        for (const auto& projection: projections) {
            if (projection.number_of_dimentions() == number_of_dimentions) {
                output += "    ((\n";
                output += layout(projection, aChart);
                output += "    " + double_to_string_lisp(projection.stress()) + " MULTIPLE-END-CONDITIONS NIL)\n";
            }
        }
        output += "    )\n";
    }
    output += acmacs_b1_data(aChart);
      // :MINIMUM-COLUMN-BASIS '"off"
      // :PLOT-SPEC '(
      // :BATCH-RUNS 'NIL
      // :MDS-DIMENSIONS '2
    output += "    :MOVEABLE-COORDS 'ALL\n    :UNMOVEABLE-COORDS 'NIL\n";
      // :CANVAS-COORD-TRANSFORMATIONS '(
    output += ")\n";

    return output;

} // make_lispmds

// ----------------------------------------------------------------------

std::string layout(const Projection& aProjection, const Chart& aChart)
{
    std::string output;
    const Layout& layout = aProjection.layout();
    for (const auto& point: layout) {
        output += "        (";
        for (double coord: point) {
            output += " " + double_to_string_lisp(coord);
        }
        output += ")\n";
    }

      // col and row adjusts
    output += "        ((COL-AND-ROW-ADJUSTS\n          (";
    for (size_t ag_no = 0; ag_no < aChart.number_of_antigens(); ++ag_no) {
        if (ag_no)
            output += " ";
        output += "-10000000";
    }
    output += "\n          ";
    const auto& stored_column_bases = aProjection.column_bases();
    if (!stored_column_bases.empty()) {
        for (double cb: stored_column_bases)
            output += " " + double_to_string_lisp(cb);
    }
    else {
        std::vector<double> column_bases;
        aChart.compute_column_bases(aProjection.minimum_column_basis(), column_bases);
        for (double cb: column_bases)
            output += " " + double_to_string_lisp(cb);
    }
    output += "\n          ";

      // avidity adjusts
    const auto& titer_multipliers = aProjection.titer_multipliers();
    if (!titer_multipliers.empty()) {
        for (size_t i = 0; i < (aChart.number_of_antigens() + aChart.number_of_sera()); ++i)
            output += " " + double_to_string_lisp(titer_multipliers[i]);
    }
    else {
        for (size_t i = 0; i < (aChart.number_of_antigens() + aChart.number_of_sera()); ++i)
            output += " 0";
    }
    output += "))))\n";
    return output;

} // layout

// ----------------------------------------------------------------------

std::string acmacs_b1_data(const Chart& aChart)
{
    std::string output = "    :ACMACS-B1-ANTIGENS '(\n";
    const auto& antigens = aChart.antigens();
    for (size_t ag_no = 0; ag_no < aChart.number_of_antigens(); ++ag_no) {
        const auto& antigen = antigens[ag_no];
        output += "        (\"index\" \"" + std::to_string(ag_no) + "\" \"name\" \"" + antigen.name() + "\"";
        if (!antigen.reassortant().empty())
            output += " \"reassortant\" \"" + antigen.reassortant() + "\"";
        if (!antigen.passage().empty())
            output += " \"passage\" \"" + antigen.passage() + "\"";
        if (!antigen.date().empty())
            output += " \"date\" \"" + antigen.date() + "\"";
        if (antigen.reference())
            output += " \"reference\" \"True\"";
        for (std::string lab_id: antigen.lab_id()) {
            const std::string lab = string::lower(lab_id.substr(0, lab_id.find('#')));
            output += " \"" + lab + "id\" \"" + lab_id.substr(lab_id.find('#') + 1) + "\"";
        }
        if (!antigen.annotations().empty())
            output += " \"extra\" \"" + string::join(" ", antigen.annotations()) + "\"";
        output += ")\n";
    }
    output += "    )\n";

    output += "    :ACMACS-B1-SERA '(\n";
    const auto& sera = aChart.sera();
    for (size_t sr_no = 0; sr_no < aChart.number_of_sera(); ++sr_no) {
        const auto& serum = sera[sr_no];
        output += "        (\"index\" \"" + std::to_string(sr_no) + "\" \"name\" \"" + serum.name() + "\"";
        if (!serum.reassortant().empty())
            output += " \"reassortant\" \"" + serum.reassortant() + "\"";
        if (!serum.passage().empty())
            output += " \"passage\" \"" + serum.passage() + "\"";
        if (!serum.serum_id().empty())
            output += " \"serum_id\" \"" + serum.serum_id() + "\"";
        if (!serum.serum_species().empty())
            output += " \"serum_species\" \"" + serum.serum_species() + "\"";
        if (!serum.annotations().empty())
            output += " \"extra\" \"" + string::join(" ", serum.annotations()) + "\"";
        output += ")\n";
    }
    output += "    )\n";

    return output;

} // acmacs_b1_data

// ----------------------------------------------------------------------

std::string encode(std::string aSource)
{
    std::string result = aSource;
    auto new_end = std::remove(result.begin(), result.end(), '[');
    new_end = std::remove(result.begin(), new_end, ']');
    new_end = std::remove(result.begin(), new_end, '(');
    new_end = std::remove(result.begin(), new_end, ')');
    result.erase(new_end, result.end());
    for (auto& c: result) {
        switch (c) {
          case ' ':
              c = '_';
              break;
          case '.':
          case ',':
          case '#':
          case ':':
          case ';':
          case '"':
          case '\'':
              c = '-';
              break;
          default:
              break;
        }
    }
    return result;

} // encode

// ----------------------------------------------------------------------

std::string convert_titer(std::string aSource)
{
    std::string result = aSource;
    if (!result.empty() && std::isdigit(result[0])) {
        const double value = stod(result);
        result = double_to_string_lisp(std::log2(value / 10.0));
    }
    else if (result.size() > 1 && (result[0] == '<' || result[0] == '>') && std::isdigit(result[1])) {
        const double value = stod(result.substr(1));
        result = std::string(1, result[0]) + double_to_string_lisp(std::log2(value / 10.0));
    }
    return result;

} // convert_titer

// ----------------------------------------------------------------------

std::string double_to_string_lisp(double aValue)
{
    std::string result = double_to_string(aValue);
    auto e_pos = result.find('e');
    if (e_pos != std::string::npos)
        result[e_pos] = 'd';
    else
        result += "d0";
    return result;

} // double_to_string_lisp

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
