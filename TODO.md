# TODO

## Code

- [ ] Implement PWM for the LEDs. All LEDs are on PWM pins.
- [ ] Add example of how to add e.g. a fn key via KeyboardComputerInterface

## Hardware

- [ ] Move a few vias further from solder pads.
- [ ] Increase distance between ground plane and some signal lines (I had a problem with the first run of PCBs).
- [ ] Use bigger solder pads where they are currently rather small.
- [ ] Move headphone jack further out. Right now, I have to physically modify it to make it fit. I didn't quite put it out enough for the circular neck to fit.
- [ ] Use a bigger resistor for the blue LED. It's at least twice as bright. Datasheet was not accurate, evidently.

## General

Fix a few design flaws, like:

- [ ] Apparently a lot of 4-pin male to male headphone cables are messed up
(i.e. not designed properly, and with wires crossed). Consider using 1-wire
protocol and three-pin male to male headphone cables or some other kind of
cable.
- [ ] Don't put something that uses a pull-up resistor on the Teensy's LED pin.
Now the LED lights up dimly whenever the keyboard is on. 

None of these are fatal flaws (and I'll keep using the V1 keyboard), but they
might bug some people a bit, and most people don't want to painstakingly
re-wire a 4-pin 3.5mm cable.
