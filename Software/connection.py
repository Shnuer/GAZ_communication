import serial
import numpy as np
import time


class CommunicationOnSerial(object):
    
    def __init__(self,num_port,transmission_speed = 115200):
        
        self.ser = serial.Serial(num_port, transmission_speed, timeout = 20)
        self.work_resolution = False
        
        self.start_byte_speed = '#'
        self.start_byte_angle = '$'
        self.start_byte_debug_and_stop = '&'
   
    #Разрешение использования других команд
    def Activate_connection(self):
        self.work_resolution = True

    
    #Команда остановки
    def Deactivate_connection(self):
        

        pkg = bytes([ord(self.start_byte_debug_and_stop),
                        np.uint8(67), 
                        np.uint8(89), 
                        np.uint8(23) 
                    ])

        self.Push_msg(pkg)

        self.work_resolution = False

    #Команда пауза
    def Stop_connection (self):

        pkg = bytes([ord(self.start_byte_debug_and_stop),
                        np.uint8(34), 
                        np.uint8(63), 
                        np.uint8(129) 
                    ])

        self.Push_msg(pkg)

        self.work_resolution = False
        

    
    def Change_speed(self, speed_gaz):
        
        if self.work_resolution == True:

            push_speed = np.uint8(speed_gaz)
            control_value_speed = speed_gaz * 2

            pkg = bytes([ord(self.start_byte_speed),
                            np.uint8(speed_gaz),
                            np.uint8(control_value_speed)
                        ])

            self.Push_msg(pkg)


    def Change_angle_rotation(self, angle_of_rotation_gaz):
        if self.work_resolution == True:

            push_angle = np.uint8(angle_of_rotation_gaz)
            control_value_angle = angle_of_rotation_gaz * 3
            
            pkg = bytes([ord(self.start_byte_angle),
                            np.uint8(angle_of_rotation_gaz),
                            np.uint8(control_value_angle)
                        ])       

            self.Push_msg(pkg)     


    def Enable_debugging(self):

        if self.work_resolution == True:

            pkg = bytes([ord(self.start_byte_debug_and_stop),
                            np.uint8(38), 
                            np.uint8(79), 
                            np.uint8(123) 
                        ])
            
            self.Push_msg(pkg)

    def Disable_debugging(self):

        if self.work_resolution == True:

            pkg = bytes([ord(self.start_byte_debug_and_stop),
                            np.uint8(31), 
                            np.uint8(39), 
                            np.uint8(115) 
                        ]) 
            
            self.Push_msg(pkg)

    
    ### to send a message to the microcontroller ###
    def Push_msg(self, pkg):
        print(pkg)
        self.ser.write(pkg) 

    def getDebugLine(self):
        return self.ser.readline(100)       
# Для теста предлагается в скрипте выделить часть "main" и в ней 
# провести инициализацию и читать отладочные строки с периодической передачей значений скорости и поворота.


if __name__ == "__main__":

    num_port = '/dev/ttyACM1'
    Connection = CommunicationOnSerial(num_port)



    Connection.Activate_connection()
    Connection.Enable_debugging()

    while(1):
        time.sleep(0.1)
        Connection.Change_speed(25)
        Connection.Change_angle_rotation(33)
        Connection.Deactivate_connection()
        Connection.Activate_connection()
        
        print('I get')
        num = Connection.getDebugLine()
        print(num)
        print()
        


    

