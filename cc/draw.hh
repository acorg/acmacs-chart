#pragma once

#include <string>
#include <vector>

#include "acmacs-draw/viewport.hh"

#include "layout.hh"
#include "point-style.hh"

class Chart;
class Surface;
class IndexGenerator;

// ----------------------------------------------------------------------

class DrawingOrder : public std::vector<size_t>
{
 public:
    DrawingOrder(Chart& aChart);

    void raise(size_t aPointNo);
    void lower(size_t aPointNo);

}; // class DrawingOrder

// ----------------------------------------------------------------------

class ChartDraw
{
 public:
    ChartDraw(Chart& aChart, size_t aProjectionNo);

    void prepare();
    void draw(Surface& aSurface);
    void draw(std::string aFilename, double aSize);

    void modify(IndexGenerator&& aGen, const PointStyle& aStyle);

    inline void modify_point_by_index(size_t aIndex, const PointStyle& aStyle, bool aRaise = false, bool aLower = false)
        {
            mPointStyles[aIndex] = aStyle;
            if (aRaise)
                drawing_order().raise(aIndex);
            else if (aLower)
                drawing_order().lower(aIndex);
        }

    void mark_egg_antigens();
    void mark_reassortant_antigens();
    void mark_all_grey(Color aColor);
    void scale_points(double aPointScale, double aOulineScale);
    void rotate(double aAngle);

    DrawingOrder& drawing_order() { return mDrawingOrder; }

    inline void background_color(Color aBackgroud) { mBackgroud = aBackgroud; }
    inline void grid(Color aGridColor, double aGridLineWidth) { mGridColor = aGridColor; mGridLineWidth = aGridLineWidth; }
    inline void border(Color aBorderColor, double aBorderWidth) { mBorderColor = aBorderColor; mBorderWidth = aBorderWidth; }

 private:
    Chart& mChart;
    size_t mProjectionNo;
    Viewport mViewport;
    Transformation mTransformation;
    std::vector<PointStyle> mPointStyles;
    DrawingOrder mDrawingOrder;
    Color mBackgroud;
    Color mGridColor;
    Pixels mGridLineWidth;
    Color mBorderColor;
    Pixels mBorderWidth;

}; // class ChartDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
