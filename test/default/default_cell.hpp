//
// Created by Román Cárdenas Rodríguez on 25/05/2020.
//

#ifndef CADMIUM_CELLDEVS_DEFAULT_CELL_HPP
#define CADMIUM_CELLDEVS_DEFAULT_CELL_HPP

#include "cell/cell.hpp"

template <typename T>
class default_cell: public cell<T, int, int, int> {
public:
    using cell<T, int, int, int>::state;
    default_cell(){ assert(false && "Default constructor is not valid."); }

    default_cell(int cell_id, std::string const &delayer_name, int initial_state, std::unordered_map<int, int> const &vicinity):
            cell<T, int, int, int>(cell_id, delayer_name, initial_state, vicinity) {}

    // user must define this function. It returns the next cell state and its corresponding timeout
    int local_computation() const override {
        int res = state.current_state;
        for (auto other: state.neighbors_state) {
            int prev = other.second * state.neighbors_vicinity.at(other.first);
            res = (prev > res)? prev : res;
        }
        return res;
    }
    // It returns the delay to communicate cell's new state.
    T output_delay(int const &cell_state) const override {
        return 3 - state.cell_id;
    }
};

#endif //CADMIUM_CELLDEVS_DEFAULT_CELL_HPP
