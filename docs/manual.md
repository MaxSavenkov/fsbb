# Finite-state Building Blocks Manual

[Concepts](#Concepts)

[Base FSM class](#FSM)

[Registry](#Registry)

[Containers](#Containers)

[Manipulators](#Manipulators)

[Enter/Exit Policies](#Policies)

[Examples](#Examples)

## Concepts

A finite-state machine built using FSBB is defined using the following concepts:

* State ID
* State Type
* State Registry
* State Container
* State Manipulator
* Enter/Exit Policy
* Context

### State ID

State ID is any C++ type that uniquely identifies a state. In most cases, this is an int, enum or a string, but it can be any type, as long as it follows this contract:

* It should has a default constructor. An object constructed in this way should denote an invalid state
* It should be copyable (i.e. has a copy constructor and operator= defined)
* It should has an equality operator (==) defined

### State Type

This is a type that represents a state of a state machine. It can be something simple, like an int, but in most cases, it will probably be a pointer to a state class that encapsulates behaviour for this state.

State Type does not have a contract it should follow by default, but Enter/Exit Policy can impose such a contract on State Type (see [Enter/Exit Policies](#policies)).

If State Type is a class or a struct, but not a pointer, it should be copyable.

### State Registry

It's a class that allows registring states for further use by the state machine. Most FSMs using FSBB will inherit from the one provided by the library.

### State Container

State Container stores the current state of state machine. It could be a single state, or a set of states.

State containers provided by FSBB are split into two parts: interface and implementation. Interfaces are used to build the final FSM's list of methods, while implementation stores the state. This is done because sometimes it is useful to have different interfaces to the same state container.

### State Manipulator

State Manipulator allows FSM's end-user to manipulate the state of the machine. For example, it could provide a set of methods to change the sate of the machine immediately, or queue the changes and process them later.

As with containers, manipulators in FSBB are too split into interface and implementation, for the same reasons. Implementation is used to store manipulator-specific additional data, like the pointer to the next state or queue of changes, that do not belong in State Container.

### Enter/Exit Policy

It is often useful to let the state know that the state machine entered or exited it. For this, a FSM can be parametrized with a Enter/Exit policy, which defines two functions: on_enter and on_exit, which are called when something happens to the state.

FSBB provides several useful policies, including the default policy that do nothing.

Enter/Exit policy is a parameter of the State Manipulator.

### Context

To make the Enter/Exit Policies even more useful, it is often necessary to provide on_enter/on_exit functions with some kind of context. The context could be anything, from a POD type to an interface for interacting with the rest of your application.

Context is a parameter of the State Manipulator.

For examples of context, see [Examples](#examples) section of this manual.

-----------------------------------------------------

## FSM

**fsbb:fsm** is a base class for finite-state machines constructed using FSBB. You can construct your own machines without using it, or you can inherit from it to extend its functionality if you need to.

```c++
template
<
    typename t_state_id,
    typename t_state,
    typename t_state_container_interface,
    typename t_state_manipulator_interface
>
class fsm : 
    public state_registry<t_state_id, t_state>,
    public t_state_container_interface,
    public t_state_manipulator_interface
{
...
};
```

This class do not provide any methods by itself, but instead serves as framework into which various building blocks can be inserted.

**Parameters:**
* **t_state_id** - type of [State ID](#State ID) used by this machine
* **t_state** - [State Type](#State Type) used by this machine
* **t_state_container_interface** - [State Container](#State Container) uses by this machine (see [Containers](#Containers))
* **t_state_manipulator_interface** - [State Manipulator](#State Manipulator) uses by this machine (see [Manipulators](#Manipulators))

## Registry

[State Registry](State Registry) stores possible states for a state machine. It uses class **fsbb::state_and_id**:

```c++
template
<
    typename t_state_id,
    typename t_state
>
struct state_and_id
{
    t_state_id id;
    t_state state;
};
```

**Fields:**
* **t_state_id id** - ID of a state
* **t_state state** - State itself

The state registry itself is defined in this way:

```c++
template
<
    typename t_state_id,
    typename t_state
>
class state_registry
{
    void register_state( t_state_id id, t_state state );
    state_and_id<t_state_id, t_state>* find_state( t_state_id id )
};
```

**Methods:**

**```bool register_state( t_state_id id, t_state state )```**

Registers state "state" under id "id". If state with this id is already registred, returns false.

**```state_and_id<t_state_id, t_state>* find_state( t_state_id id )```**

Finds previously registred state. If no state is found, returns 0.

## Containers

FSBB provides two types of containers for building state machines: single-state and stacked-state container. In reality, stacked-state does not really uses a stack, but rather just an array of states with random access for insertation/removal of members.

### Single-state container:

```c++
#include "fsbb_single.hpp"

template
<
    typename t_state_id,
    typename t_state
>
class state_container_single_interface
{
    state_container_single_interface( t_impl& impl );

    t_state get_current_state_id() const;
    const t_state get_current_state() const;
};

```

**Constructors:**

**```state_container_single_interface( t_impl& impl )```**

Initializes container interface with a container implementation. The base [FSM](#FSM) class takes care of this automatically.

**Methods:**

**```t_state get_current_state_id() const```**

Returns the current state's ID.

**```const t_state get_current_state() const```**

Returns the current state.

TODO: If there is no current state, presently it returns 0. This will not work for any state type.


### Stacked-state container:

```c++
#include "fsbb_stacked.hpp"

class state_container_stacked_interface
{
public:
    typedef typename t_impl::current_states_vector current_states_vector;

    state_container_stacked_interface( t_impl& impl );

    t_state get_top_state_id() const;
    t_state get_top_state() const;

    template<typename t_functor>
    void for_all_states_from_bottom( t_functor& f );

    template<typename t_functor>
    void for_all_states_from_top( t_functor& f );

    const current_states_vector& get_current_states() const;
};

```

**Constructors:**

**```state_container_stacked_interface( t_impl& impl )```**

Initializes container interface with a container implementation. The base [FSM](#FSM) class takes care of this automatically.

**Methods:**

**```t_state get_top_state_id() const```**

Returns the current top state's ID.

**```const t_state get_top_state() const```**

Returns the current top state.

TODO: If there is no current top state, presently it returns 0. This will not work for any state type.

**```
template<typename t_functor>
void for_all_states_from_bottom( t_functor& f )```**

Applies functor f to all states, starting from the bottom of the stack.

**```
template<typename t_functor>
void for_all_states_from_top( t_functor& f )```**

Applies functor f to all states, starting from the top of the stack.

**```const current_states_vector& get_current_states() const```*

Returns the current set of states.

## Manipulators

FSBB provides three type of state manipulators for both single- and stacked-state containers. This types are immediate, queued and combined.

Immediate manipulators change the state of the machine at once. This is the most basic behaviour, but it may be dangerous in some cases.

Queued manipulators place requested operations into a queue and execute them at once (in order of addition to queue) when the update function is called.

Combined manipulators provide both interfaces to allow for maximum flexibility.

All state manipulators can be parametrized with an [Enter/Exit Policy](#enter-exit-policies) and a [Context](#context).

### Single-state manipulators

#### Immediate single-state manipulator

```c++
template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_single_immediate_interface
{
public:
    state_manipulator_single_immediate_interface( t_impl& impl );

    bool change_state_immediate( t_state_id id, context_holder<t_context> ctx = context_holder<t_context>() );
};
```

**Constructors:**

**```state_manipulator_single_immediate_interface( t_impl& impl )```**

Initializes manipulator interface with a manipulator implementation. The base [FSM](#FSM) class takes care of this automatically.

**Methods:**

**```bool change_state_immediate( t_state_id id, context_holder<t_context> ctx )```**

Immediately changes the current state of FSM to a specified one. If the specified state is not found, the previous state is left intact, and the method returns false.

#### Queued single-state manipulator

```c++
template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_single_queued_interface : public state_manipulator_single_queued_interface_base<t_state_id, t_state, t_on_enter_exit_policy, t_context>
{
public:
    state_manipulator_single_queued_interface( t_impl& impl ) ;

    // Inherited from state_manipulator_single_queued_interface_base:
  
    bool queue_change_state( t_state_id id );

    void update( context_holder<t_context> ctx = context_holder<t_context>() );
};
```

**Constructors:**

**```state_manipulator_single_queued_interface( t_impl& impl )```**

Initializes manipulator interface with a manipulator implementation. The base [FSM](#FSM) class takes care of this automatically.

**Methods:**

**```bool queue_change_state( t_state_id id );```**

Queues a change of state. If the specified state is not found the method returns false. Only a single change of state can be queued at the same time, meaning that if you will call this function several times before calling update(), only the last queued change will actually be executed.

TODO: Is this proper? Or maybe we should provide two different implementations, one with a single queued state change, and another with a real queue?

**```void update( context_holder<t_context> ctx );```**

Processes the queued change of state.

#### Combined single-state manipulator

```c++
template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_single_combined_interface :
    public state_manipulator_single_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>,
    public state_manipulator_single_queued_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>
{
public:
    state_manipulator_single_combined_interface( t_impl& impl ) ;
};
```

**Constructors:**

**```state_manipulator_single_combined_interface( t_impl& impl )```**

Initializes manipulator interface with a manipulator implementation. The base [FSM](#FSM) class takes care of this automatically.

**Methods:**

Combined manipulator provides methods from both [immediate](#immediate-single-state-manipulator) and [queued](#Queued-single-state-manipulator) state manipulators.