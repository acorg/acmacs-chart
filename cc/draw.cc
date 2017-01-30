#include <memory>

#include "draw.hh"
#include "chart.hh"

#include "acmacs-base/range.hh"
#include "acmacs-base/float.hh"
#include "acmacs-draw/surface-cairo.hh"

// ----------------------------------------------------------------------

DrawingOrder::DrawingOrder(Chart& aChart)
    : std::vector<size_t>(Range<size_t>::begin(aChart.number_of_points()), Range<size_t>::end())
{

} // DrawingOrder::DrawingOrder

// ----------------------------------------------------------------------

ChartDraw::ChartDraw(Chart& aChart, size_t aProjectionNo)
    : mChart(aChart),
      mLayout(mChart.projection(aProjectionNo).layout()),
      mPointStyles(mChart.number_of_points()),
      mDrawingOrder(mChart)
{
      // std::cerr << "DrawingOrder: " << mDrawingOrder << std::endl;
    // auto ag_ind = aChart.antigen_indices(), sr_ind = aChart.serum_indices();
    // std::vector<size_t> ag(ag_ind.begin(), ag_ind.end());
    // std::cerr << "AG " << ag << std::endl;
    // std::vector<size_t> sr(sr_ind.begin(), sr_ind.end());
    // std::cerr << "SR " << sr << std::endl;
    // std::cerr << "AGref " << aChart.reference_antigen_indices() << std::endl;
    // std::cerr << "AGegg " << aChart.egg_antigen_indices() << std::endl;
}

// ----------------------------------------------------------------------

void ChartDraw::prepare()
{
    std::unique_ptr<BoundingBall> bb(mLayout.minimum_bounding_ball());
    mViewport.set_from_center_size(bb->center(), bb->diameter());
    mViewport.whole_width();
      // std::cerr << mViewport << std::endl;

    modify(mChart.reference_antigen_indices(), PointStyle(PointStyle::Empty).fill("transparent").size(Pixels{8}));
    modify(mChart.serum_indices(), PointStyle(PointStyle::Empty).shape(PointStyle::Shape::Box).fill("transparent").size(Pixels{8}));
    mark_egg_antigens();
    mark_reassortant_antigens();

} // ChartDraw::prepare

// ----------------------------------------------------------------------

void ChartDraw::draw(Surface& aSurface)
{
    aSurface.grid(Scaled{1}, "grey80", Pixels{1});
    aSurface.border("black", Pixels{1});

    for (size_t index: mDrawingOrder) {
        mPointStyles[index].draw(aSurface, mLayout[index]);
    }

} // ChartDraw::draw

// ----------------------------------------------------------------------

void ChartDraw::draw(std::string aFilename, double aSize)
{
    PdfCairo surface(aFilename, aSize, aSize);
    // surface.resize(mViewport.size.width);
    Surface& rescaled = surface.subsurface({0, 0}, Scaled{surface.viewport().size.width}, mViewport, true);
    draw(rescaled);

} // ChartDraw::draw

// ----------------------------------------------------------------------

void ChartDraw::modify(IndexGenerator&& aGen, const PointStyle& aStyle)
{
    for (auto index: aGen)
        mPointStyles[index] = aStyle;

} // ChartDraw::modify

// ----------------------------------------------------------------------

void ChartDraw::mark_egg_antigens()
{
    modify(mChart.egg_antigen_indices(), PointStyle(PointStyle::Empty).aspect(AspectEgg));

} // ChartDraw::mark_egg_antigens

// ----------------------------------------------------------------------

void ChartDraw::mark_reassortant_antigens()
{
    modify(mChart.reassortant_antigen_indices(), PointStyle(PointStyle::Empty).rotation(RotationReassortant));

} // ChartDraw::mark_reassortant_antigens

// ----------------------------------------------------------------------

void ChartDraw::scale_points(double aPointScale, double aOulineScale)
{
    if (float_zero(aOulineScale))
        aOulineScale = aPointScale;
    for (auto& style: mPointStyles)
        style.scale(aPointScale).scale_outline(aOulineScale);

} // ChartDraw::scale_points

// ----------------------------------------------------------------------

void ChartDraw::mark_all_grey(Color aColor)
{
    modify(mChart.reference_antigen_indices(), PointStyle(PointStyle::Empty).outline(aColor));
    modify(mChart.test_antigen_indices(), PointStyle(PointStyle::Empty).fill(aColor).outline(aColor));
    modify(mChart.serum_indices(), PointStyle(PointStyle::Empty).outline(aColor));

} // ChartDraw::mark_all_grey

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
