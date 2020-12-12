Efficient State Machine framework
=================================

![status](https://github.com/mryndzionek/esm/workflows/build/badge.svg)

Introduction
------------
Good system design is often about knowing what to avoid.
Unrestricted use of traditional techniques for writing concurrent software, like preemptive threading, almost always
results in systems that are unpredictable and unreliable. Degradation of those two aspects
is specially problematic in embedded systems where predictability and reliability are even
more important than performance and/or expressiveness.

**Active Object Model** brings the same improvements to behavioral design as
**Object Orientation** to architectural design. Active objects are objects that encapsulate own flow of control and
are designed as message pumps with **Run-to-completion** (RTC) semantics and explicit state machine structure.

Adapting **Active Object Model** allows for construction of comprehensible concurrent programs.
Resulting conceptual integrity has also added benefit of making it possible to spend effort on design instead of implementation.
Implementation step in this method is, for the most part, mechanical process and as such, can be automated.

This repository gathers all the ideas and implementation tricks around lightweight, efficient
statecharts and AOM implementation in C. Inspired by [QP framework](https://state-machine.com/doc/concepts).
It's basically a simple cooperative priority-based scheduler and a (hierarchical) state machine framework.
Some implementation techniques and design patterns (like 'Embedded Anchor' and linker-section-based 'plugin' system) are borrowed from Linux kernel.

Another interesting feature is the possibility of running the system without delays, so as fast as possible.
Having the inputs defined explicitly makes mocking them out easy and then linking with the `test` platform
produces a binary that can generate an execution log containing days worth of system operation in seconds.
Then it's easy to analyze the log (e.g. in an Excel spreadsheet) for safety/liveness properties.
Sort of poor man's model checker :smiley:.

Provided are following examples/demos:
 - [simple blink](apps/blink) - hierarchical state machine transitioning between two states and reacting to a button press
 - [Dining Philosophers Problem (DPP)](apps/dpp) - originally formulated in 1965 by Edsger Dijkstra
 - [cigarette smokers problem](apps/smokers) - classical problem originally described in 1971 by Suhas Patil
 - [pelican crossing simulation](apps/pelican) - simplest safety-critical system simulation
 - [producer-consumer simulation](apps/prodcons) - two producers requesting actions on a single resource guarded by a 'bus' module

There are also two more complex apps:
 - [sk6812 LED strip mood lamp and clock](apps/clock) with RTTTL alarms and sunrise simulator
 - [USB HID keyboard firmware](apps/keyboard) - more lightweight QMK alternative

Simplest Example
----------------

In the form of hierarchical state machine (statechart) the logic of a single 'blinker' is:

![blink](apps/blink/umlet/blink.png)

The code implementing this is a set of pointers to structures (black arrows) encoding
the hierarchy and pointer assignments in switch/case handlers (transitions - red arrows):

![blink_dot](apps/blink/umlet/blink_dot.png)

```c
// Blink state machine configuration structure
// It's kept in read-only memory and holds the delay
// the machine will stay in state before transitioning to another

typedef struct {
	const uint32_t delay;
	const uint8_t led_num;
} blink_cfg_t;

// Structure representing the machine object (active object)
// Timer is used to schedule timeout signal on which machine changes state

typedef struct {
	hesm_t esm;
	esm_timer_t timer;
	blink_cfg_t const *const cfg;
} blink_esm_t;

// Define two top-level states (state structures) 'active' and 'paused'

ESM_COMPLEX_STATE(active, top, 1);
ESM_LEAF_STATE(paused, top, 1);

// Define two nested states 'on' and 'off'

ESM_LEAF_STATE(on, active, 2);
ESM_LEAF_STATE(off, active, 2);

static void esm_active_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(on);
}

static void esm_active_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_button:
		ESM_TRANSITION(paused);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_on_entry(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->delay>>3, &sig);

	BOARD_LED_ON(self->cfg->led_num);
}

static void esm_on_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_on_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(off);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_off_entry(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->delay, &sig);
	BOARD_LED_OFF(self->cfg->led_num);
}

static void esm_off_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_off_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(on);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_paused_entry(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	BOARD_LED_OFF(self->cfg->led_num);
}

static void esm_paused_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_paused_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_button:
		ESM_TRANSITION(active);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_blink_init(esm_t *const esm)
{
	ESM_TRANSITION(active);
}

// Configuration structure of machine instance 'blink_1'
static const blink_cfg_t blink1_cfg = {
		.delay = 300UL,
		.led_num = 0
};

// Register instance 'blink_1' of the blink machine with the framework
ESM_COMPLEX_REGISTER(blink, blink1, esm_gr_blinkers, 2, 3, 0);
```

Recommended reading
-------------------

Books:

- [Practical UML Statecharts in C/C++, 2nd Edition:
Event-Driven Programming for Embedded Systems](https://www.state-machine.com/psicc2/)

- [Introduction to Embedded Systems](http://leeseshia.org/releases/LeeSeshia_DigitalV2_2.pdf)

Articles:

- [STATECHARTS: A VISUAL FORMALISM FOR COMPLEX SYSTEMS](http://www.inf.ed.ac.uk/teaching/courses/seoc/2005_2006/resources/statecharts.pdf)

- [Managing Concurrency in Complex Embedded Systems](https://www.state-machine.com/doc/Cummings2006.pdf) - also all the articles from [here](https://www.state-machine.com/doc/an#Active)

- [Problem with threads](https://www2.eecs.berkeley.edu/Pubs/TechRpts/2006/EECS-2006-1.pdf)

Build instructions
------------------

Standard CMake routine with some configuration options.
Currently most extensively tested with:

```sh
cmake version 3.16.3
arm-none-eabi-gcc (15:9-2019-q4-0ubuntu1) 9.2.1 20191025
libnewlib-arm-none-eabi 3.3.0-0ubuntu1
```

Project setup for native Linux:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Project setup for test mode on Linux:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DESM_PLATFORM=test -DESM_BOARD=native ..
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
  - [x] adding hierarchical state machine support
  - [x] add proper platform (BSP) separation
  - [x] change to more flexible build system (CMake)
  - [x] evaluate usefulness of publish-subscribe (efficient implementation limits signal types to 31)
  - [x] switch from array to list in main scheduler loop
  - [x] research and implement efficient priority support
  - [ ] add more examples
  - [ ] handle timer rollover
  - [ ] add automatic tests

License
-------
  - MPLv2

Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
