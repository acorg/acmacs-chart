# -*- Python -*-
# license
# license.
# ----------------------------------------------------------------------

import os
from pathlib import Path
from acmacs_base.timeit import timeit
from acmacs_chart_backend import LocDb

# ----------------------------------------------------------------------

def get_locdb(locdb_file :Path = Path(os.environ["ACMACSD_ROOT"], "data", "locationdb.json.xz")):
    global sLocDb
    if sLocDb is None:
        with timeit("Loading locationdb from " + str(locdb_file)):
            sLocDb = LocDb()
            sLocDb.import_from(str(locdb_file))
    return sLocDb

# ----------------------------------------------------------------------

sLocDb = None

# ----------------------------------------------------------------------
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
