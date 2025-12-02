# Advent of Code 2022 solutions in C.

I want to facilitate the resolution of the problems using custom libraries and unit tests.
Since recurent problems involve data structures, algorithms and 2d puzzles, I created libraries in /lib with tree, double linked list, stack, ring buffers, parsers... and still continuing.


To run : 

    $ cmake ./CMakeLists.txt

then

    $ make all

then

    $ make test
    
ascii 2d graphics is in use in days 005 009 and 099
each puzzle can be invoked by calling the executables:
ex: 2022 day 001

    $ cd 2022/001

    $ build/bin/2022day001 2022/001/data/test1.txt --draw-delay 50

to have time to see the puzzle unfold, --draw-delay x must be specified (in ms)

Note: I am reforging this library to support future problems better, so it is not working at the moment.
Gifs are in their respective folders as an apology while I finish the work in progress.
