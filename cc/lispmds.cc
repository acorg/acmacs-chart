#include "acmacs-base/read-file.hh"
#include "acmacs-base/xz.hh"
#include "acmacs-base/date.hh"
#include "acmacs-base/string.hh"
#include "acmacs-base/float.hh"

#include "lispmds.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

static std::string make_lispmds(const Chart& aChart);
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

    output += "(MAKE-MASTER-MDS-WINDOW\n    (HI-IN '(\n";
    for (const auto& antigen: aChart.antigens())
        output += "             " + encode(antigen.full_name()) + "\n";
    output += "            )\n            '(\n";
    for (const auto& serum: aChart.sera())
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
    if (!aChart.projections().empty()) {
        output += "    :STARTING-COORDSS '(\n";
        const Layout& layout = aChart.projection(0).layout();
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
        const auto& stored_column_bases = aChart.projection(0).column_bases();
        if (!stored_column_bases.empty()) {
            for (double cb: stored_column_bases)
                output += " " + double_to_string_lisp(cb);
        }
        else {
            std::vector<double> column_bases;
            aChart.compute_column_bases(aChart.projection(0).minimum_column_basis(), column_bases);
            for (double cb: column_bases)
                output += " " + double_to_string_lisp(cb);
        }
        output += "\n          ";

          // avidity adjusts
        if (!aChart.projection(0).titer_multipliers().empty())
            throw std::runtime_error("chart has titer_multipliers, generating lispmds save is not implemented in this case");
        for (size_t i = 0; i < (aChart.number_of_antigens() + aChart.number_of_sera()); ++i)
            output += " 0";
        output += "))))\n";
    }
      // :BATCH-RUNS '(
      // :REFERENCE-ANTIGENS '(
      // :ACMACS-B1-ANTIGENS '(
      // :ACMACS-B1-SERA '(
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
