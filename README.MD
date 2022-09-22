# userbutton-sct

This was a side-project from the frc 2022 season that intended to toggle startup code on userbutton press. 
Sadly this project died in the testing phase, as I had bigger priorities for worlds. I do not intend to finish this project, however someone from frc2468
may. In that event, a project will probably be pushed to the [frc2468 github](https://github.com/frc2468).

While this project is called "userbutton-sct", the current state of the code does not make use of the userbutton, as I did not have time to figure out the fpga calls and setup.
As I was short on time, I instead opted to toggle on usb device insert (ideally a flashdrive that could be carried onto the field during setup.

## motivation

In the [2nd semifinal match 2468 played at waco](https://www.youtube.com/watch?v=ILfNy_uYCMs), the robot did not move. For some reason, startup code was not enabled. During the time between the first semi-final match abd
the second I worked to resolve some issues (I can not remeber in detail what they were), but it did not regard toggling startup code. Regardless, it was not enabled, and I still
have no explanation for it to this day. The main motivation for this project is to provide a saftey net for those situations, as when the robot was on the field I was
allowed to go up to it and powercycle, so in theory I would have been able to plug in a flashdrive or press the userbutton. I do not really know if the startup code setting in the relevant ini file was even the issue. 

## usage
First of all, don't, but if you are not detered by my warning you first must change the following line in `sct.c` to point to the correct ini file
```c
const char* filepath = "test.ini";
```
I would do it myself, but I can not recall off the top of my head, and I do not have a roborio on me to check. There is probably some documentation page that says
where the relevant ini file is, but I can not be bothered for a project that will likely stay abandoned.
