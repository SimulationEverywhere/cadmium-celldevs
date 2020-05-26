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

template <template <typename> typename CELL_MODEL, typename T, typename S, typename V>
class grid_coupled: public coupled<T> {
    public:

        using C = cell_position;
        using C_HASH = seq_hash<C>;
        using NS = unordered_map<C, S, C_HASH>;
        using NV = unordered_map<C, V, C_HASH>;
        using CNV = unordered_map<C, V, C_HASH>;

        grid_coupled(string const &id, string const &delayer_id, grid_scenario<S, V> &scenario) : coupled<T>(id) {
            cadmium::dynamic::modeling::Models atomic_cells;
            cadmium::dynamic::modeling::ICs ics;
            for (auto const &cell: scenario.get_states()) {
                cell_position cell_id = cell.first;
                cell_map<S, V> map = scenario.get_cell_map(cell_id);
                coupled<T>::_models.push_back(cadmium::dynamic::translate::make_dynamic_atomic_model<CELL_MODEL, T>(get_cell_name(cell_id), map, delayer_id));
            }
            for (auto const &cell: scenario.get_states()) {
                cell_position cell_to = cell.first;
                cell_unordered<V> vicinities = scenario.get_cell_map(cell_to).vicinity;
                for (auto const &neighbor: vicinities) {
                    C cell_from = neighbor.first;
                    coupled<T>::_ic.push_back(
                            cadmium::dynamic::translate::make_IC<
                                    typename std::tuple_element<0, typename CELL_MODEL<T>::output_ports>::type,
                                    typename std::tuple_element<0, typename CELL_MODEL<T>::input_ports>::type
                            >(get_cell_name(cell_from), get_cell_name(cell_to))
                    );
                }
            }
        }

        string get_cell_name(C const &cell_id) const {
            std::stringstream model_name;
            model_name << coupled<T>::get_id() << "_cell_" << cell_id;
            return model_name.str();
        }
};

#endif //CADMIUM_CELLDEVS_GRID_COUPLED_HPP
