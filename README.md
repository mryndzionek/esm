Efficient State Machine framework
=================================

Introduction
------------
This repository gathers all the ideas and implementation tricks I had around lightweight, efficient
statecharts implementation in C. Inspired by [QP framework](https://state-machine.com/doc/concepts).
Provided are four examples: simple blink state machine transitioning between two states,
classic dining philosophers problem, cigarette smokers problem and pelican crossing simulation.

Recommended reading
-------------------

- [STATECHARTS: A VISUAL FORMALISM FOR COMPLEX SYSTEMS](http://www.inf.ed.ac.uk/teaching/courses/seoc/2005_2006/resources/statecharts.pdf)

- [Practical UML Statecharts in C/C++, 2nd Edition:
Event-Driven Programming for Embedded Systems](https://www.state-machine.com/psicc2/)

- [Introduction to Embedded Systems](http://leeseshia.org/releases/LeeSeshia_DigitalV2_2.pdf)

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

License
-------
  - MPLv2

Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
