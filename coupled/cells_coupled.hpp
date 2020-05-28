//
// Created by Román Cárdenas Rodríguez on 20/05/2020.
//

#ifndef CADMIUM_CELLDEVS_CELLS_COUPLED_HPP
#define CADMIUM_CELLDEVS_CELLS_COUPLED_HPP

#include <exception>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>

#include "cell/cell.hpp"
#include "cell/grid_cell.hpp"
#include "utils/common.hpp"
#include "utils/grid_utils.hpp"

using namespace std;
using namespace cadmium::dynamic::modeling;

template<typename T, typename C, typename S, typename V=int, typename C_HASH=std::hash<C>>
class cells_coupled: public coupled<T> {
    public:

        using CV = std::unordered_map<C, V, C_HASH>;
        using CNV = std::unordered_map<C, CV, C_HASH>;

        CNV vicinities;

        explicit cells_coupled(string const &id) : coupled<T>(id), vicinities() {}

        template <template <typename> typename CELL_MODEL, typename... Args>
        void add_cell(C const &cell_id, S const &initial_state, CV const &vicinity, string const &delayer_id, Args&&... args) {
            add_cell_vicinity(cell_id, vicinity);
            coupled<T>::_models.push_back(
                    cadmium::dynamic::translate::make_dynamic_atomic_model<CELL_MODEL, T>(get_cell_name(cell_id),
                                                                                          cell_id, initial_state,
                                                                                          vicinity, delayer_id,
                                                                                          std::forward<Args>(args)...));
        }

        void add_cell_vicinity(C const &cell_id, CV const &vicinity) {
            auto it = vicinities.find(cell_id);
            if (it != vicinities.end()) {
                throw std::bad_typeid();
            }
            vicinities.insert({cell_id, vicinity});
        }

        template <template <typename> typename CELL_MODEL, typename... Args>
        void add_cell(C const &cell_id, S const &initial_state, std::vector<C> const &neighbors, string const &delayer_id, Args&&... args) {
            CV vicinity = CV();
            for (auto const &neighbor: neighbors) {
                vicinity.insert({neighbor, V()});
            }
            add_cell<CELL_MODEL, Args...>(cell_id, initial_state, vicinity, delayer_id, std::forward<Args>(args)...);
        }

        void couple_cells() {
            for (auto const &neighborhood: vicinities) {
                C cell_to = neighborhood.first;
                unordered_map<C, V, C_HASH> neighbors = neighborhood.second;
                for (auto const &neighbor: neighbors) {
                    C cell_from = neighbor.first;
                    V vicinity = neighbor.second;
                    coupled<T>::_ic.push_back(
                            cadmium::dynamic::translate::make_IC<
                                    typename std::tuple_element<0, typename cell<T, C, S, V, C_HASH>::output_ports>::type,
                                    typename std::tuple_element<0, typename cell<T, C, S, V, C_HASH>::input_ports>::type
                            >(get_cell_name(cell_from), get_cell_name(cell_to))
                    );
                }
            }
        }

        string get_cell_name(C const &cell_id) const {
            std::stringstream model_name;
            model_name << coupled<T>::get_id() << "_" << cell_id;
            return model_name.str();
        }
};

#endif //CADMIUM_CELLDEVS_CELLS_COUPLED_HPP
