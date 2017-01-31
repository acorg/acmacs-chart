# -*- Python -*-
# license
# license.
# ----------------------------------------------------------------------

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

def mark_vaccines(chart_draw, chart, hidb_dir):
    hidb = get_hidb(chart=chart, hidb_dir=hidb_dir)
    for vaccine_entry in vaccines(chart=chart):
        antigens = chart.vaccines(vaccine_entry["name"], hidb)
        for vaccine_data, name in [[antigens.egg(), "EGG:"], [antigens.cell(), "CELL:"], [antigens.reassortant(), "REASSORTANT:"]]:
            if vaccine_data:
                print(name, vaccine_data.antigen_index, vaccine_data.antigen.full_name())
                # for es in vaccine_data.homologous_sera:
                #     print("  ", es.serum.full_name(), " tables:", es.number_of_tables(), " most-recent:", es.most_recent_table, " species:", es.serum.serum_species(), sep="")

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
