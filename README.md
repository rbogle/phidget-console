
## Phidget Console

This console application can be used to drive the relays on a Phidget InterfaceKit board (tested on 0/0/4, but should work on others)

You must have the Phidget Drivers installed on your system for this to work. See: http://www.phidgets.com/docs/OS_-_Windows#Quick_Downloads

It is driven by command line options.

### Allowed options:
```
-? [ --help ]         produce help message and exit
-i [ --info ]         get diagnostic info from phidget and exits
-p [ --port ] arg     set port number, if no port is set then all ports are used.
-s [ --state ] arg    state to set port: 0 off, 1 on. if state is not set then 0 is used
-t [ --toggle ]       toggle specific port or all ports
-v [ --verbose ]      output state of ports as they change
```

### Notes:
----------------------

Many options can (or should) be combined.

Default settings result in the following behaviors:

* if no port or state options are given, all ports are shut off.
* if only state is given (0 or 1) then all ports are set to that state.
* Toggling can work with a specified port, with no port specified all ports are used, supplying state has no effect.
