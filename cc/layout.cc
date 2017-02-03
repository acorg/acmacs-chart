#include <cmath>

#include "layout.hh"

// ----------------------------------------------------------------------

void Transformation::rotate(double aAngle)
{
    const double cos = std::cos(aAngle);
    const double sin = std::sin(aAngle);
    const double r0 = cos * (*this)[0] + -sin * (*this)[2];
    const double r1 = cos * (*this)[1] + -sin * (*this)[3];
    const double r2 = sin * (*this)[0] +  cos * (*this)[2];
    const double r3 = sin * (*this)[1] +  cos * (*this)[3];
    (*this)[0] = r0;
    (*this)[1] = r1;
    (*this)[2] = r2;
    (*this)[3] = r3;

} // Transformation::rotate

// ----------------------------------------------------------------------

void Transformation::flip(double aX, double aY)
{
      // vector [aX, aY] must be first transformed using this
    const double x = aX * (*this)[0] + aY * (*this)[2];
    const double y = aX * (*this)[1] + aY * (*this)[3];

    const double x2y2 = x * x - y * y, xy = 2 * x * y;
    const double r0 = x2y2 * (*this)[0] + xy    * (*this)[2];
    const double r1 = x2y2 * (*this)[1] + xy    * (*this)[3];
    const double r2 = xy   * (*this)[0] + -x2y2 * (*this)[2];
    const double r3 = xy   * (*this)[1] + -x2y2 * (*this)[3];
    (*this)[0] = r0;
    (*this)[1] = r1;
    (*this)[2] = r2;
    (*this)[3] = r3;

} // Transformation::flip

// ----------------------------------------------------------------------

BoundingBall* Layout::minimum_bounding_ball() const
{
    const size_t nd = number_of_dimensions();
    std::vector<size_t> min(nd), max(nd);
    min_max_points(min, max);

    Point min_point(nd), max_point(nd);
    for (size_t dim = 0; dim < nd; ++dim) {
        min_point[dim] = (*this)[min[dim]][dim];
        max_point[dim] = (*this)[max[dim]][dim];
    }

    BoundingBall* bb = new BoundingBall(min_point, max_point);
    bounding_ball_extend(*bb);
    return bb;

} // Layout::minimum_bounding_ball

// ----------------------------------------------------------------------

void Layout::min_max_points(std::vector<size_t>& aMin, std::vector<size_t>& aMax) const
{
    constexpr const size_t none = static_cast<size_t>(-1);
    std::fill(aMin.begin(), aMin.end(), none);
    std::fill(aMax.begin(), aMax.end(), none);
    for (auto point = cbegin(); point != cend(); ++point) {
        const size_t point_no = static_cast<size_t>(std::distance(cbegin(), point));
        for (size_t dim = 0; dim < point->size(); ++dim) {
            const auto v = (*point)[dim];
            if (std::isnan(v)) {
                  // Move aMin and aMax, if they refer to a point with NaN coordinates
                if (aMin[dim] == point_no)
                    aMin[dim] = point_no + 1;
                if (aMax[dim] == point_no)
                    aMax[dim] = point_no + 1;
            }
            else {
                if (aMin[dim] == none || v < (*this)[aMin[dim]][dim])
                    aMin[dim] = point_no;
                if (aMax[dim] == none || v > (*this)[aMax[dim]][dim])
                    aMax[dim] = point_no;
            }
        }
    }

} // Layout::min_max_points

// ----------------------------------------------------------------------

void Layout::transform(const Transformation& aTransformation)
{
      // multiply matrices: this x aTransformation
    for (auto& row: *this) {
        if (!row.empty()) {     // empty row is for disconnected points
            const double x1 = row[0] * aTransformation[0] + row[1] * aTransformation[2];
            const double x2 = row[0] * aTransformation[1] + row[1] * aTransformation[3];
            row[0] = x1;
            row[1] = x2;
        }
    }

} // Layout::transform

// ----------------------------------------------------------------------

void BoundingBall::extend(const Point& aPoint)
{
    const double distance2_to_center = distance2FromCenter(aPoint);
    if (distance2_to_center > radius2()) {
        const double dist = std::sqrt(distance2_to_center);
        mDiameter = mDiameter * 0.5 + dist;
        const double difference = dist - mDiameter * 0.5;
        Vector::const_iterator p = aPoint.begin();
        for (Vector::iterator c = mCenter.begin(); c != mCenter.end(); ++c, ++p)
            *c = (mDiameter * 0.5 * (*c) + difference * (*p)) / dist;
    }

} // BoundingBall::extend

// ----------------------------------------------------------------------

void BoundingBall::extend(const BoundingBall& aBoundingBall)
{
    Vector new_center(center());
    new_center.add(aBoundingBall.center());
    new_center.multiply_by(0.5);
    set(new_center, diameter() + center().distance(aBoundingBall.center()));

} // BoundingBall::extend

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
