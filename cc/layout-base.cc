#include "acmacs-chart-1/layout-base.hh"

// ----------------------------------------------------------------------

LayoutBase::~LayoutBase()
{
} // LayoutBase::~LayoutBase

// ----------------------------------------------------------------------

BoundingBall* LayoutBase::minimum_bounding_ball() const
{
    const size_t nd = number_of_dimensions();
    std::vector<size_t> min(nd), max(nd);
    min_max_points(min, max);

    Coordinates min_point(nd), max_point(nd);
    for (size_t dim = 0; dim < nd; ++dim) {
        min_point[dim] = (*this)[min[dim]][dim];
        max_point[dim] = (*this)[max[dim]][dim];
    }

    BoundingBall* bb = new BoundingBall(min_point, max_point);
    bounding_ball_extend(*bb);
    return bb;

} // LayoutBase::minimum_bounding_ball

// ----------------------------------------------------------------------

void LayoutBase::transform(const acmacs::Transformation& aTransformation)
{
      // multiply matrices: this x aTransformation
    for (size_t point_no = 0; point_no < number_of_points(); ++point_no) {
        const auto& row = operator[](point_no);
        if (!row.empty()) {     // empty row is for disconnected points
            const auto x = aTransformation.transform(row[0], row[1]); // no C++17 destructurization in cheerp yet
            set(point_no, {x.first, x.second});
        }
    }

} // LayoutBase::transform

// ----------------------------------------------------------------------

void LayoutBase::min_max_points(std::vector<size_t>& aMin, std::vector<size_t>& aMax) const
{
    constexpr const size_t none = static_cast<size_t>(-1);
    std::fill(aMin.begin(), aMin.end(), none);
    std::fill(aMax.begin(), aMax.end(), none);
    for (size_t point_no = 0; point_no < number_of_points(); ++point_no) {
        const auto& point = operator[](point_no);
        for (size_t dim = 0; dim < point.size(); ++dim) {
            const auto v = point[dim];
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

} // LayoutBase::min_max_points

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
