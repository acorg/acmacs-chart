# -*- Python -*-
# license
# license.
# ----------------------------------------------------------------------

import copy
import logging; module_logger = logging.getLogger(__name__)
from .hidb_access import get_hidb
from .vaccines import vaccines
from acmacs_chart_backend import ChartDraw, PointStyle

# ----------------------------------------------------------------------

def draw_chart(output_file, chart, settings, output_width):
    chart_draw = ChartDraw(chart)
    chart_draw.prepare()
    # chart_draw.background_color("green")
    # chart_draw.grid("red", 1)
    # chart_draw.border("orange", 2)
    chart_draw.mark_egg_antigens()
    chart_draw.mark_reassortant_antigens()
    #chart_draw.scale_points(3)
    chart_draw.all_grey()
    # chart_draw.modify_point_by_index(0, PointStyle().fill("blue").outline("black").size(20))
    # chart_draw.modify_point_by_index(0, make_point_style({"fill": "blue", "outline": "black", "size": 20}))
    # chart_draw.modify_point_by_index(10, acmacs_chart.PointStyle(fill="red", outline="black", size=20))
    # chart_draw.rotate(1.57)
    # chart_draw.flip_ns()
    # chart_draw.flip_ew()
    # chart_draw.flip(-1, 1)                # flip about diagonal from [0,0] to [1,1], i.e. flip in direction [-1,1]

    mark_continents(chart_draw=chart_draw, chart=chart)
    mark_vaccines(chart_draw=chart_draw, chart=chart)
    chart_draw.draw(str(output_file), output_width)

# ----------------------------------------------------------------------

sStyleByVaccineType = {
    "previous": {"fill": "blue", "outline": "black"},
    "current": {"fill": "red", "outline": "black"},
    "surrogate": {"fill": "pink", "outline": "black"},
    }

sStyleByPassageType = {
    "egg": {"aspect": 0.75},
    "reassortant": {"aspect": 0.75, "rotation": 0.5},
    "cell": {}
    }

def mark_vaccines(chart_draw, chart, style={"size": 15}, raise_=True):
    hidb = get_hidb(chart=chart)
    for vaccine_entry in vaccines(chart=chart):
        # module_logger.debug('{}'.format(vaccine_entry))
        antigens = chart.vaccines(vaccine_entry["name"], hidb)
        for passage_type in ["egg", "reassortant", "cell"]:
            vaccine_data = getattr(antigens, passage_type)()
            if vaccine_data:
                vstyle = copy.deepcopy(sStyleByVaccineType[vaccine_entry["type"]])
                vstyle.update(sStyleByPassageType[passage_type])
                if style:
                    vstyle.update(style)
                module_logger.info('Marking vaccine {} {}'.format(vaccine_data.antigen_index, vaccine_data.antigen.full_name()))
                chart_draw.modify_point_by_index(vaccine_data.antigen_index, make_point_style(vstyle), raise_=raise_)

# ----------------------------------------------------------------------

sStyleByContinent = {
    "EUROPE":            {"fill": "green"},
    "CENTRAL-AMERICA":   {"fill": "#AAF9FF"},
    "MIDDLE-EAST":       {"fill": "#8000FF"},
    "NORTH-AMERICA":     {"fill": "blue4"},
    "AFRICA":            {"fill": "darkorange1"},
    "ASIA":              {"fill": "red"},
    "RUSSIA":            {"fill": "maroon"},
    "AUSTRALIA-OCEANIA": {"fill": "hotpink"},
    "SOUTH-AMERICA":     {"fill": "turquoise"},
    "ANTARCTICA":        {"fill": "grey50"},
    "":                  {"fill": "grey50"},
    }

def mark_continents(chart_draw, chart):
    from .locdb_access import get_locdb
    data = chart.antigens().continents(get_locdb())
    module_logger.info('[Continents] {}'.format(" ".join(f"{continent}:{len(data[continent])}" for continent in sorted(data))))
    global sStyleByContinent
    for continent, indices in data.items():
        chart_draw.modify_points_by_indices(indices, make_point_style(sStyleByContinent[continent]))
    chart_draw.continent_map([0, 0], 200)

# ----------------------------------------------------------------------

def make_point_style(data):
    ps = PointStyle()
    for k, v in data.items():
        setter = getattr(ps, k, None)
        if setter:
            setter(v)
    return ps

# ----------------------------------------------------------------------
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
