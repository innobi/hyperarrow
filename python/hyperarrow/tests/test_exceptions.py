import pyarrow as pa
import pytest

import hyperarrow as hal


def test_unsupported_type(tmp_hyper):
    tbl = pa.Table.from_arrays([[None]], names=["a"])
    with pytest.raises(RuntimeError, match="Type not supported"):
        hal.write_to_hyper(tbl, str(tmp_hyper), "schema", "table")
