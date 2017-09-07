#pragma once

#include <string>
#include <vector>

#include "acmacs-base/throw.hh"

// ----------------------------------------------------------------------

class LabelStyle
{
 public:
    enum class Slant {Normal, Italic, Oblique};
    enum class Weight {Normal, Bold};

    inline LabelStyle() : mShown(true), mSlant(Slant::Normal), mWeight(Weight::Normal), mSize(1), mColor("black"), mRotation(0), mInterline(0.2) {}

    inline void shown(bool aShown) { mShown = aShown; }
    inline bool shown() const { return mShown; }

    inline std::vector<double>& position() { return mPosition; }
    inline const std::vector<double>& position() const { return mPosition; }

    inline void text(const char* str, size_t length) { mText.assign(str, length); }
    inline std::string text() const { return mText; }

    inline void face(const char* str, size_t length) { mFace.assign(str, length); }
    inline std::string face() const { return mFace; }

    inline void slant(const char* str, size_t length) { mSlant = slant_from_string(str, length); }
    inline Slant slant() const { return mSlant; }
    inline std::string slant_as_stirng() const
        {
            switch (mSlant) {
              case Slant::Normal:
                  return "normal";
              case Slant::Italic:
                  return "italic";
              case Slant::Oblique:
                  return "oblique";
            }
            return "?";
        }

    inline void weight(const char* str, size_t length) { mWeight = weight_from_string(str, length); }
    inline Weight weight() const { return mWeight; }
    inline std::string weight_as_stirng() const
        {
            switch (mWeight) {
              case Weight::Normal:
                  return "normal";
              case Weight::Bold:
                  return "bold";
            }
            return "?";
        }

    inline void size(double aSize) { mSize = aSize; }
    inline double size() const { return mSize; }

    inline void color(const char* str, size_t length) { mColor.assign(str, length); }
    inline std::string color() const { return mColor; }

    inline void rotation(double aRotation) { mRotation = aRotation; }
    inline double rotation() const { return mRotation; }

    inline void interline(double aInterline) { mInterline = aInterline; }
    inline double interline() const { return mInterline; }

 private:
    bool mShown;                   // "+"
    std::vector<double> mPosition; // "p": [0.0, 1.0] label position (2D only), list of two doubles, default is [0, 1] means under point
    std::string mText;             // "t": "label text if forced by user",
    std::string mFace;             // "f": "font face",
    Slant mSlant;                  // "S": "normal OR italic OR oblique", // font slant, default normal
    Weight mWeight;                // "W": "normal OR bold", // font weight, default normal
    double mSize;                  // "s": 1.0,           // size, default 1.0
    std::string mColor;            // "c": "black",   // color, default black
    double mRotation;              // "r": 0.0,       // rotation, default 0.0
    double mInterline;             // "i": 0.2, // addtional interval between lines as a fraction of line height, default - 0.2

    inline Slant slant_from_string(const char* str, size_t length) const
        {
            const std::string s{str, length};
            if (s == "normal")
                return Slant::Normal;
            if (s == "italic")
                return Slant::Italic;
            if (s == "oblique")
                return Slant::Oblique;
            THROW(std::runtime_error("Unrecognized font slant: " + s), Slant::Normal);
        }

    inline Weight weight_from_string(const char* str, size_t length) const
        {
            const std::string s{str, length};
            if (s == "normal")
                return Weight::Normal;
            if (s == "bold")
                return Weight::Bold;
            THROW(std::runtime_error("Unrecognized font weight: " + s), Weight::Normal);
        }

}; // class LabelStyle

// ----------------------------------------------------------------------

class ChartPlotSpecStyle
{
 public:
    enum Shape {Circle, Box, Triangle};

    inline ChartPlotSpecStyle() : mShown(true), mFillColor("transparent"), mOutlineColor("black"), mOutlineWidth(1), mShape(Circle), mSize(1), mRotation(0), mAspect(1) {}

