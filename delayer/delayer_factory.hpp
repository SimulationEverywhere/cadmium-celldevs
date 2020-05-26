//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#ifndef CADMIUM_CELLDEVS_FACTORY_HPP
#define CADMIUM_CELLDEVS_FACTORY_HPP

#include <string>
#include "delayer.hpp"
#include "inertial.hpp"
#include "transport.hpp"

template <typename T, typename S>
class delayer_factory {
public:
    static delayer<T, S> *create_delayer(std::string const &delayer_name) {
        if (delayer_name == "inertial") {
            return new inertial_delayer<T, S>();
        } else if (delayer_name == "transport") {
            return new transport_delayer<T, S>();
        } else {
            throw -1;
        }
    }
};

#endif //CADMIUM_CELLDEVS_FACTORY_HPP
