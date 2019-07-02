import serial
import numpy as np
import time


class CommunicationOnSerial(object):

    def __init__(self, num_port, transmission_speed=115200):

        self.ser = serial.Serial(num_port, transmission_speed, timeout=20)
        self.ser_buffer = ""
        self.work_resolution = False

        self.start_byte_ctl = '#'
        self.start_byte_cmd = '&'

    # Разрешение использования других команд
    def Activate_connection(self):
        self.work_resolution = True

    # Команда остановки
    def Deactivate_connection(self):

        pkg = bytes([ord(self.start_byte_cmd), 67, 89, 23])
        self.Push_msg(pkg)

        self.work_resolution = False

    # Команда пауза
    def Stop_connection(self):

        pkg = bytes([ord(self.start_byte_cmd), 34, 63, 129])
        self.Push_msg(pkg)
        
        self.work_resolution = False

    def set_control(self, speed_gaz, steer_gaz):

        if self.work_resolution == True:
            pkg = bytes([ord(self.start_byte_ctl), 
                         np.uint8(speed_gaz),
                         np.uint8(steer_gaz),
                         np.uint8(speed_gaz + steer_gaz * 2)
                         ])

            self.Push_msg(pkg)

    def Enable_debugging(self):

        if self.work_resolution == True:

            pkg = bytes([ord(self.start_byte_cmd), 38, 79, 123])
            self.Push_msg(pkg)

    def Disable_debugging(self):

        if self.work_resolution == True:

            pkg = bytes([ord(self.start_byte_cmd), 31, 39, 115])
            self.Push_msg(pkg)

    ### to send a message to the microcontroller ###

    def Push_msg(self, pkg):
        print('Send package: {} / hex: {}'.format(list(pkg), pkg))
        self.ser.write(pkg)

    def getDebugLine(self):
        data_2_read = self.ser.inWaiting()
        
        if data_2_read > 0:
            self.ser_buffer += self.ser.read(data_2_read).decode('ascii')
        
        nl_idx = self.ser_buffer.find('\n')
        result = None
        
        if nl_idx >= 0:
            result = self.ser_buffer[:nl_idx]
            self.ser_buffer = self.ser_buffer[nl_idx+1:]
        
        return result

# Для теста предлагается в скрипте выделить часть "main" и в ней
# провести инициализацию и читать отладочные строки с периодической передачей значений скорости и поворота.


if __name__ == "__main__":

    import argparse
    parser = argparse.ArgumentParser(description='Communication script test')
    parser.add_argument('device', action='store', help='Serial port device')

    args = parser.parse_args()

    Connection = CommunicationOnSerial(args.device)

    print('Set connection activated')
    Connection.Activate_connection()
    print('Set debug enabled')
    Connection.Enable_debugging()

    print('Start main loop')

    check_time = time.time()

    while(1):
        
        if time.time() - check_time > 1:
            check_time = time.time()
            
            spst_pair = (np.random.randint(-100, 100),
                         np.random.randint(-100, 100))
            print('New speed/steer pair: {}'.format(spst_pair))
            Connection.set_control(spst_pair[0], spst_pair[1])
            
        inp = Connection.getDebugLine()
        if inp:
            print('I get: {}'.format(inp))
