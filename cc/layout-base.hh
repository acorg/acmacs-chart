#pragma once

#include "acmacs-base/config.hh"
#include "acmacs-base/transformation.hh"
#include "acmacs-chart/bounding-ball.hh"

// ----------------------------------------------------------------------

class LayoutBase
{
 public:
    inline LayoutBase() = default;
    inline LayoutBase(const LayoutBase&) = default;
    virtual ~LayoutBase();
    virtual LayoutBase* clone() const = 0;

    virtual const Coordinates& operator[](size_t aIndex) const = 0;
      // virtual Coordinates& operator[](size_t aIndex) = 0;
    virtual void set(size_t aIndex, const Coordinates& aCoordinates) = 0;
    virtual size_t number_of_points() const = 0;
    virtual size_t number_of_dimensions() const = 0;
    virtual bool empty() const = 0;

    BoundingBall* minimum_bounding_ball() const;

    inline double distance(size_t p1, size_t p2, double no_distance = std::numeric_limits<double>::quiet_NaN()) const
        {
            const auto& c1 = operator[](p1);
            const auto& c2 = operator[](p2);
            return c1.empty() || c2.empty() ? no_distance : c1.distance(c2);
        }

    void transform(const Transformation& aTransformation);

    void min_max_points(std::vector<size_t>& aMin, std::vector<size_t>& aMax) const;

 private:
    inline void bounding_ball_extend(BoundingBall& aBoundingBall) const
        {
            for (size_t point_no = 0; point_no < number_of_points(); ++point_no)
                aBoundingBall.extend(operator[](point_no));
        }

}; // class LayoutBase

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
