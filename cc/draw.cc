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
      // std::cerr << mViewport << std::endl;

} // ChartDraw::prepare

// ----------------------------------------------------------------------

void ChartDraw::draw(Surface& aSurface)
{
    double pix = 0.01;
    aSurface.grid(1, "cyan3", pix);
    aSurface.border("blue", pix * 5);
    aSurface.circle({0, 0}, 1, 1, 0, "pink", pix);

} // ChartDraw::draw

// ----------------------------------------------------------------------

void ChartDraw::draw(std::string aFilename, double aSize)
{
    PdfCairo surface(aFilename, aSize, aSize);
    // surface.resize(mViewport.size.width);
    Surface& rescaled = surface.subsurface({0, 0}, surface.size(), mViewport.size.width, true);
    rescaled.viewport_offset(mViewport.offset());
    draw(rescaled);

} // ChartDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
