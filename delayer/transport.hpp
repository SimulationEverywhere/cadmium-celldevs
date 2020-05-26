//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#ifndef CADMIUM_CELLDEVS_TRANSPORT_HPP
#define CADMIUM_CELLDEVS_TRANSPORT_HPP

#include <functional>
#include <limits>
#include <queue>
#include <tuple>
#include <vector>
#include "delayer.hpp"

template<typename T, typename S>
class transport_delayer : public delayer<T, S> {
    private:
        std::priority_queue<std::pair<T, S>, std::vector<std::pair<T, S>>, std::greater<std::pair<T, S>>> delayed_outputs;
    public:
        transport_delayer() : delayer<T,S>() {}
        void add_to_buffer(S state, T scheduled_time) override {
            delayed_outputs.push(std::make_pair(scheduled_time, state));
        }
        T next_timeout() const override {
            if (delayed_outputs.empty()) {
                return std::numeric_limits<T>::infinity();
            } else {
                return delayed_outputs.top().first;
            }
        }
        S next_state() const override { return delayed_outputs.top().second; }
        void pop_buffer() override { delayed_outputs.pop(); }
};

#endif //CADMIUM_CELLDEVS_TRANSPORT_HPP
