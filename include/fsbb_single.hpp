#pragma once

#include "fsbb_common.hpp"

namespace fsbb
{
//----------------------------------------------------------------
// State containers ( impl; interface )
//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_container_single_impl
{
    state_container_single_impl() : m_current_state(0) {}

    state_and_id<t_state_id, t_state> *m_current_state;
};

template
<
    typename t_state_id,
    typename t_state
>
class state_container_single_interface
{
public:
    typedef state_container_single_impl<t_state_id, t_state> t_impl;

    state_container_single_interface( t_impl& impl ) : m_impl( impl ) {}

    t_state_id get_current_state_id() const { return m_impl.m_current_state ? m_impl.m_current_state->id : t_state_id(); }
    const t_state get_current_state() const { return m_impl.m_current_state ? m_impl.m_current_state->state : 0; }

protected:
    t_impl& m_impl;
};

//----------------------------------------------------------------
// State manipulators ( impl; interface )
//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_manipulator_single_immediate_impl
{
    typedef state_container_single_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_single_immediate_impl
        ( 
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : m_state_container_impl( state_container_impl ) 
        , m_state_registry( state_registry )
    {}
    
    t_state_container_impl& m_state_container_impl;
    state_registry<t_state_id, t_state>& m_state_registry;
};

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
    typedef state_manipulator_single_immediate_impl<t_state_id, t_state> t_impl;

    state_manipulator_single_immediate_interface( t_impl& impl ) : m_impl( impl ) {}

    bool change_state_immediate( t_state_id id, context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_and_id<t_state_id, t_state>* new_state = m_impl.m_state_registry.find_state( id );
        if ( new_state == 0 )
            return false;

        state_and_id<t_state_id, t_state> *& current_state = m_impl.m_state_container_impl.m_current_state;
        if ( current_state != 0 )
            t_on_enter_exit_policy::on_exit( *current_state, ctx );
        
        current_state = new_state;

        t_on_enter_exit_policy::on_enter( *current_state, ctx );

        return true;
    }

protected:
    t_impl& m_impl;
};

//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_manipulator_single_queued_impl_base
{
    typedef state_container_single_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_single_queued_impl_base
        ( 
            state_manipulator_single_immediate_impl<t_state_id, t_state>& immediate_impl,
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : m_immediate_impl( immediate_impl )
        , m_state_container_impl( state_container_impl ) 
        , m_state_registry( state_registry )
        , m_next_state( 0 )
    {}
    
    state_manipulator_single_immediate_impl<t_state_id, t_state>& m_immediate_impl;
    t_state_container_impl& m_state_container_impl;
    state_registry<t_state_id, t_state>& m_state_registry;
    state_and_id<t_state_id, t_state> *m_next_state;
};

template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_single_queued_interface_base
{
public:
    typedef state_manipulator_single_queued_impl_base<t_state_id, t_state> t_impl;

    state_manipulator_single_queued_interface_base(
        state_manipulator_single_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>& immediate_interface,
        t_impl& impl 
    ) 
        : m_immediate_interface( immediate_interface )
        , m_impl( impl ) 
    {}

    bool queue_change_state( t_state_id id )
    {
        state_and_id<t_state_id, t_state>* new_state = m_impl.m_state_registry.find_state( id );
        if ( new_state == 0 )
            return false;

        m_impl.m_next_state = new_state;

        return true;
    }

    void update( context_holder<t_context> ctx )
    {
        if ( !m_impl.m_next_state )
            return;

        state_and_id<t_state_id, t_state>* next_state = m_impl.m_next_state;
        m_impl.m_next_state = 0;

        m_immediate_interface.change_state_immediate( next_state->id, ctx );        
    }

    template<typename T = t_context>
    typename std::enable_if<std::is_void<T>::value, void>::type update()
    {
        update(context_holder<void>());
    }

protected:
    t_impl& m_impl;
    state_manipulator_single_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>& m_immediate_interface;
};

//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_manipulator_single_queued_impl : public state_manipulator_single_queued_impl_base<t_state_id, t_state>
{
    typedef state_container_single_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_single_queued_impl
        (
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : state_manipulator_single_queued_impl_base<t_state_id, t_state>( m_immediate_impl, state_container_impl, state_registry )
        , m_immediate_impl( state_container_impl, state_registry )
    {}

    state_manipulator_single_immediate_impl<t_state_id, t_state> m_immediate_impl;
};

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
    typedef state_manipulator_single_queued_impl<t_state_id, t_state> t_impl;

    state_manipulator_single_queued_interface( t_impl& impl ) 
        : state_manipulator_single_queued_interface_base<t_state_id, t_state, t_on_enter_exit_policy, t_context>( m_immediate_interface, impl )
        , m_immediate_interface( impl.m_immediate_impl )
    {}

protected:
    state_manipulator_single_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context> m_immediate_interface;
};

//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_manipulator_single_combined_impl : 
    public state_manipulator_single_immediate_impl<t_state_id, t_state>,
    public state_manipulator_single_queued_impl<t_state_id, t_state>
{
    typedef state_container_single_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_single_combined_impl
        ( 
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : state_manipulator_single_immediate_impl<t_state_id, t_state>( state_container_impl, state_registry )
        , state_manipulator_single_queued_impl<t_state_id, t_state>( state_container_impl, state_registry )
    {}
};

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
    typedef state_manipulator_single_combined_impl<t_state_id, t_state> t_impl;

    state_manipulator_single_combined_interface( t_impl& impl ) 
        : state_manipulator_single_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>( impl )
        , state_manipulator_single_queued_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>( impl )
    {}
};

//----------------------------------------------------------------
}
