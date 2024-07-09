## Interface
This folder contains the interface layouts. The App Designer was selected as it allows rapid prototyping and can be used in conjunction with Simulink.

***
## Files
1. Interface. 	The interface designed in Matlab.


***
## Matlab code  to work with Arduino
1. serialportlist("available")'
2. arduinoObj = serialport("/dev/ttyACM0",115200)
3. flush(arduinoObj)
4. configureTerminator(arduinoObj,"CR/LF"); 

