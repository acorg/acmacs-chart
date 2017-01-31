#pragma once

#include <iostream>

#include "acmacs-base/stream.hh"
#include "vector.hh"

// ----------------------------------------------------------------------

using Point = Vector;

class BoundingBall
{
 public:
    inline BoundingBall() = default;
      // aP1 and aP2 are opposite corners of some area, draw a minimal circle through them
    inline BoundingBall(const Point& aP1, const Point& aP2)
        : mCenter(aP1.size()), mDiameter(0)
        {
            Vector v{aP1};
            v.subtract(aP2);
            mDiameter = v.eucledian_norm();
            for (Vector::size_type dim = 0; dim < aP1.size(); ++dim)
                mCenter[dim] = (aP1[dim] + aP2[dim]) * 0.5;
        }

    inline const Vector& center() const { return mCenter; }
    inline double diameter() const { return mDiameter; }

    inline void set(const Vector& aCenter, double aDiameter)
        {
            mCenter = aCenter;
            mDiameter = aDiameter;
        }

      // The same as *this=BoundingBall(aP1, aP2, aDistance) but avoids assignment
    inline void move(const Vector& aP1, const Vector& aP2, double aDistance)
        {
            mDiameter = aDistance;
            mCenter.resize(aP1.size());
            for (Vector::size_type dim = 0; dim < aP1.size(); ++dim)
                mCenter[dim] = (aP1[dim] + aP2[dim]) * 0.5;
        }

    inline void moveCenter(const Vector& aBy)
        {
            mCenter.add(aBy);
        }

      // Extends bounding ball (change center and diameter) to make sure the bounding ball includes the passed point
    void extend(const Point& aPoint);

      // Extends bounding ball to make sure it includes all points of the second boundig ball
    void extend(const BoundingBall& aBoundingBall);

    inline double top() const { return mCenter[1] - mDiameter / 2.0; }
    inline double bottom() const { return mCenter[1] + mDiameter / 2.0; }
    inline double left() const { return mCenter[0] - mDiameter / 2.0; }
    inline double right() const { return mCenter[0] + mDiameter / 2.0; }

 private:
    Vector mCenter;
    double mDiameter;

      // Returns distance^2 from the ball center to point
    inline double distance2FromCenter(const Point& aPoint) const
        {
            Vector v(aPoint);
            v.subtract(mCenter);
            return v.inner_product();
        }

    inline double radius2() const
        {
            return mDiameter * mDiameter * 0.25;
        }

}; // class BoundingBall

inline std::ostream& operator << (std::ostream& out, const BoundingBall& a) { return out << "BoundingBall(" << a.center() << ", " << a.diameter() << ")"; }

// ----------------------------------------------------------------------

class Transformation : public std::vector<double>
{
 public:
    inline Transformation() : std::vector<double>{{1, 0, 0, 1}} {}
    void rotate(double aAngle);
};

// ----------------------------------------------------------------------

class Layout : public std::vector<Point>
{
 public:
    inline Layout() = default;

    BoundingBall* minimum_bounding_ball() const;
    void transform(const Transformation& aTransformation);

    inline size_t number_of_dimensions() const { return front().size(); }

 private:
    void min_max_points(std::vector<size_t>& aMin, std::vector<size_t>& aMax) const;

    inline void bounding_ball_extend(BoundingBall& aBoundingBall) const
        {
            for (const auto& point: *this)
                aBoundingBall.extend(point);
        }

}; // class Layout

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
