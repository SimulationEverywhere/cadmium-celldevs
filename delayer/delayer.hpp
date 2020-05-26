//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#ifndef CADMIUM_CELLDEVS_DELAYER_HPP
#define CADMIUM_CELLDEVS_DELAYER_HPP

#include <limits>

template<typename T, typename S>
class delayer {
    public:
        virtual void add_to_buffer(S state, T scheduled_time) {};
        virtual T next_timeout() const { return std::numeric_limits<T>::infinity(); };
        virtual S next_state() const { return S(); };
        virtual void pop_buffer() {};
};

#endif //CADMIUM_CELLDEVS_DELAYER_HPP
