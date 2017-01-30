#pragma once

#include <string>
#include <limits>

#include "acmacs-draw/surface.hh"

#include "layout.hh"

// ----------------------------------------------------------------------

class Surface;

#include "acmacs-base/global-constructors-push.hh"

const Aspect AspectRegular{1.0};
const Aspect AspectEgg{0.75};
const Aspect AspectNoChange{std::numeric_limits<double>::quiet_NaN()};

const Rotation RotationRegular{0.0};
const Rotation RotationReassortant{0.5};
const Rotation RotationNoChange{std::numeric_limits<double>::quiet_NaN()};

#include "acmacs-base/diagnostics-pop.hh"

// ----------------------------------------------------------------------

class PointStyle
{
 public:
    enum class Shown { Hidden, Shown, NoChange };
    enum class Shape { NoChange, Circle, Box, Triangle };

    inline PointStyle()
        : mShown(Shown::Shown), mShape(Shape::Circle), mFill("green"), mOutline("black"),
          mSize(5), mOutlineWidth(1), mAspect(AspectRegular), mRotation(RotationRegular) {}
    PointStyle& operator = (const PointStyle& aPS);

    void draw(Surface& aSurface, const Point& aCoord);

 private:
    Shown mShown;
    Shape mShape;
    Color mFill;
    Color mOutline;
    Pixels mSize;
    Pixels mOutlineWidth;
    Aspect mAspect;
    Rotation mRotation;

}; // class PointStyle

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
