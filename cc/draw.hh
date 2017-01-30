#pragma once

#include <string>

#include "acmacs-draw/viewport.hh"

#include "point-style.hh"

class Chart;
class Surface;

// ----------------------------------------------------------------------

class ChartDraw
{
 public:
    inline ChartDraw(Chart& aChart, size_t aProjectionNo) : mChart(aChart), mProjectionNo(aProjectionNo) {}

    void prepare();
    void draw(Surface& aSurface);
    void draw(std::string aFilename, double aSize);

 private:
    Chart& mChart;
    size_t mProjectionNo;
    Viewport mViewport;

}; // class ChartDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
