import numpy as np

class Test(object):
    def __init__(self):
        pass

    def __function_print_msg(self):
        print('msg')

    def dont_private_msg(self):
        self.function_print_msg()


tst = Test()

tst.dont_private_msg()