This is the source code for an experiment that involves:
-- Arduino Uno + USB Shield, as USB host
-- Arduino Uno, as CDC-ACM USB device

The experiment is very simple and it is meant to serve as a 
template that one can use to develop more complex projects
that need to use such USB communication.

To replicate, you will need to:
1-- Compile and program an Arduino Uno + USB Shield with the sketch: 
arduino1_usb_host/
2-- Compile and program another Arduino Uno with the sketch: 
arduino2_cdc_acm_device/
3-- Then, in the Serial terminal connected to Arduino 1, type "go".
This will trigger sending of a command to the CDC device (i.e., Arduino 2)
and prompt it to send its data to Arduino 1.

For additional details, please see the description/comments 
inside the source code files. 