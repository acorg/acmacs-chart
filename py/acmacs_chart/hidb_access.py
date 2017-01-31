# -*- Python -*-
# license
# license.
# ----------------------------------------------------------------------

from pathlib import Path
from acmacs_chart_backend import HiDbSet

# ----------------------------------------------------------------------

def get_hidb(virus_type, hidb_path :Path = Path("~/AD/data")):
    global sHidbSet
    if sHidbSet is None:
        sHidbSet = HiDbSet(str(Path(hidb_path).expanduser().resolve()))
    return sHidbSet.get(virus_type)

# ----------------------------------------------------------------------

sHidbSet = None

# ----------------------------------------------------------------------
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
