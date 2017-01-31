# -*- Python -*-
# license
# license.
# ----------------------------------------------------------------------

from .hidb_access import get_hidb
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
    chart_draw.draw(str(output_file), output_width)

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
