/**
* Copyright (c) 2020, Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
* All rights reserved.
*
* Cells for modeling lattice-based Cell-DEVS scenarios
*/

#ifndef GRID_CELLS_HPP
#define GRID_CELLS_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include "cell.hpp"
#include "utils/common.hpp"
#include "utils/grid_utils.hpp"

template <typename T, typename S, typename V>
class grid_cell : public cell<T, cell_position, S, V, seq_hash<cell_position>> {
    public:
        using NV = unordered_map<cell_position, V, seq_hash<cell_position>>;

        cell_map<S, V> map;
        grid_cell(){ assert(false && "Default constructor is not valid."); }

        grid_cell(cell_map<S, V> const &map_in, std::string const &delayer_id):
                cell<T, cell_position, S, V, seq_hash<cell_position>>(map_in.location, delayer_id, map_in.state, map_in.vicinity),
                map{map_in} {}
};

#endif //GRID_CELLS_HPP
