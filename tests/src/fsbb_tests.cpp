#include "fsbb_prefabs.hpp"
#include <vector>
#include <assert.h>

using namespace fsbb;

const int CONTEXT = 1;

struct test_action
{
    enum type{ enter, exit } m_type;
    int m_state_id;
};

std::vector<test_action> g_test_actions;

class state
{
public:
    state( int id ) : m_id(id) {}

    int get_id() const { return m_id; }
    void on_enter( int ctx ) { test_action a; a.m_type = test_action::enter; a.m_state_id = m_id; g_test_actions.push_back( a ); }
    void on_exit( int ctx ) { test_action a; a.m_type = test_action::exit; a.m_state_id = m_id; g_test_actions.push_back( a ); }

private:
    int m_id;    
};

//----------------------------------------------------------------

void test_simple_fsm()
{
    fsm_single_combined_enter_exit<int, state*, int> test1;
    
    g_test_actions.clear();

    test1.register_state( 1, new state( 1 ) );
    test1.register_state( 2, new state( 2 ) );
    test1.register_state( 3, new state( 3 ) );

    //  // Check that FSM correctly enters the first state
    assert( test1.change_state_immediate( 1, CONTEXT ) );
    assert( test1.get_current_state() != 0 && test1.get_current_state()->get_id() == 1 );
    assert( g_test_actions[0].m_type == test_action::enter && g_test_actions[0].m_state_id == 1 );

    g_test_actions.clear();

    //  // Check that FSM correctly changes states
    assert( test1.change_state_immediate( 3, CONTEXT ) );
    assert( test1.get_current_state() != 0 && test1.get_current_state()->get_id() == 3 );
    assert( g_test_actions[0].m_type == test_action::exit && g_test_actions[0].m_state_id == 1 );
    assert( g_test_actions[1].m_type == test_action::enter && g_test_actions[1].m_state_id == 3 );

    //  // Check that FSM correctly queues state change without modifying its state
    g_test_actions.clear();
    test1.queue_change_state( 2 );
    assert( test1.get_current_state() != 0 && test1.get_current_state()->get_id() == 3 );
    assert( g_test_actions.empty() );

    //  // Check that FSM correctly acts on queued changes
    test1.update( CONTEXT );
    assert( test1.get_current_state() != 0 && test1.get_current_state()->get_id() == 2 );
    assert( g_test_actions[0].m_type == test_action::exit && g_test_actions[0].m_state_id == 3 );
    assert( g_test_actions[1].m_type == test_action::enter && g_test_actions[1].m_state_id == 2 );

    //  // Check that FSM rejects non-existent states
    assert( !test1.change_state_immediate( 4, CONTEXT ) );
    assert( !test1.queue_change_state( 4 ) );
}

void test_stacked_fsm()
{
    g_test_actions.clear();

    fsm_stacked_immediate_enter_exit<int, state*, int> test1;
    test1.register_state( 1, new state( 1 ) );
    test1.register_state( 2, new state( 2 ) );
    test1.register_state( 3, new state( 3 ) );

      // Check that FSM correctly enters the first state
    assert( test1.push_state( 1, CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 1 );
    assert( g_test_actions[0].m_type == test_action::enter && g_test_actions[0].m_state_id == 1 );

    g_test_actions.clear();

      // Check that FSM correctly pushes the second state
    assert( test1.push_state( 2, CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 2 );
    assert( g_test_actions[0].m_type == test_action::enter && g_test_actions[0].m_state_id == 2 );

    g_test_actions.clear();

      // Check that FSM correctly pops a state
    assert( test1.pop_state( CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 1 );
    assert( g_test_actions[0].m_type == test_action::exit && g_test_actions[0].m_state_id == 2 );

    g_test_actions.clear();

      // Check that FSM correctly inserts a state into the end of the stack
    assert( test1.insert_state( 2, 0, CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 1 );
    assert( g_test_actions[0].m_type == test_action::enter && g_test_actions[0].m_state_id == 2 );

    g_test_actions.clear();

      // Check that FSM correctly inserts a state into the middle of the stack
    assert( test1.insert_state( 3, 1, CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 1 );
    assert( g_test_actions[0].m_type == test_action::enter && g_test_actions[0].m_state_id == 3 );

    g_test_actions.clear();

      // Check that FSM correctly removes a state from the end of the stack
    assert( test1.remove_state( 2, CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 1 );
    assert( g_test_actions[0].m_type == test_action::exit && g_test_actions[0].m_state_id == 2 );

    g_test_actions.clear();
      
    assert( test1.insert_state( 2, 0, CONTEXT ) );
    
    g_test_actions.clear();

      // Check that FSM correctly removes a state and all above it    
    assert( test1.remove_state_and_all_above( 3, CONTEXT ) );
    assert( test1.get_top_state() != 0 && test1.get_top_state()->get_id() == 2 );
    assert( g_test_actions[0].m_type == test_action::exit && g_test_actions[0].m_state_id == 1 );
    assert( g_test_actions[1].m_type == test_action::exit && g_test_actions[1].m_state_id == 3 );

    g_test_actions.clear();

      // Check that FSM correctly removes all states
    test1.remove_all_states( CONTEXT );
    assert( test1.get_top_state() == 0 );
    assert( g_test_actions[0].m_type == test_action::exit && g_test_actions[0].m_state_id == 2 );
}

void test_stacked_queued_fsm()
{
    g_test_actions.clear();

    fsm_stacked_combined_enter_exit<int, state*, int> test1;
    test1.register_state( 1, new state( 1 ) );
    test1.register_state( 2, new state( 2 ) );
    test1.register_state( 3, new state( 3 ) );

      // Check that FSM correctly enters the first state
    test1.queue_push_state( 1 );
    test1.queue_push_state( 2 );
    test1.queue_pop_state();
    test1.queue_remove_state( 1 );

    test1.queue_push_state( 1 );
    test1.queue_push_state( 3 );
    test1.queue_push_state( 2 );

    test1.queue_remove_state_and_all_above( 3 );
    test1.queue_remove_all_states();

    test1.update( CONTEXT );

    assert( test1.get_top_state() == 0 );
    
    assert( g_test_actions[0].m_type == test_action::enter && g_test_actions[0].m_state_id == 1 );
    assert( g_test_actions[1].m_type == test_action::enter && g_test_actions[1].m_state_id == 2 );
    assert( g_test_actions[2].m_type == test_action::exit && g_test_actions[2].m_state_id == 2 );
    assert( g_test_actions[3].m_type == test_action::exit && g_test_actions[3].m_state_id == 1 );

    assert( g_test_actions[4].m_type == test_action::enter && g_test_actions[4].m_state_id == 1 );
    assert( g_test_actions[5].m_type == test_action::enter && g_test_actions[5].m_state_id == 3 );
    assert( g_test_actions[6].m_type == test_action::enter && g_test_actions[6].m_state_id == 2 );

    assert( g_test_actions[7].m_type == test_action::exit && g_test_actions[7].m_state_id == 2 );
    assert( g_test_actions[8].m_type == test_action::exit && g_test_actions[8].m_state_id == 3 );

    assert( g_test_actions[9].m_type == test_action::exit && g_test_actions[9].m_state_id == 1 );
}

int main( int argc, char** argv )
{
    test_simple_fsm();
    test_stacked_fsm();
    test_stacked_queued_fsm();
}
