#include <memory>

#include "draw.hh"
#include "chart.hh"

#include "acmacs-draw/surface-cairo.hh"

// ----------------------------------------------------------------------

void ChartDraw::prepare()
{
    std::unique_ptr<BoundingBall> bb(mChart.projection(mProjectionNo).layout().minimum_bounding_ball());
    mViewport.set_from_center_size(bb->center(), bb->diameter());
    mViewport.whole_width();
    std::cerr << mViewport << std::endl;

} // ChartDraw::prepare

// ----------------------------------------------------------------------

void ChartDraw::draw(Surface& aSurface)
{
    aSurface.grid(100, "cyan3", 3);
    aSurface.border("blue", 5);

} // ChartDraw::draw

// ----------------------------------------------------------------------

void ChartDraw::draw(std::string aFilename, double aSize)
{
    PdfCairo surface(aFilename, aSize, aSize);

    draw(surface);

} // ChartDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
