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
struct state_container_stacked_impl
{
    typedef std::vector<state_and_id<t_state_id, t_state>*> current_states_vector;

    current_states_vector m_current_states;
};

template
<
    typename t_state_id,
    typename t_state
>
class state_container_stacked_interface
{
public:
    typedef state_container_stacked_impl<t_state_id, t_state> t_impl;
    typedef typename t_impl::current_states_vector current_states_vector;

    state_container_stacked_interface( t_impl& impl ) : m_impl( impl ) {}

    t_state get_top_state_id() const 
    { 
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_current_states;
        return !current_states.empty() ? current_states.back()->id : t_state_id; 
    }
    t_state get_top_state() const
    { 
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_current_states;
        return !current_states.empty() ? current_states.back()->state : 0; 
    }

    template<typename t_functor>
    void for_all_states_from_bottom( t_functor& f )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_current_states;

        for( unsigned int i = 0; i < current_states.size(); ++i )
            f( current_states[i].id, current_states[i].state );
    }

    template<typename t_functor>
    void for_all_states_from_top( t_functor& f )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_current_states;

        for( unsigned int i = current_states.size()-1; i >= 0; --i )
            f( current_states[i].id, current_states[i].state );
    }

    const current_states_vector& get_current_states() const { return m_impl.m_current_states; }

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
struct state_manipulator_stacked_immediate_impl
{
    typedef state_container_stacked_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_stacked_immediate_impl
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
class state_manipulator_stacked_immediate_interface
{
public:
    typedef state_manipulator_stacked_immediate_impl<t_state_id, t_state> t_impl;

    state_manipulator_stacked_immediate_interface( t_impl& impl ) : m_impl( impl ) {}

    bool replace_top_state( t_state_id id, context_holder<t_context> ctx = context_holder<t_context>() )
    {
        pop_state( ctx );
        return push_state( id, ctx );
    }

    bool push_state( t_state_id id, context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_state_container_impl.m_current_states;

        return insert_state( id, current_states.size(), ctx );
    }

    bool insert_state( t_state_id id, size_t position, context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_state_container_impl.m_current_states;
        if ( current_states.begin() + position > current_states.end() )
            return false;

        state_and_id<t_state_id, t_state>* new_state = m_impl.m_state_registry.find_state( id );
        if ( new_state == 0 )
            return false;

        state_container_stacked_impl<t_state_id, t_state>::current_states_vector::iterator iter;
        for( iter = current_states.begin(); iter != current_states.end(); ++iter )
            if ( (*iter)->id == id )
                break;

        if ( iter != current_states.end() )
            return false;

        current_states.insert( current_states.begin() + position, new_state );
        t_on_enter_exit_policy::on_enter( *new_state, ctx );

        return true;
    }

    bool pop_state( context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_state_container_impl.m_current_states;

        if ( current_states.empty() )
            return false;

        return remove_state( current_states.back()->id, ctx );
    }

    bool remove_state( t_state_id id, context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_state_container_impl.m_current_states;

        state_container_stacked_impl<t_state_id, t_state>::current_states_vector::iterator iter;
        for( iter = current_states.begin(); iter != current_states.end(); ++iter )
            if ( (*iter)->id == id )
                break;

        if ( iter == current_states.end() )
            return false;

        state_and_id<t_state_id, t_state> removed_state = **iter;        
        current_states.erase( iter );
        t_on_enter_exit_policy::on_exit( removed_state, ctx );

        return true;        
    }

    bool remove_state_and_all_above( t_state_id id, context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_state_container_impl.m_current_states;

        state_container_stacked_impl<t_state_id, t_state>::current_states_vector::iterator iter;
        for( iter = current_states.begin(); iter != current_states.end(); ++iter )
            if ( (*iter)->id == id )
                break;

        if ( iter == current_states.end() )
            return false;

        for( state_container_stacked_impl<t_state_id, t_state>::current_states_vector::reverse_iterator riter = current_states.rbegin(); riter.base() != iter; ++riter )
            t_on_enter_exit_policy::on_exit( *(*riter), ctx );

        current_states.erase( iter, current_states.end() );

        return true;
    }

