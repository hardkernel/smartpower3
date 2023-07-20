#/usr/bin/python3


from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import pandas as pd
import pdb
import pyvisa
import seaborn as sns
import time
from pandas.core.frame import DataFrame


def get_device_index_number(instances):
    available_instruments = []
    print('Available instruments (Select number and press Enter):')

    for num, inst in enumerate(instances):
        print(f'{num}: {inst}')
        available_instruments.append(str(num))
    print(available_instruments)

    device = input('Make your selection: ')
    print(f'Selected instrument: "{device}"')
    if device not in available_instruments:
        print('Invalid instrument number')
        return -1
    else:
        return device

def get_logging_time():
    logging_time = input('Logging time in seconds: ')
    return int(logging_time)

def connect_to_device(device_num):
    rm = pyvisa.ResourceManager('@py')
    instances = rm.list_resources()

    while (device_num == -1):
        device_num = get_device_index_number(instances)

    return rm.open_resource(instances[int(device_num)])

def setup_instrument(inst, output_channel, device_input_voltage):
    inst.baud_rate = 115200
    inst.read_termination = '\r\n'
    inst.write_termination = '\r\n'
    print(inst.query('*idn?', 0.2))
    time.sleep(0.5)
    inst.query('*rst')
    inst.write(f'source{output_channel}:volt {device_input_voltage}')
    inst.write(f'source{output_channel}:curr 3') # no limit on current

def ask_how_long_to_log():
    logging_time = '0'
    while(int(logging_time) <= 0 or not logging_time.isnumeric()):
        print('How long would you like to log? Please enter time in whole seconds.')
        print('Your choice:')
        logging_time = input()

    return int(logging_time)

def capture(inst, output_channel, logging_time):
    line_ending='\r\n'
    end_time = datetime.now() + timedelta(seconds=logging_time)
    inst.write(f'system:communicate:serial:feed LOG')
    inst.write(f'output{output_channel} ON')
    time.sleep(1)

    with open('sp3_dataset.csv', 'w') as f:
        while(datetime.now() < end_time):
            line = inst.read()
            f.write(f'{line}{line_ending}')
            print(line)

def finish_capture(inst, output_channel):
    # uncomment the following line to cut power to the measured device when the logging is done
    #inst.write(f'output{output_channel} OFF')
    inst.write(f'system:communicate:serial:feed NONE')

def plot_graph(output_channel):
    data = pd.read_csv('./sp3_dataset.csv', names=[
        'ms',
        'input_volt', 'input_ampere', 'input_watt', 'input_on_off',
        'ch0_volt', 'ch0_ampere', 'ch0_watt', 'ch0_on_off', 'ch0_interrupts',
        'ch1_volt', 'ch1_ampere', 'ch1_watt', 'ch1_on_off', 'ch1_interrupts',
        'CheckSum8_2s_Complement', 'CheckSum8_Xor'])

    ndata = data.drop(columns=[
        'input_volt', 'input_ampere', 'input_watt', 'input_on_off',
        'ch0_volt', 'ch0_ampere', 'ch0_on_off', 'ch0_interrupts',
        'ch1_volt', 'ch1_ampere', 'ch1_on_off', 'ch1_interrupts',
        'CheckSum8_2s_Complement', 'CheckSum8_Xor'])

    ndata.insert(3, 'seconds', 1.0)
    start_ms = ndata.ms[0]
    ndata.seconds = (ndata.ms-start_ms)/1000
    print(ndata)

    graph = sns.relplot(x='seconds', y=f'ch{output_channel-1}_watt', data=ndata, kind='line')
    graph.set_axis_labels("Seconds", "milliWatts")
    plt.show()

def run():
    # Please change the value on the following line to suit your device
    device_input_voltage = 5.1  # input voltage (V)
    # Please change the value on the following line to the channel your device is connected to
    output_channel = 1  # output channel
    device_num = -1
    logging_time = 0  # in seconds

    inst = connect_to_device(device_num)
    setup_instrument(inst, output_channel, device_input_voltage)
    time.sleep(2)
    logging_time = ask_how_long_to_log()
    try:
        capture(inst, output_channel, logging_time)
    except Exception as excp:
        print('Sorry, no data could be captured this time')
        print(excp)
    finish_capture(inst, output_channel)
    plot_graph(output_channel)

if __name__ == '__main__':
    run()
