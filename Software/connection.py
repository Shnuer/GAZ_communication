import serial
import numpy as np
import time


class CommunicationOnSerial(object):
    """Класс для работы с USB.
    
    Перед началом работы необходимо разрешить передачу данных.

    Перед началом чтения отладочной строки необходимопередать разрешение на отладку.
    """

    def __init__(self, num_port, transmission_speed=115200):

        self.ser = serial.Serial(num_port, transmission_speed, timeout=20)
        self.ser_buffer = ""
        self.work_resolution = False

        self.start_byte_ctl = '#'
        self.start_byte_cmd = '&'

    def activate_connection(self):
        """Разрешение передачи данных."""

        self.work_resolution = True

    # Команда остановки
    def deactivate_connection(self):
        """Остановка соединения."""

        pkg = bytes([ord(self.start_byte_cmd), 67, 89, 23])
        self.__push_msg(pkg)

        self.work_resolution = False

    # Команда пауза
    def stop_connection(self):
        """Пауза соединения."""

        pkg = bytes([ord(self.start_byte_cmd), 34, 63, 129])
        self.__push_msg(pkg)
        
        self.work_resolution = False

    def set_control(self, speed_gaz, steer_gaz):
        """Передача значения скорости и угла поворота."""

        if self.work_resolution is not False:
            pkg = bytes([ord(self.start_byte_ctl), 
                         np.uint8(speed_gaz),
                         np.uint8(steer_gaz),
                         np.uint8(speed_gaz + steer_gaz * 2)
                         ])

            self.__push_msg(pkg)

    def enable_debugging(self):
        """Передача разрешения для отладки."""

        if self.work_resolution is not False:

            pkg = bytes([ord(self.start_byte_cmd), 38, 79, 123])
            self.__push_msg(pkg)

    def disable_debugging(self):
        """Передача запрета для отладки."""

        if self.work_resolution is not False:

            pkg = bytes([ord(self.start_byte_cmd), 31, 39, 115])
            self.__push_msg(pkg)


    def __push_msg(self, pkg):
        print('Send package: {} / hex: {}'.format(list(pkg), pkg))
        self.ser.write(pkg)

    def get_debug_line(self):
        """Получения отладочной информации."""
        
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
# 
# Для теста необходимо передать имя устройства в консоле при запуске файла на подобие: "python connection.py /dev/ttyACM1".


if __name__ == "__main__":

    import argparse

    # Производится чтение аргумента из консоли.
    parser = argparse.ArgumentParser(description='Communication script test')
    parser.add_argument('device', action='store', help='Serial port device')

    args = parser.parse_args()

    # Создание объекта и передача полученного аргумента из консоли.
    Connection = CommunicationOnSerial(args.device)

    print('Set connection activated')
    Connection.activate_connection()
    print('Set debug enabled')
    Connection.enable_debugging()

    print('Start main loop')

    check_time = time.time()

    while(1):
        
        if time.time() - check_time > 1:
            check_time = time.time()
            
            spst_pair = (np.random.randint(-100, 100),
                         np.random.randint(-100, 100))
            print('New speed/steer pair: {}'.format(spst_pair))
            
            Connection.set_control(spst_pair[0], spst_pair[1])
            
        inp = Connection.get_debug_line()
        if inp:
            print('I get: {}'.format(inp))
