package hello
foreign func hello_sayHello_f(): CPointer<Unit>
public func sayHello(): CPointer<Unit> {
    return unsafe {
        hello_sayHello_f()
    }
}
