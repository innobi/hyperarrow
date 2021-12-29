import os
import sys


def test():
    try:
        import pytest
    except ImportError as e:
        raise ImportError("pytest required to run hyperarrow tests") from e

    module_dir = os.path.dirname(__file__)
    sys.exit(pytest.main([module_dir]))
