#pragma once

#include <vector>

namespace fsbb
{
//----------------------------------------------------------------

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

//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
class state_registry
{
public:
    void register_state( t_state_id id, t_state state )
    {
        state_and_id<t_state_id, t_state> s;
        s.id = id;
        s.state = state;
        m_states.push_back( s );
    }

    state_and_id<t_state_id, t_state>* find_state( t_state_id id )
    {
        for ( size_t i = 0; i < m_states.size(); ++i )
        {
            if ( m_states[i].id == id )
                return &m_states[i];
        }

        return 0;
    }

protected:
    std::vector<state_and_id<t_state_id, t_state>> m_states;
};

//----------------------------------------------------------------

template<typename T>
struct context_holder
{
    context_holder( T t )
        : m_context(t)
    {
    }

    T m_context;
};

template<>
struct context_holder<void>
{
    context_holder()
    {
    }
};

//----------------------------------------------------------------

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
public:
    fsm()
        : t_state_container_interface( m_state_container )
        , t_state_manipulator_interface( m_state_manipulator )
        , m_state_manipulator( m_state_container, *this )
    {}

protected:
    typename t_state_container_interface::t_impl m_state_container;
    typename t_state_manipulator_interface::t_impl m_state_manipulator;
};

//----------------------------------------------------------------
// Enter/Exit policies
//----------------------------------------------------------------

struct enter_exit_policy_default
{
    template<typename t_state_id, typename t_state, typename t_context = void>
    static void on_enter( state_and_id<t_state_id, t_state>& state, context_holder<t_context>& ctx ) {}

    template<typename t_state_id, typename t_state, typename t_context = void>
    static void on_exit( state_and_id<t_state_id, t_state>& state, context_holder<t_context>& ctx ) {}
};

//----------------------------------------------------------------

struct enter_exit_policy_notify
{
    template<typename t_state_id, typename t_state, typename t_context>
    static void on_enter( state_and_id<t_state_id, t_state>& state, context_holder<t_context>& ctx ) { state.state->on_enter( ctx.m_context ); }

    template<typename t_state_id, typename t_state>
    static void on_enter( state_and_id<t_state_id, t_state>& state, context_holder<void>& ctx ) { state.state->on_enter(); }

    template<typename t_state_id, typename t_state, typename t_context>
    static void on_exit( state_and_id<t_state_id, t_state>& state, context_holder<t_context>& ctx ) { state.state->on_exit( ctx.m_context ); }

    template<typename t_state_id, typename t_state>
    static void on_exit( state_and_id<t_state_id, t_state>& state, context_holder<void>& ctx ) { state.state->on_exit(); }
};

//----------------------------------------------------------------

struct enter_exit_policy_call
{
    template<typename t_state_id, typename t_state, typename t_context>
    static void on_enter( state_and_id<t_state_id, t_state>& state, context_holder<t_context>& ctx ) { state.state( ctx.m_context ); }

    template<typename t_state_id, typename t_state, typename t_context>
    static void on_enter( state_and_id<t_state_id, t_state>& state, context_holder<void>& ctx ) { state.state(); }

    template<typename t_state_id, typename t_state, typename t_context = void>
    static void on_exit( state_and_id<t_state_id, t_state>& state, context_holder<t_context>& ctx ) {}
};

//----------------------------------------------------------------
}
