#include "point-style.hh"

#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

void PointStyle::draw(Surface& aSurface, const Point& aCoord)
{
    if (mShown) {
      // aSurface.circle(mViewport.center(), Scaled{1}, 1, 0, "pink", Pixels{pix});
      // std::cerr << "draw " << aCoord << std::endl;
        aSurface.circle_filled(aCoord, Pixels{5}, 1, 0, "magenta", Pixels{1}, "pink");
    }

} // PointStyle::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
