#pragma once

#include <string>
#include <vector>

#include "acmacs-draw/viewport.hh"

#include "layout.hh"
#include "point-style.hh"

class Chart;
class Surface;

// ----------------------------------------------------------------------

class DrawingOrder : public std::vector<size_t>
{
 public:
    DrawingOrder(Chart& aChart);

}; // class DrawingOrder

// ----------------------------------------------------------------------

class ChartDraw
{
 public:
    ChartDraw(Chart& aChart, size_t aProjectionNo);

    void prepare();
    void draw(Surface& aSurface);
    void draw(std::string aFilename, double aSize);

 private:
    Chart& mChart;
    Viewport mViewport;
    Layout mLayout;
    std::vector<PointStyle> mPointStyles;
    DrawingOrder mDrawingOrder;

}; // class ChartDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
