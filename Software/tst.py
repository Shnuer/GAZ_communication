class A(object):
    val = 1
    def foo(self):
        self.val +=3

    def op(self):
        A.val +=5


a = A()
b = A()

a.foo()
b.op()

c = A()

print(a.val)
print(b.val)
print(c.val)
