#pragma once

#include <string>
#include <vector>

// ----------------------------------------------------------------------

class Vaccine
{
 public:
    enum Type { Previous, Current, Surrogate };

    inline Vaccine(std::string aName, Type aType) : name(aName), type(aType) {}

    std::string name;
    Type type;

    std::string type_as_string() const;
    
}; // class Vaccine

// ----------------------------------------------------------------------

class Chart;

const std::vector<Vaccine>& vaccines(std::string aSubtype, std::string aLineage);
const std::vector<Vaccine>& vaccines(const Chart& aChart);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
