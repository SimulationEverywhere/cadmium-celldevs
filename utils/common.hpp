/**
* Copyright (c) 2020, Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
* All rights reserved.
*
* Cell-DEVS common utils: bunch of common useful definitions for implementing Cell-DEVS in Cadmium
*/

#ifndef CADMIUM_CELLDEVS_UTILS_HPP
#define CADMIUM_CELLDEVS_UTILS_HPP

#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <cassert>
#include <iostream>
#include <string>

#include <boost/functional/hash.hpp>


/***************************************************/
/********************** Utils **********************/
/***************************************************/
// Hash function for enabling sequences to be  keys of unordered maps
template <typename SEQ>
struct seq_hash {
    std::size_t operator() (SEQ const &seq) const {
        return boost::hash_range(seq.begin(), seq.end());
    }
};

// Dictionary for keys represented as sequences (valid for strings, arrays, and vectors, for instance)
template <typename K, typename V>
using unordered_seq_map = std::unordered_map<K , V, seq_hash<K>>;

/***************************************************/
/******************** Messages *********************/
/***************************************************/
// Messages between cell MUST include the ID of the source cell
template <typename C, typename S>
struct cell_state_message {
    C cell_id;
    S state;
    explicit cell_state_message(C cell_id_in, S state_in): cell_id(cell_id_in), state(state_in) {}
};

/***************************************************/
/***************** Output streams ******************/
/***************************************************/
// For printing vectors (i.e., cell position)
template <typename T>
std::ostream &operator <<(std::ostream &os, std::vector<T> const &v) {
    os << "(";
    std::string separator;
    for (auto x : v) {
        os << separator << x;
        separator = ",";
    }
    os << ")";
    return os;
}
// For printing cell messages
template <typename C, typename S>
std::ostream &operator << (std::ostream &os, const cell_state_message<C, S> &msg) {
    os << msg.cell_id << " ; " << msg.state;  // User must define the serialization of the cell state
    return os;
}

#endif //CADMIUM_CELLDEVS_UTILS_HPP
