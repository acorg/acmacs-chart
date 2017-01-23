#include "layout.hh"

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
    std::fill(aMin.begin(), aMin.end(), 0);
    std::fill(aMax.begin(), aMax.end(), 0);
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
                if (v < (*this)[aMin[dim]][dim])
                    aMin[dim] = point_no;
                if (v > (*this)[aMax[dim]][dim])
                    aMax[dim] = point_no;
            }
        }
    }

} // Layout::min_max_points

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
