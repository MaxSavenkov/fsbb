#pragma once

#include "fsbb_single.hpp"
#include "fsbb_stacked.hpp"

/*
    This file contains some "pre-fabricated" finite-state machines, which implement use-cases I consider common.
    They can be furhter parametrized with state ID and state type for use in your code.

    Each machine is described by a simple table:

    Current state : what is "current state" for this machine, e.g. single current state or a stack of states
    Switching     : which interface does the machine provides for switching a state, e.g. immediate, queued or combined
    Reactions     : what does the machine do when it exits or enters a state, e.g. calls a function of the state or prints something
    Comment       : any additional information

*/

namespace fsbb
{
//----------------------------------------------------------------

/*
    Current state : single
    Switching     : immediate
    Reactions     : does not do anything
*/
template
<
    typename t_state_id,
    typename t_state
>
class fsm_single_immediate
    : public fsm
    <
        t_state_id,
        t_state,
        state_container_single_interface<t_state_id, t_state>,
        state_manipulator_single_immediate_interface<t_state_id, t_state>
    >
{
};

//----------------------------------------------------------------

/*
    Current state : single
    Switching     : immediate
    Reactions     : call on_enter/on_exit functions of the state. The state in this case must be
                    a pointer type which provides these two functions.
*/
template
<
    typename t_state_id,
    typename t_state,
    typename t_context = void
>
class fsm_single_immediate_enter_exit
    : public fsm
    <
        t_state_id,
        t_state,
        state_container_single_interface<t_state_id, t_state>,
        state_manipulator_single_immediate_interface<t_state_id, t_state, enter_exit_policy_notify, t_context>
    >
{
};

//----------------------------------------------------------------

/*
    Current state : single
    Switching     : combined
    Reactions     : call on_enter/on_exit functions of the state. The state in this case must be
                    a pointer type which provides these two functions.
*/
template
<
    typename t_state_id,
    typename t_state,
    typename t_context = void
>
class fsm_single_combined_enter_exit
    : public fsm
    <
        t_state_id,
        t_state,
        state_container_single_interface<t_state_id, t_state>,
        state_manipulator_single_combined_interface<t_state_id, t_state, enter_exit_policy_notify, t_context>
    >
{
};
//----------------------------------------------------------------

/*
    Current state : stack
    Switching     : immediate
    Reactions     : does not do anything
*/
template
<
    typename t_state_id,
    typename t_state
>
class fsm_stacked_immediate
    : public fsm
    <
        t_state_id,
        t_state,
        state_container_stacked_interface<t_state_id, t_state>,
        state_manipulator_stacked_immediate_interface<t_state_id, t_state>
    >
{
};

//----------------------------------------------------------------

/*
    Current state : stack
    Switching     : immediate
    Reactions     : call on_enter/on_exit functions of the state. The state in this case must be
                    a pointer type which provides these two functions.
*/
template
<
    typename t_state_id,
    typename t_state,
    typename t_context = void
>
class fsm_stacked_immediate_enter_exit
    : public fsm
    <
        t_state_id,
        t_state,
        state_container_stacked_interface<t_state_id, t_state>,
        state_manipulator_stacked_immediate_interface<t_state_id, t_state, enter_exit_policy_notify, t_context >
    >
{
};

//----------------------------------------------------------------

/*
    Current state : stack
    Switching     : combined
    Reactions     : call on_enter/on_exit functions of the state. The state in this case must be
                    a pointer type which provides these two functions.
*/
template
<
    typename t_state_id,
    typename t_state,
    typename t_context = void
>
class fsm_stacked_combined_enter_exit
    : public fsm
    <
        t_state_id,
        t_state,
        state_container_stacked_interface<t_state_id, t_state>,
        state_manipulator_stacked_combined_interface<t_state_id, t_state, enter_exit_policy_notify, t_context>
    >
{
};

//----------------------------------------------------------------
}
