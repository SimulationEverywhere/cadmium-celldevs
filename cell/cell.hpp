/**
* Copyright (c) 2020, Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
* All rights reserved.
*
* Abstract cell atomic model base implementation
*/

#ifndef CADMIUM_CELLDEVS_ABSTRACT_CELL_HPP
#define CADMIUM_CELLDEVS_ABSTRACT_CELL_HPP

#include <string>
#include <limits>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <cassert>
#include <string>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include "delayer/delayer_factory.hpp"
#include "utils/common.hpp"

using namespace cadmium;
using namespace std;

/***************************************************/
/******************* CELL PORTS ********************/
/***************************************************/
template <typename C, typename S>
struct cell_ports_def{
    struct cell_in: public in_port<cell_state_message<C, S>> {};
    struct cell_out : public out_port<cell_state_message<C, S>> {};
};


/***************************************************/
/***************** ABSTRACT CELL *******************/
/***************************************************/
template <typename T, typename C, typename S, typename V, typename C_HASH=std::hash<C>>
class cell {
    public:
        // ports definition
        using input_ports = tuple<typename cell_ports_def<C, S>::cell_in>;
        using output_ports = tuple<typename cell_ports_def<C, S>::cell_out>;

        using NV = unordered_map<C, V, C_HASH>;
        using NS = unordered_map<C, S, C_HASH>;

        T clock;                                        // simulation clock
        delayer<T, S> *buffer;                          // output message buffer

        struct state_type {
            C cell_id;                                  // Cell ID
            S current_state;                            // Cell's internal state
            T next_internal;                            // Time remaining until next internal state transition
            NV neighbors_vicinity;                      // Neighboring cell' vicinity type
            NS neighbors_state;                         // neighboring cell' public state
        };
        state_type state;

        cell(){ assert(false && "Default constructor is not valid."); }

        cell(C const &cell_id, string const &delayer_id, S initial_state, NV const &vicinity) {
            assert(numeric_limits<T>::has_infinity && "This time base has does not define infinity");
            clock = T(); // Clock is set to 0
            buffer = delayer_factory<T, S>::create_delayer(delayer_id);
            state.cell_id = cell_id;
            state.current_state = initial_state;
            buffer->add_to_buffer(initial_state, T());
            state.next_internal = buffer->next_timeout() - clock;
            for (auto const &entry: vicinity) {
                state.neighbors_vicinity[entry.first] = entry.second;
                state.neighbors_state[entry.first];  // Neighbors' initial state is set as the default
            }
        }

        /***************** User-Defined *******************/
        // user must define this function. It returns the next cell state and its corresponding timeout
        virtual S local_computation() const {
            return state.current_state;
        }
        // It returns the delay to communicate cell's new state.
        virtual T output_delay(S const &cell_state) const {
            return numeric_limits<T>::infinity();
        }
        // checks if two states are different. Depending on the state's complexity, user may want to overwrite this function
        virtual bool different_states(S const &first, S const &second) const {
            return first != second;
        }

        void update_cell_state(S new_state) {
            // Update current state
            state.current_state = new_state;
            // Compute output delay and add new message to output buffer
            T delay = output_delay(new_state);
            buffer->add_to_buffer(new_state, clock + delay);
            // Update next internal due to an scheduled output event
            state.next_internal = buffer->next_timeout() - clock;
        }

        // internal transition
        void internal_transition() {
            // Update clock and next internal events
            clock += time_advance();
            buffer->pop_buffer();
            state.next_internal = buffer->next_timeout() - clock;
        }

        // external transition
        void external_transition(T e, typename make_message_bags<input_ports>::type mbs) {
            // Update clock and next internal event
            clock += e;
            state.next_internal -= e;
            // Refresh the neighbors' current state
            vector<cell_state_message<C, S>> bagPortIn = get_messages<typename cell_ports_def<C, S>::cell_in>(mbs);
            for (cell_state_message<C, S> msg: bagPortIn) {
                auto it = state.neighbors_state.find(msg.cell_id);
                if (it != state.neighbors_state.end()) {
                    state.neighbors_state[msg.cell_id] = msg.state;
                }
            }
            // Compute next state
            S next = local_computation();
            // If next state is not the current state, then I change my state and schedule my next internal transition
            if (different_states(next, state.current_state)) {
                update_cell_state(next);
            }
        }

        void confluence_transition(T e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(T(), move(mbs));
        }

        T time_advance() const { return state.next_internal; }

        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bag;
            S public_state = buffer->next_state();
            vector<cell_state_message<C, S>> bag_port_out = {cell_state_message<C, S>(state.cell_id, public_state)};
            get_messages<typename cell_ports_def<C, S>::cell_out>(bag) = bag_port_out;
            return bag;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os,
                const typename cell<T, C, S, V, C_HASH>::state_type& i) {
            os << i.current_state;
            return os;
        }
};

#endif //CADMIUM_CELLDEVS_ABSTRACT_CELL_HPP
