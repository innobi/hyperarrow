import datetime

import pyarrow as pa

import hyperarrow.libhyperarrow as hal


def test_roundtrip():
    schema = pa.schema([
        ("int16", pa.int16()),
        ("int32", pa.int32()),
        ("int64", pa.int64()),
        ("float64", pa.float64()),
        ("boolean", pa.bool_()),
        ("string", pa.utf8()),
        ("date", pa.date32()),
        ("datetime", pa.timestamp("us")),
    ])

    tbl = pa.Table.from_pydict({
        "int16": range(3),
        "int32": range(3),
        "int64": range(3),
        "float64": [1., 2., 3.],
        "boolean": [True, False, True],
        "string": list("abc"),
        "date": [
            datetime.date(1900, 8, 15),
            datetime.date(1970, 1, 2),
            datetime.date(2021, 12, 23),
        ],
        "datetime": [
            datetime.datetime(1900, 8, 15, 12, 36, 0),
            datetime.datetime(1970, 1, 2, 22, 42, 17, 0),
            datetime.datetime(2021, 12, 23, 14, 32, 59, 0),
        ]
    }, schema=schema)

    hal.write_to_hyper(tbl)
    read = hal.read_from_hyper("example.hyper", "schema", "table")
    assert tbl.equals(read)