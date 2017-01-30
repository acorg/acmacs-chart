#include <memory>

#include "draw.hh"
#include "chart.hh"

#include "acmacs-base/range.hh"
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
    std::cerr << "DrawingOrder: " << mDrawingOrder << std::endl;
}

// ----------------------------------------------------------------------

void ChartDraw::prepare()
{
    std::unique_ptr<BoundingBall> bb(mLayout.minimum_bounding_ball());
    mViewport.set_from_center_size(bb->center(), bb->diameter());
    mViewport.whole_width();
      // std::cerr << mViewport << std::endl;

} // ChartDraw::prepare

// ----------------------------------------------------------------------

void ChartDraw::draw(Surface& aSurface)
{
    double pix = 1;
    aSurface.grid(Scaled{1}, "cyan3", Pixels{pix});
    aSurface.border("blue", Pixels{pix * 5});
    aSurface.circle(mViewport.center(), Scaled{1}, 1, 0, "pink", Pixels{pix});

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


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End: