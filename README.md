Efficient State Machine framework
=================================

Introduction
------------
This repository gathers all the ideas and implementation tricks I had around lightweight, efficient
statecharts implementation in C. Inspired by [QP framework](https://state-machine.com/doc/concepts).
Provided are four examples: simple blink state machine transitioning between two states,
classic dining philosophers problem, cigarette smokers problem and pelican crossing simulation.

Build instructions
------------------

Standard CMake routine with some configuration options.
Project setup for native Linux:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

For STM32 target ([bluepill board](http://wiki.stm32duino.com/index.php?title=Blue_Pill)):

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DESM_PLATFORM=stm32 -DESM_BOARD=bluepill \
      -DCMAKE_TOOLCHAIN_FILE=../platform/stm32/Toolchain.cmake ..
```

Build step:
```sh
make
```

TODO
----

 - add more examples
 - handle timer rollover
 - research and implement efficient priority support
 - check what impact switching from array to list in main scheduler loop will have

License
-------
  - MPLv2

Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
