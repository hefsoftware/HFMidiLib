************************************************************************
WARNING: The library is still in development in an early beta stage.
************************************************************************

HFMidiLib is a library for reading and write of Midi event in Qt
through a signal/slot paradigm, supporting a modular driver architecture.
At the moment it includes support for Windows and Linux (ALSA).
Also included is a demo application that serves both as test and as a demo
to get an idea on how to call the library.

A special thanks goes to the creator and contributors of QMidi project. 
Besides using it in some early projects where I needed to connect to MIDI
devices reading its code gave me a basic idea on how to use the winmm&ALSA APIs.
Other sources of reference were RtMidi, the ALSA documentation and random snippets
found around internet.