#pragma once

#include <string>

class Chart;
class Surface;

// ----------------------------------------------------------------------

class ChartDraw
{
 public:
    inline ChartDraw(Chart& aChart) : mChart(aChart) {}

    void prepare();
    void draw(Surface& aSurface);
    void draw(std::string aFilename, double aSize);

 private:
    Chart& mChart;

}; // class ChartDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
