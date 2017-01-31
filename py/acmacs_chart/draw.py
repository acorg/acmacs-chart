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

def draw_chart(output_file, chart, settings, hidb_dir, output_width):
    chart_draw = ChartDraw(chart)
    chart_draw.prepare()
    #chart_draw.scale_points(3)
    chart_draw.all_grey()
    # chart_draw.modify_point_by_index(0, PointStyle().fill("blue").outline("black").size(20))
    # chart_draw.modify_point_by_index(0, make_point_style({"fill": "blue", "outline": "black", "size": 20}))
    # chart_draw.modify_point_by_index(10, acmacs_chart.PointStyle(fill="red", outline="black", size=20))
    mark_vaccines(chart_draw=chart_draw, chart=chart, hidb_dir=hidb_dir)
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

def mark_vaccines(chart_draw, chart, hidb_dir, style={"size": 15}, raise_=True):
    hidb = get_hidb(chart=chart, hidb_dir=hidb_dir)
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
