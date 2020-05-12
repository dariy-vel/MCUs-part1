from functools import reduce
import threading
import tkinter as tk
from tkinter import TOP, LEFT, RIGHT, messagebox
import serial
import serial.tools.list_ports as lp

BAUD_RATE = 9600
SERIAL_PORTS_PATH = './DefaultSerialPorts'
WINDOW_MIN_HEIGHT = 400

app_is_running = False
ports = list()
selected_port = ''
serial_port = serial.Serial()

SLAVE1 = 'Slave 1'
SLAVE2 = 'Slave 2'
data_sections = dict.fromkeys((SLAVE1, SLAVE2))
selected_slave = SLAVE1

CRC_BIT_LENGTH = 8
POLY = 0xFF
INIT_CRC = 0x2F
XOR_OUT = 0xFF
REF_IN = False
REF_OUT = False

received = ''


def main():
    global app_is_running, ports
    app_is_running = True

    for port in sorted(lp.comports()):
        ports.append(port.device)
    if not ports:
        with open(SERIAL_PORTS_PATH, 'r') as file:
            ports = list(filter(None, file.read().split('\n')))

    serial_communication = threading.Thread(target=read_from_port)
    serial_communication.daemon = True
    serial_communication.start()

    window = tk.Tk()
    _ = AppGUI(window)
    window.mainloop()
    serial_port.close()
    app_is_running = False
    serial_communication.join()


class AppGUI:

    def __init__(self, window):
        global ports, data_sections

        window.title('Serial Interface')
        window.minsize(height=WINDOW_MIN_HEIGHT, width=round(WINDOW_MIN_HEIGHT * 1.5))

        port_frame = tk.Frame(window)
        data_labels_frame = tk.Frame(window)
        data_frame = tk.Frame(window)
        button_frame = tk.Frame(window)

        port_label = tk.Label(
            port_frame,
            text='Select port',
            font=('Verdana', 12),
        )
        port_name_variable = tk.StringVar(window)
        port_name_variable.set(' ' * 10)

        def select_port(port):
            global selected_port
            selected_port = port

        port_selection = tk.OptionMenu(port_frame, port_name_variable, *ports, command=select_port)
        port_open_button = tk.Button(
            port_frame,
            text='Open',
            font=('Verdana', 10),
            bg='#E2E2E4',
            activebackground='lightgrey',
            command=open_port,
        )

        slave1_label = tk.Label(data_labels_frame, text='Slave 1', font=('Verdana', 14))
        slave2_label = tk.Label(data_labels_frame, text='Slave 2', font=('Verdana', 14))

        data_section_slave1 = tk.Text(data_frame, state='disabled', height=10, width=25, bg='black', fg='white')
        data_section_slave2 = tk.Text(data_frame, state='disabled', height=10, width=25, bg='black', fg='white')
        data_sections[SLAVE1] = data_section_slave1
        data_sections[SLAVE2] = data_section_slave2
        data_sections[SLAVE1].tag_configure('warning', foreground='red')
        data_sections[SLAVE2].tag_configure('warning', foreground='red')

        from_slave1_button = tk.Button(
            button_frame,
            text='From Slave 1',
            font=('Verdana', 10, 'bold'),
            bg='#E2E2E4',
            fg='black',
            command=request_data_from_slave1,
        )
        from_slave2_button = tk.Button(
            button_frame,
            text='From Slave 2',
            font=('Verdana', 10, 'bold'),
            bg='#E2E2E4',
            fg='black',
            command=request_data_from_slave2,
        )

        port_frame.pack(side=TOP)
        data_labels_frame.pack()
        data_frame.pack()
        button_frame.pack()

        port_frame_padding = {'x': 30, 'y': 5}
        data_labels_frame_padding = {'x': 100, 'y': 5}
        data_frame_padding = {'x': 15, 'y': 15}
        button_frame_padding = {'x': 60, 'y': 15}

        port_label.pack(side=LEFT, fill=tk.Y,
                        padx=port_frame_padding['x'], pady=port_frame_padding['y'])
        port_selection.pack(side=LEFT, fill=tk.Y,
                            padx=port_frame_padding['x'], pady=port_frame_padding['y'])
        port_open_button.pack(side=LEFT, fill=tk.Y,
                              padx=port_frame_padding['x'], pady=port_frame_padding['y'])

        slave1_label.pack(side=LEFT, fill=tk.Y,
                          padx=data_labels_frame_padding['x'], pady=data_labels_frame_padding['y'])
        slave2_label.pack(side=RIGHT, fill=tk.Y,
                          padx=data_labels_frame_padding['x'], pady=data_labels_frame_padding['y'])

        data_section_slave1.pack(side=LEFT,
                                 padx=data_frame_padding['x'], pady=data_frame_padding['y'])
        data_section_slave2.pack(side=RIGHT,
                                 padx=data_frame_padding['x'], pady=data_frame_padding['y'])

        from_slave1_button.pack(side=LEFT,
                                padx=button_frame_padding['x'], pady=button_frame_padding['y'])
        from_slave2_button.pack(side=RIGHT,
                                padx=button_frame_padding['x'], pady=button_frame_padding['y'])

        show_data(SLAVE1, '> Data Channel 1\n...')
        show_data(SLAVE2, '> Data Channel 2\n...')


