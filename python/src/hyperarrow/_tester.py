import sys


def test():
    try:
        import pytest
    except ImportError as e:
        raise ImportError("pytest required to run hyperarrow tests") from e

    sys.exit(pytest.main())