    void remove_all_states( context_holder<t_context> ctx = context_holder<t_context>() )
    {
        state_container_stacked_impl<t_state_id, t_state>::current_states_vector& current_states = m_impl.m_state_container_impl.m_current_states;

        std::vector<t_state_id> states_to_remove;
        for ( state_container_stacked_impl<t_state_id, t_state>::current_states_vector::reverse_iterator iter = current_states.rbegin(); iter != current_states.rend(); ++iter )
            states_to_remove.push_back( (*iter)->id );

        for ( size_t i = 0; i < states_to_remove.size(); ++i )
            remove_state( states_to_remove[i], ctx );
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
struct state_manipulator_stacked_queued_impl_base
{
    typedef state_container_stacked_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_stacked_queued_impl_base
        ( 
            state_manipulator_stacked_immediate_impl<t_state_id, t_state>& immediate_impl,
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : m_immediate_impl( immediate_impl )
        , m_state_container_impl( state_container_impl ) 
        , m_state_registry( state_registry )
    {}
    
    state_manipulator_stacked_immediate_impl<t_state_id, t_state>& m_immediate_impl;
    t_state_container_impl& m_state_container_impl;
    state_registry<t_state_id, t_state>& m_state_registry;

    struct queued_action
    {
        enum action_id { push, pop, remove, remove_and_above, remove_all };

        queued_action( action_id action, t_state_id state ) : m_action(action), m_state_id(state) {}        
        
        action_id m_action;
        t_state_id m_state_id;
    };
    std::vector<queued_action> m_queued_actions;
};

template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_stacked_queued_interface_base
{
public:
    typedef state_manipulator_stacked_queued_impl_base<t_state_id, t_state> t_impl;

    state_manipulator_stacked_queued_interface_base(
        state_manipulator_stacked_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>& immediate_interface,
        t_impl& impl 
    ) 
        : m_immediate_interface( immediate_interface )
        , m_impl( impl ) 
    {}

    void queue_push_state( t_state_id id )
    {
        m_impl.m_queued_actions.push_back( t_impl::queued_action( t_impl::queued_action::push, id ) );
    }

    // It would dangrous to allow this, since position can become invalid because of other queued actions
    // bool queue_insert_state( t_state_id id, size_t position )

    void queue_pop_state()
    {
        m_impl.m_queued_actions.push_back( t_impl::queued_action( t_impl::queued_action::pop, t_state_id() ) );
    }

    void queue_remove_state( t_state_id id )
    {
        m_impl.m_queued_actions.push_back( t_impl::queued_action( t_impl::queued_action::remove, id ) );
    }

    void queue_remove_state_and_all_above( t_state_id id )
    {
        m_impl.m_queued_actions.push_back( t_impl::queued_action( t_impl::queued_action::remove_and_above, id ) );
    }

    void queue_remove_all_states()
    {
        m_impl.m_queued_actions.push_back( t_impl::queued_action( t_impl::queued_action::remove_all, t_state_id() ) );
    }

    void update( context_holder<t_context> ctx = context_holder<t_context>() )
    {
        for ( std::vector<t_impl::queued_action>::iterator iter = m_impl.m_queued_actions.begin();
              iter != m_impl.m_queued_actions.end();
              ++iter )
        {
            switch( iter->m_action )
            {
                case t_impl::queued_action::push:
                    m_immediate_interface.push_state( iter->m_state_id, ctx );
                    break;

                case t_impl::queued_action::pop:
                    m_immediate_interface.pop_state( ctx );
                    break;

                case t_impl::queued_action::remove:
                    m_immediate_interface.remove_state( iter->m_state_id, ctx );
                    break;

                case t_impl::queued_action::remove_and_above:
                    m_immediate_interface.remove_state_and_all_above( iter->m_state_id, ctx );
                    break;

                case t_impl::queued_action::remove_all:
                    m_immediate_interface.remove_all_states( ctx );
                    break;
            }
        }

        m_impl.m_queued_actions.clear();
    }

protected:
    t_impl& m_impl;
    state_manipulator_stacked_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>& m_immediate_interface;
};

//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_manipulator_stacked_queued_impl : public state_manipulator_stacked_queued_impl_base<t_state_id, t_state>
{
    typedef state_container_stacked_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_stacked_queued_impl
        (
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : state_manipulator_stacked_queued_impl_base<t_state_id, t_state>( m_immediate_impl, state_container_impl, state_registry )
        , m_immediate_impl( state_container_impl, state_registry )
    {}

    state_manipulator_stacked_immediate_impl<t_state_id, t_state> m_immediate_impl;
};

template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_stacked_queued_interface : public state_manipulator_stacked_queued_interface_base<t_state_id, t_state, t_on_enter_exit_policy, t_context>
{
public:
    typedef state_manipulator_stacked_queued_impl<t_state_id, t_state> t_impl;

    state_manipulator_stacked_queued_interface( t_impl& impl ) 
        : state_manipulator_stacked_queued_interface_base<t_state_id, t_state, t_on_enter_exit_policy, t_context>( m_immediate_interface, impl )
        , m_immediate_interface( impl.m_immediate_impl )
    {}

protected:
    state_manipulator_stacked_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context> m_immediate_interface;
};

//----------------------------------------------------------------

template
<
    typename t_state_id,
    typename t_state
>
struct state_manipulator_stacked_combined_impl : 
    public state_manipulator_stacked_immediate_impl<t_state_id, t_state>,
    public state_manipulator_stacked_queued_impl<t_state_id, t_state>
{
    typedef state_container_stacked_impl<t_state_id, t_state> t_state_container_impl;
    state_manipulator_stacked_combined_impl
        ( 
            t_state_container_impl& state_container_impl, 
            state_registry<t_state_id, t_state>& state_registry
        ) 
        : state_manipulator_stacked_immediate_impl( state_container_impl, state_registry )
        , state_manipulator_stacked_queued_impl<t_state_id, t_state>( state_container_impl, state_registry )
    {}
};

template
<
    typename t_state_id,
    typename t_state,
    typename t_on_enter_exit_policy = enter_exit_policy_default,
    typename t_context = void
>
class state_manipulator_stacked_combined_interface :
    public state_manipulator_stacked_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>,
    public state_manipulator_stacked_queued_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>
{
public:
    typedef state_manipulator_stacked_combined_impl<t_state_id, t_state> t_impl;

    state_manipulator_stacked_combined_interface( t_impl& impl ) 
        : state_manipulator_stacked_immediate_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>( impl )
        , state_manipulator_stacked_queued_interface<t_state_id, t_state, t_on_enter_exit_policy, t_context>( impl )
    {}
};
//----------------------------------------------------------------
}
