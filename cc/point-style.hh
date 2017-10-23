#pragma once

#include <string>
#include <limits>
#include <iostream>

#include "acmacs-base/throw.hh"
#include "acmacs-base/color-target.hh"
#include "acmacs-base/size-scale.hh"
#include "acmacs-chart/layout.hh"

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
    enum Empty { Empty };

    inline PointStyle()
        : mShown(Shown::Shown), mShape(Shape::Circle), mFill("green"), mOutline("black"),
          mSize(5), mOutlineWidth(1), mAspect(AspectRegular), mRotation(RotationRegular) {}
    inline PointStyle(enum Empty)
        : mShown(Shown::NoChange), mShape(Shape::NoChange), mFill(ColorNoChange), mOutline(ColorNoChange),
          mSize(Pixels::make_empty()), mOutlineWidth(Pixels::make_empty()), mAspect(AspectNoChange), mRotation(RotationNoChange) {}
    inline PointStyle(const PointStyle& aPS) = default;

    inline PointStyle& operator = (const PointStyle& aPS)
        {
            if (aPS.mShown != Shown::NoChange)
                mShown = aPS.mShown;
            if (aPS.mShape != Shape::NoChange)
                mShape = aPS.mShape;
            if (!aPS.mFill.empty())
                mFill = aPS.mFill;
            if (!aPS.mOutline.empty())
                mOutline = aPS.mOutline;
            if (!aPS.mSize.empty())
                mSize = aPS.mSize;
            if (!aPS.mOutlineWidth.empty())
                mOutlineWidth = aPS.mOutlineWidth;
            if (!aPS.mAspect.empty())
                mAspect = aPS.mAspect;
            if (!aPS.mRotation.empty())
                mRotation = aPS.mRotation;
            return *this;
        }

    inline PointStyle& show(Shown aShown = Shown::Shown) { mShown = aShown; return *this; }
    inline PointStyle& hide() { mShown = Shown::Hidden; return *this; }
    inline PointStyle& shape(Shape aShape) { mShape = aShape; return *this; }
    inline PointStyle& shape(std::string aShape) { if (aShape == "circle" || aShape == "C") shape(Shape::Circle); else if (aShape == "box" || aShape == "B") shape(Shape::Box); else if (aShape == "triangle" || aShape == "T") shape(Shape::Triangle); else THROW(std::runtime_error("Unrecognized point style shape: " + aShape), *this); return *this; }
    inline PointStyle& fill(Color c) { mFill = c; return *this; }
    inline PointStyle& fill(std::string aColor) { mFill = aColor; return *this; }
    inline PointStyle& fill(const char* aColor) { mFill = aColor; return *this; }
    inline PointStyle& outline(Color c) { mOutline = c; return *this; }
    inline PointStyle& outline(std::string aColor) { mOutline = aColor; return *this; }
    inline PointStyle& outline(const char* aColor) { mOutline = aColor; return *this; }
    inline PointStyle& size(Pixels aSize) { mSize = aSize; return *this; }
    inline PointStyle& outline_width(Pixels aOutlineWidth) { mOutlineWidth = aOutlineWidth; return *this; }
    inline PointStyle& aspect(Aspect aAspect) { mAspect = aAspect; return *this; }
    inline PointStyle& aspect(double aAspect) { mAspect = aAspect; return *this; }
    inline PointStyle& rotation(Rotation aRotation) { mRotation = aRotation; return *this; }
    inline PointStyle& rotation(double aRotation) { mRotation = aRotation; return *this; }

    inline PointStyle& scale(double aScale) { mSize *= aScale; return *this; }
    inline PointStyle& scale_outline(double aScale) { mOutlineWidth *= aScale; return *this; }

    inline bool shown() const { return mShown == Shown::Shown; }
    inline Shape shape() const { return mShape; }
    inline Pixels size() const { return mSize; }
    inline std::string fill() const { return mFill.to_string(); }
    inline std::string outline() const { return mOutline.to_string(); }
    inline std::string fill_hex() const { return mFill.to_hex_string(); }
    inline std::string outline_hex() const { return mOutline.to_hex_string(); }
    inline Pixels outline_width() const { return mOutlineWidth; }
    inline Aspect aspect() const { return mAspect; }
    inline Rotation rotation() const { return mRotation; }

    inline Shown shown_raw() const { return mShown; }
    inline Color fill_raw() const { return mFill; }
    inline Color outline_raw() const { return mOutline; }

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

inline PointStyle PointStyleEmpty()
{
    return {PointStyle::Empty};
}

// ----------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const PointStyle& aStyle)
{
    out << "PointStyle(fill=\"" << aStyle.fill() << "\", outline=\"" << aStyle.outline() << "\", size=" << aStyle.size()
        << ", aspect=" << aStyle.aspect() << ", rotation=" << aStyle.rotation() << ")";
    return out;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
