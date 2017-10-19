# Finite State Building Blocks
Finite State Building Blocks is a C++ library for quickly building finite-state machines, geared toward game development.

## States and Finite-state Machines

The [State pattern](http://gameprogrammingpatterns.com/state.html) is useful for defining state-dependent behaviour of a program.
It can be use in many places, from general state of application (in game, that could Main Menu, Level, Pause etc.) to animation state of an actor.

However, while the general pattern holds, details of implementation of a state machine vary from case to case.
This make it impossible to provide a single universal state machine, or even a limited set of machines that cover all common use cases.
The aim of this library is to provide simple building blocks to assemble varied finite-state machines.

## Overview of other libraries

There already exist several general-purpose libraries for defining finite-state machines. Howeve, I found that none of them fit my needs.

* [Boost.Statechart](http://www.boost.org/doc/libs/1_61_0/libs/statechart/doc/index.html) is geared toward translating UML charts into C++ code and back. The users need to define their state machines statically, which is often too limiting, as in game development you often need to define states and transitions in resources, for example, when defining animation states. It's also very verbose and hard to use quickly.
* [Boost.Meta State Machine](http://www.boost.org/doc/libs/1_61_0/libs/msm/doc/HTML/index.html) is pretty much the same ideologically.
* [TinyFSM](http://digint.ch/tinyfsm/index.html) is much simplier (and limited) library, however, it too favors statically-defined machines.
* [Libero](http://www.cs.vu.nl/~eliens/mt/@archive/online/archive/documents/libero/lrintr.htm) is an old-school FSM generator which operates on a textual description of a FSM to create C code.
* [GFSM](http://kaskade.dwds.de/~moocow/mirror/projects/gfsm/) is a monstroity of a FSM C library, of which functions I'm not sure, because documentation is mostly "here, take a look at our header files".

As you can see, none of these libraries provide dynamically configurable finite-state machines (despite Boost.Statechart's insistence that "However, fully-featured dynamic FSM libraries do exist").

## Examples

1) Pre-fabricated FSM.

For added simplicty, FSBB also contains a "fsbb_prefab.hpp" header, which has some pre-defined state-machines.

```c++

  // A single-state FSM with immediate interface
  // States are identified by int
  // State itself is a string
fsm_single_immediate<int, std::string> fsm;

fsm.register_state( STATE_ID_1, "state1" );
fsm.register_state( STATE_ID_2, "state1" );

fsm.change_state_immediate( STATE_ID_1 );
const std::string& current_state = fsm.get_current_state();
```

However, you might want to mix-and-match building blocks for your unique use cases without much troubles, too:
```c++
  // A pushdown automata with combined immediate/queued interface
  // States are identified using a custome class 'state_id'
  // State itself is a shared pointer to 'base_state' class, presumbly, with virtual functions that provide state's functionality
  // When a state is placed or removed from stack, on_enter and on_exit functions are called on state object, per enter_exit_policy_notify policy
  // on_enter/on_exit functions are provided with a non-const reference to 'my_context' class for additional interface to outside world
class my_own_customzed_fsm
    : public fsm
    <
        state_id,
        std::shared_ptr<base_state>,
        state_container_stacked_interface<state_id, std::shared_ptr<base_state> >,
        state_manipulator_stacked_combined_interface<state_id, std::shared_ptr<base_state>, enter_exit_policy_notify, my_context&>
    >
{
};

my_own_customzed_fsm fsm;

my_context ctx;

fsm.register_state( state_id_provider.get_id( STATE1 ), std::make_shared<state1>() ); // state1 is a public descendant of base_state
fsm.register_state( state_id_provider.get_id( STATE2 ), std::make_shared<state2>() ); // state2 is a public descendant of base_state

fsm.push_state( state_id_provider.get_id( STATE1 ), ctx ); // Enter the first state immediately

fsm.queue_push_state( state_id_provider.get_id( STATE2 ) ); // Queue entering to state 2
fsm.queue_remove_state( state_id_provider.get_id( STATE1 ) ); // Queue removing of state 1

fsm.update( ctx ); // Process queued actions
```

Constructing your own FSMs might seem a little daunting from this example, but it's really not that hard, and most of the time, you will only need to use pre-fabricated types or define just a few of your own custom types.

## Documentation

Further documentation could be found at: [docs/manual.md](docs/manual.md)
