# fork

mp_server leaves the last few worker processes as zombies. Why doesn't the signal handler fire for them?

# pthread

## Attributes

[Very good tutorial](http://users.cs.cf.ac.uk/Dave.Marshall/C/node30.html)

## Misc.

* the worker thread seems to be scheduled before the master thread. OS runs worker right after spawning.
  - i guess this can be changed by thread attrs.
