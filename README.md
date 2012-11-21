injectevents
============

Record input events on Linux and replay (inject) them later.  Timing between
events is maintained.  Useful for emulating user input.

Compile
-------

For Android:

    make CROSS_COMPILE=/path/to/android/toolchain/arm-linux-androideabi-
    make install_android

To run on your host:

    make

record
------

Usage:

    record [<eventfile>]

Records input events and dumps them to <eventfile>, which defaults to
/sdcard/events.

replay
------

Usage:

    replay [<eventfile>]

Replay events from <eventfile> (default /sdcard/events).


Authors
-------

Originally written by Nicholas FitzRoy-Dale <nfd@cse.unsw.edu.au>
Maintained by Aaron Carroll <aaronc@cse.unsw.edu.au>

