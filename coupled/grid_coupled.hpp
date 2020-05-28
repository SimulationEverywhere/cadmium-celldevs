//
// Created by Román Cárdenas Rodríguez on 21/05/2020.
//

#ifndef CADMIUM_CELLDEVS_GRID_COUPLED_HPP
#define CADMIUM_CELLDEVS_GRID_COUPLED_HPP

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>

#include "cell/grid_cell.hpp"
#include "utils/grid_utils.hpp"
#include "cells_coupled.hpp"

using namespace std;
using namespace cadmium::dynamic::modeling;

template <typename T, typename S, typename V=int>
class grid_coupled: public cells_coupled<T, cell_position, S, V, seq_hash<cell_position>> {
    public:
        using cells_coupled<T, cell_position, S, V, seq_hash<cell_position>>::get_cell_name;
        using cells_coupled<T, cell_position, S, V, seq_hash<cell_position>>::add_cell_vicinity;

        explicit grid_coupled(string const &id) : cells_coupled<T, cell_position, S, V, seq_hash<cell_position>>(id) {}

        template <template <typename> typename CELL_MODEL, typename... Args>
        void add_lattice(grid_scenario<S, V> &scenario, string const &delayer_id, Args&&... args) {
            for (auto const &cell: scenario.get_states()) {
                cell_position cell_id = cell.first;
                cell_map<S, V> map = scenario.get_cell_map(cell_id);
                coupled<T>::_models.push_back(
                        cadmium::dynamic::translate::make_dynamic_atomic_model<CELL_MODEL, T>(get_cell_name(cell_id),
                                                                                              map, delayer_id,
                                                                                              forward<Args>(args)...));
            }
            for (auto const &cell: scenario.get_states()) {
                cell_position cell_to = cell.first;
                cell_unordered<V> vicinities = scenario.get_cell_map(cell_to).vicinity;
                add_cell_vicinity(cell_to, vicinities);
            }
        }
};

#endif //CADMIUM_CELLDEVS_GRID_COUPLED_HPP
