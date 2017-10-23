#include "acmacs-base/range.hh"
//#include "acmacs-base/enumerate.hh"
#include "chart-plot-spec.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

void ChartPlotSpec::clear()
{
    mDrawingOrder.clear();
    mStyleForPoint.clear();
    mStyles.clear();
    mShownOnAll.clear();

} // ChartPlotSpec::clear

// ----------------------------------------------------------------------

void ChartPlotSpec::reset(const Chart& aChart)
{
    clear();

    acmacs::fill_with_indexes(mDrawingOrder, aChart.number_of_points());
    mStyleForPoint.resize(aChart.number_of_points(), 0);
    mStyles.emplace_back();

      // reference antigens
    ChartPlotSpecStyle ref_antigen("transparent", "black", ChartPlotSpecStyle::Circle, 1.5);
    for(auto index: aChart.antigens().reference_indices())
        set(index, ref_antigen);

      // sera
    ChartPlotSpecStyle serum("transparent", "black", ChartPlotSpecStyle::Box, 1.5);
    for(auto index: acmacs::incrementer(aChart.number_of_antigens(), aChart.number_of_points())) {
        set(index, serum);
    }

} // ChartPlotSpec::reset

// ----------------------------------------------------------------------

void ChartPlotSpec::set(size_t aPointNo, const ChartPlotSpecStyle& aStyle)
{
    auto found = std::find(mStyles.begin(), mStyles.end(), aStyle);
    if (found == mStyles.end()) {
        mStyles.push_back(aStyle);
        found = mStyles.end() - 1;
    }
    mStyleForPoint[aPointNo] = static_cast<size_t>(found - mStyles.begin());

} // ChartPlotSpec::set

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
