# furutaPendulum
This is the code for developing a furuta pendulum. The plan was to use a pc as a deep rl learning device, and an arduino as a motor controller. I ultimately just did pretty basic control logic to get active stabilization around the stabile equilbrium, but obviously unable to get stabilization around the unstable equilibrium.

The way it works is a pendulums hangs from a rotary encoder, which sends is connected to an arduino. The arduino keeps track of the position and sends it via UDP over an ethernet cable to the PC. The PC then implements whichever algorithm I was working on, and sends the amount of steps the stper motor should take. Then the arduino sends that to the stepper motor driver.

Below are some pictures and videos of the pendulum
https://github.com/user-attachments/assets/0d204ed0-89dc-42aa-81c1-2f68d254fa65
![IMG_2317](https://github.com/user-attachments/assets/db64d5bf-d27b-4819-a076-aac5cdce143c)

For the physical pendulum I machined it out of aluminum, using both a lathe and a mill.
