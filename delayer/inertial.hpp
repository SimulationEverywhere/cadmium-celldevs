//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#ifndef CADMIUM_CELLDEVS_INERTIAL_DELAYER_HPP
#define CADMIUM_CELLDEVS_INERTIAL_DELAYER_HPP

#include <limits>
#include "delayer.hpp"

template<typename T, typename S>
class inertial_delayer : public delayer<T, S> {
    private:
        S state_buffer;
        T time;
    public:
        inertial_delayer() : delayer<T,S>() {
            state_buffer = S();
            time = std::numeric_limits<T>::infinity();
        }
        void add_to_buffer(S state, T scheduled_time) override {
            state_buffer = state;
            time = scheduled_time;
        }
        T next_timeout() const override { return time; }
        S next_state() const override { return state_buffer; }
        void pop_buffer() override { time = std::numeric_limits<T>::infinity(); }
};

#endif //CADMIUM_CELLDEVS_INERTIAL_DELAYER_HPP