    inline void shown(bool aShown) { mShown = aShown; }
    inline bool shown() const { return mShown; }

    inline void fill_color(const char* str, size_t length) { mFillColor.assign(str, length); }
    inline std::string fill_color() const { return mFillColor; }

    inline void outline_color(const char* str, size_t length) { mOutlineColor.assign(str, length); }
    inline std::string outline_color() const { return mOutlineColor; }

    inline void outline_width(double aOutlineWidth) { mOutlineWidth = aOutlineWidth; }
    inline double outline_width() const { return mOutlineWidth; }

    inline void shape(const char* str, size_t length) { mShape = shape_from_string(str, length); }
    inline Shape shape() const { return mShape; }
    inline std::string shape_as_string() const
        {
            switch(mShape) {
              case Circle:
                  return "C";
              case Box:
                  return "B";
              case Triangle:
                  return "T";
            }
            return "?";
        }

    inline void size(double aSize) { mSize = aSize; }
    inline double size() const { return mSize; }

    inline void rotation(double aRotation) { mRotation = aRotation; }
    inline double rotation() const { return mRotation; }

    inline void aspect(double aAspect) { mAspect = aAspect; }
    inline double aspect() const { return mAspect; }

    inline LabelStyle& label() { return mLabel; }
    inline const LabelStyle& label() const { return mLabel; }

 private:
    bool mShown;               // "+"
    std::string mFillColor;    //  "F": "fill color: #FF0000 or T[RANSPARENT] or color name (red, green, blue, etc.), default is transparent",
    std::string mOutlineColor; // "O": "outline color: #000000 or T[RANSPARENT] or color name (red, green, blue, etc.), default is black",
    double mOutlineWidth;      // "o": 1.0,             // outline width, default 1.0
    Shape mShape;              // "S": "shape: C[IRCLE], B[OX], T[RIANGLE], default is circle",
    double mSize;              // "s": 1.0,             // size, default is 1.0
    double mRotation;          // "r": 0.0,             // rotation in radians, default is 0.0
    double mAspect;            // "a": 1.0,              // aspect ratio, default is 1.0
    LabelStyle mLabel;         // "l"

    inline Shape shape_from_string(const char* str, size_t length) const
        {
            switch (*str) {
              case 'C':
              case 'c':
                  return Circle;
              case 'B':
              case 'b':
                  return Box;
              case 'T':
              case 't':
                  return Triangle;
            }
            THROW(std::runtime_error("Unrecognized point shape: " + std::string(str, length)), Circle);
        }

}; // class ChartPlotSpecStyle

// ----------------------------------------------------------------------

class ChartPlotSpec
{
 public:
    inline ChartPlotSpec() {}

    inline std::vector<size_t>& drawing_order() { return mDrawingOrder; }
    inline const std::vector<size_t>& drawing_order() const { return mDrawingOrder; }

    inline std::vector<size_t>& style_for_point() { return mStyleForPoint; }
    inline const std::vector<size_t>& style_for_point() const { return mStyleForPoint; }

    inline std::vector<ChartPlotSpecStyle>& styles() { return mStyles; }
    inline const std::vector<ChartPlotSpecStyle>& styles() const { return mStyles; }

    inline std::vector<size_t>& shown_on_all() { return mShownOnAll; }
    inline const std::vector<size_t>& shown_on_all() const { return mShownOnAll; }

    inline bool empty() const { return mDrawingOrder.empty() && mStyleForPoint.empty() && mStyles.empty() && mShownOnAll.empty(); }

    inline const ChartPlotSpecStyle& style_for(size_t aPointNo) const
        {
            return styles().at(style_for_point().at(aPointNo));
        }

 private:
    std::vector<size_t> mDrawingOrder;       // "d"
    std::vector<size_t> mStyleForPoint;      // "p"
    std::vector<ChartPlotSpecStyle> mStyles; // "P"
    std::vector<size_t> mShownOnAll;         // "s"

}; // class ChartPlotSpec

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
