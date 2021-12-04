import pyarrow as pa
import pytest

import hyperarrow.libhyperarrow as hal


@pytest.mark.skip
def test_unsupported_type():
    tbl = pa.Table.from_arrays([[None]], names=['a'])
    with pytest.raises(RuntimeError, match="Type not supported"):
        hal.write_to_hyper(tbl)
    