def request_data_from_slave1():
    global serial_port, selected_slave
    if serial_port.isOpen():
        selected_slave = SLAVE1
        serial_port.write(b'1')


def request_data_from_slave2():
    global serial_port, selected_slave
    if serial_port.isOpen():
        selected_slave = SLAVE2
        serial_port.write(b'2')


def show_data(slave, data, is_warning=False):
    """
    :param slave: SLAVE1 or SLAVE2 constant to specify screen for receiving data
    :param data: actual text to print on the screen
    :param is_warning: if True, make piece of text red
    :return:
    """
    data_sections[slave].configure(state='normal')
    if is_warning:
        data_sections[slave].insert('end', data, 'warning')
    else:
        data_sections[slave].insert('end', data)
    data_sections[slave].configure(state='disabled')
    data_sections[slave].see('end')


def open_port():
    global app_is_running, serial_port

    try:
        serial_port = serial.Serial(selected_port, BAUD_RATE)
    except serial.serialutil.SerialException as e:
        serial_port.close()
        if app_is_running:
            messagebox.showerror('Error', e.strerror)
    else:
        messagebox.showinfo(selected_port, 'Port is opened successfully')


def read_from_port():
    global app_is_running, serial_port, selected_slave, received

    while app_is_running:
        if serial_port.isOpen():
            data = str(serial_port.read())[2:-1]
            if data:
                if received != '':
                    received += data
                elif data == "#" and received == '':
                    received += data

                if received.endswith("*"):
                    received = received[1:-1]
                    received = received.split(';')
                    # show_data(selected_slave, received[0] + '\n')
                    crc = int("0x" + received[0], 16)
                    result = check_crc(received[1], crc, CRC_BIT_LENGTH, POLY, INIT_CRC, REF_IN, REF_OUT, XOR_OUT)
                    show_data(selected_slave, received[1] + '\n' + "Crc check: " + str(result) + '\n')
                    received = ''


def check_crc(data, crc, crc_bit_length, poly, init_crc, in_ref, res_ref, final_xor):
    if isinstance(data, str):
        data = convert_ascii_str_to_int_list(data)

    poly_msb = 1 << (crc_bit_length - 1)
    cutter = (1 << crc_bit_length) - 1

    new_crc = init_crc

    for i in range(len(data)):
        temp = data[i]
        if in_ref:
            temp = reverse_bits(temp, 8)
        temp <<= crc_bit_length - 8
        new_crc ^= temp

        for _ in range(8):
            if (new_crc & poly_msb) != 0:
                new_crc = (new_crc << 1) ^ poly
            else:
                new_crc <<= 1
        new_crc &= cutter

    if res_ref:
        new_crc = reverse_bits(new_crc, crc_bit_length)
    new_crc ^= final_xor
    return new_crc == crc


def reverse_bits(number, bits_quantity=None):
    number = bin(number)[2:]

    if bits_quantity is not None:
        return int(''.join(number[::-1] + '0' * (bits_quantity - len(number))), 2)
    return int(''.join(number[::-1]), 2)


def convert_ascii_str_to_int_list(data):
    return [convert_ascii_str_to_int(i) for i in data]


def convert_ascii_str_to_int(data):
    return reduce(lambda x, y: x | y, [ord(data[::-1][i]) * ((2 ** 8) ** i) for i in range(len(data))])


if __name__ == '__main__':
    main()
