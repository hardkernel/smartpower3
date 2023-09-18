from platformio.public import TestCase, TestRunnerBase, TestStatus

try:
    import os
except ImportError as exc:
    print(exc)


class CustomTestRunner(TestRunnerBase):

    pytest_installed = False

    def setup(self):
        if os.system("pip3 --require-virtualenv install pytest pyvisa-py") == 0:  # bash script returning 0 meas success
            self.pytest_installed = True

    def teardown(self):
        if self.pytest_installed:
            os.system("pip3 uninstall -y pytest pyvisa pyvisa-py")

    def map_pytest_test_result_to_platformio_result(self, pytest, result):
        if result == pytest.ExitCode.OK:
            return TestStatus.PASSED
        elif result == pytest.ExitCode.TESTS_FAILED:
            return TestStatus.FAILED
        elif result == pytest.ExitCode.INTERRUPTED:
            return TestStatus.ERRORED
        elif result == pytest.ExitCode.INTERNAL_ERROR:
            return TestStatus.ERRORED
        elif result == pytest.ExitCode.USAGE_ERROR:
            return TestStatus.ERRORED
        elif result == pytest.ExitCode.NO_TESTS_COLLECTED:
            return TestStatus.SKIPPED
        else:
            return TestStatus.WARNED

    def stage_building(self):
        pass

    def stage_testing(self):
        import pytest

        test_result = pytest.main(["test/python_scpi/test_scpi/scpi_tests.py"])
        self.test_suite.add_case(
            TestCase(
                name="pyvisa_scpi_tests", status=self.map_pytest_test_result_to_platformio_result(pytest, test_result)))
