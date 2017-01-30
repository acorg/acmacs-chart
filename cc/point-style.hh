#pragma once

#include <string>

#include "acmacs-draw/color.hh"
#include "acmacs-draw/size-scale.hh"

#include "layout.hh"

// ----------------------------------------------------------------------

class Aspect
{
 public:
    inline Aspect(double a = 1.0) : mValue(a) {}
    inline operator double() const { return mValue; }

 private:
    double mValue;
};

class Rotation
{
 public:
    inline Rotation(double a = 0.0) : mValue(a) {}
    inline operator double() const { return mValue; }

 private:
    double mValue;
};

#include "acmacs-base/global-constructors-push.hh"

const Aspect AspectRegular{1.0};
const Aspect AspectEgg{0.75};

const Rotation RotationRegular{0.0};
const Rotation RotationReassortant{0.5};

#include "acmacs-base/diagnostics-pop.hh"

// ----------------------------------------------------------------------

class Surface;

class PointStyle
{
 public:
    enum class Shape { NoChange, Circle, Box, Triangle };

    inline PointStyle()
        : mShown(true), mShape(Shape::Circle), mFill("green"), mOutline("black"),
          mSize(5), mOutlineWidth(1), mAspect(AspectRegular), mRotation(RotationRegular) {}

    void draw(Surface& aSurface, const Point& aCoord);

 private:
    bool mShown;
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
