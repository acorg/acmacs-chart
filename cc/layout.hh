#pragma once

#include <iostream>
#include <limits>

#include "acmacs-chart-1/layout-base.hh"

// ----------------------------------------------------------------------

class Layout : public LayoutBase
{
 public:
    inline Layout() = default;
    inline Layout(const Layout&) = default;
    inline Layout* clone() const override { return new Layout(*this); }

    inline const Coordinates& operator[](size_t aIndex) const override { return mCoordinates[aIndex]; }
    // inline Coordinates& operator[](size_t aIndex) override { return mCoordinates[aIndex]; }
    inline void set(size_t aIndex, const Coordinates& aCoordinates) override { mCoordinates[aIndex] = aCoordinates; }
    inline bool empty() const override { return mCoordinates.empty(); }

    inline size_t number_of_points() const override { return mCoordinates.size(); }

    inline size_t number_of_dimensions() const override
        {
            for (const auto& coordinates: mCoordinates) {
                const size_t num_dims = coordinates.size();
                if (num_dims)
                    return num_dims;
            }
            THROW(std::runtime_error("getting number_of_dimensions for empty layout"), 0);
        }

    inline std::vector<Coordinates>& data() { return mCoordinates; }
    inline const std::vector<Coordinates>& data() const { return mCoordinates; }

 private:
    std::vector<Coordinates> mCoordinates;

}; // class Layout

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
