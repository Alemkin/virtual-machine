/**************************************
University of Central Florida
COP3402 System Software
**************************************/
(Test case used was one of the provided)

Alexander Lemkin
HW1 (Virtual Machine)
Test Case 2 provided by TA

Just call "gcc -o compiledfile VirtualMachine.c" 
in order to compile my program. 

Then call "./compiledfile" to run it.

The focus of this test case is on call and return operations, 
and the correct storage of values into the stack. Note that 
after the first call the procedure modifies values in it's own 
AR and the previous AR (the values are set to 22), and after 
the second call values inside all activation records are 
modified (set to 33). 

Included in the zip is stacktrace.txt which was the output based on the provided
mcode.txt that my program produced. 
