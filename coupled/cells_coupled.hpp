//
// Created by Román Cárdenas Rodríguez on 20/05/2020.
//

#ifndef CADMIUM_CELLDEVS_CELLS_COUPLED_HPP
#define CADMIUM_CELLDEVS_CELLS_COUPLED_HPP

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>

#include "cell/cell.hpp"
#include "utils/common.hpp"

using namespace std;
using namespace cadmium::dynamic::modeling;

template <template <typename> typename CELL_MODEL, typename T, typename C, typename S, typename V, typename C_HASH=std::hash<C>>
class cells_coupled: public coupled<T> {
    public:
        using NS = unordered_map<C, S, C_HASH>;
        using NV = unordered_map<C, V, C_HASH>;
        using CNV = unordered_map<C, NV, C_HASH>;

        cells_coupled(string const &id, string const &delayer_id, NS const &initial_states, CNV const &vicinities) : coupled<T>(id) {
            cadmium::dynamic::modeling::Models atomic_cells;
            cadmium::dynamic::modeling::ICs ics;
            for (auto const &cell: initial_states) {
                C cell_id = cell.first;
                S initial_state = cell.second;
                NV vicinity = vicinities.at(cell_id);
                coupled<T>::_models.push_back(
                        cadmium::dynamic::translate::make_dynamic_atomic_model<CELL_MODEL, T>(get_cell_name(cell_id), cell_id, delayer_id, initial_state, vicinity)
                );
            }
            for (auto const &neighborhood: vicinities) {
                C cell_to = neighborhood.first;
                NV neighbors = neighborhood.second;
                for (auto const &neighbor: neighbors) {
                    C cell_from = neighbor.first;
                    V vicinity = neighbor.second;
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

#endif //CADMIUM_CELLDEVS_CELLS_COUPLED_HPP
