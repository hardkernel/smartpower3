import time
from datetime import datetime, timedelta

import pytest
import pyvisa

from platformio.project.config import ProjectConfig


def get_device_port():
    return ProjectConfig.get_instance().get('env:python_scpi', 'test_port', default='/dev/ttyUSB0')


def get_device_baud_rate():
    return ProjectConfig.get_instance().get('env:python_scpi', 'test_speed', default=11500)


@pytest.fixture
def get_resource():
    device_string = get_device_port()

    rm = pyvisa.ResourceManager('@py')
    instances = rm.list_resources()
    for instance in instances:
        if device_string in instance:
            return rm.open_resource(instance)
    pytest.fail("Could not create device instance. Please check your connection settings and device connection.")


def setup_instrument(instance):
    instance.baud_rate = get_device_baud_rate()
    instance.read_termination = '\r\n'
    instance.write_termination = '\r\n'
    instance.write('*RST')
    time.sleep(1)
    instance.write('*CLS')
    time.sleep(1)
    instance.read()
    time.sleep(1)


@pytest.fixture
def connect_and_setup_instrument(get_resource):
    if get_resource:
        setup_instrument(get_resource)
    else:
        pytest.exit(reason='Could not connect to instrument')

    return get_resource


@pytest.fixture
def prepare_reset_conditions(connect_and_setup_instrument):
    instance = connect_and_setup_instrument
    instance.write('source:volt 5')
    time.sleep(1)
    instance.write('source1:curr 3')
    time.sleep(1)
    instance.write('source2:volt 4.6')
    time.sleep(1)
    instance.write('source2:curr 1.5')
    time.sleep(1)
    instance.write('output1:state on')
    time.sleep(1)
    instance.write('output2:state on')
    time.sleep(1)
    return instance


class TestSCPICommands:

    def test_dummy(self):
        assert 1 == 1

    def get_mac(self, instance):
        mac = instance.query('SYSTem:COMMunicate:NETwork:MAC?')
        instance.write('*CLS')
        return mac

    def test_idn_query(self, connect_and_setup_instrument):
        instance = connect_and_setup_instrument
        time.sleep(1)
        mac_address = self.get_mac(instance)
        time.sleep(1)
        expected_idn_string = f'Hardkernel Co Ltd,SmartPower3,{mac_address},Build date'
        time.sleep(3)
        assert expected_idn_string in instance.query('*IDN?')

    def test_system_communicate_network_mac_query(self, connect_and_setup_instrument):
        instance = connect_and_setup_instrument
        expected_mac_string = self.get_mac(instance)
        got_mac_address = instance.query('SYSTem:COMMunicate:NETwork:MAC?')
        assert expected_mac_string == got_mac_address
        mac_parts = got_mac_address.split(':')
        assert len(mac_parts) == 6
        for part in mac_parts:
            assert len(part) == 2

    def test_reset(self, prepare_reset_conditions):
        # check that SP3 is powered by power supply
        assert 1 < float(prepare_reset_conditions.query('FETCh:SCALar:VOLTage:DC? (@1)'))
        assert '**Reset' == prepare_reset_conditions.query('*RST')

    def test_system_error_count_and_next_query(self, connect_and_setup_instrument):
        connect_and_setup_instrument.query('SVSTnothing')
        connect_and_setup_instrument.query('somecommand')
        # now we should have two errors
        assert 2 == int(connect_and_setup_instrument.query('SYSTem:ERRor:COUNt?'))
        time.sleep(1)
        assert '-113,"Undefined header"' == connect_and_setup_instrument.query('SYSTem:ERRor:NEXT?')
        time.sleep(1)
        assert 1 == int(connect_and_setup_instrument.query('SYSTem:ERRor:COUNt?'))
        time.sleep(1)
        assert '**ERROR: 0, "No error"' == connect_and_setup_instrument.query('SYSTem:ERRor:NEXT?')
        time.sleep(1)
        assert '-113,"Undefined header"' == connect_and_setup_instrument.query('SYSTem:ERRor:NEXT?')
        time.sleep(1)
        assert '0,"No error"' == connect_and_setup_instrument.query('SYSTem:ERRor:NEXT?')
        time.sleep(1)

    def test_system_version_query(self, connect_and_setup_instrument):
        assert '1999.0' == connect_and_setup_instrument.query('SYSTem:Version?')

    def test_system_communicate_network_subnet(self, connect_and_setup_instrument):
        connect_and_setup_instrument.write('SYSTem:COMMunicate:NETwork:SUBNet "255.255.255.0"')
        time.sleep(3)
        assert "255.255.255.0" == connect_and_setup_instrument.query('SYSTem:COMMunicate:NETwork:SUBNet?')

    def test_system_communicate_socket_port(self, connect_and_setup_instrument):
        connect_and_setup_instrument.write('SYSTem:COMMunicate:SOCKet:PORT 1')
        time.sleep(2)
        assert '1' == connect_and_setup_instrument.query('SYSTem:COMMunicate:SOCKet:PORT?')
        assert 38 == connect_and_setup_instrument.write('SYSTem:COMMunicate:SOCKet:PORT 10001')
        assert 34 == connect_and_setup_instrument.write('SYSTem:COMMunicate:SOCKet:PORT 0')
