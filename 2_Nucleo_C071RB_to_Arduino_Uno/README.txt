This is the source code for an experiment that involves:
-- NUCLEO-C071RB, as USB host
-- Arduino Uno, as CDC-ACM USB device

The experiment is very simple and it is meant to serve as a 
template that one can use to develop more complex projects
that need to use such USB communication.

To replicate, you will need to:
1-- Compile (using STM32CubeIDE) and program the Nucleo board with the project:
nano/proj_usb_host_montanari/
2-- Compile and program an Arduino Uno with the sketch: 
arduino_as_cdc_acm_device/

For additional details, please see the description/comments 
inside the source code files. 

NOTE: 
The "nucleo" folder has the entire STM32CubeIDE project folder zipped 
because it contains lots of individual files.