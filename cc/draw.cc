#include "draw.hh"

#include "acmacs-draw/surface-cairo.hh"

// ----------------------------------------------------------------------

void ChartDraw::prepare()
{

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
