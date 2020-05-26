//
// Created by Román Cárdenas Rodríguez on 25/05/2020.
//

#include <fstream>

#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include "coupled/cells_coupled.hpp"
#include "delayer/inertial.hpp"
#include "delayer/transport.hpp"
#include "default_cell.hpp"

using TIME = float;
std::string delayer_id = "inertial";
/*************** Loggers *******************/
static ofstream out_messages("../simulation_results/default/" + delayer_id + "_output_messages.txt");
struct oss_sink_messages{
    static ostream& sink(){
        return out_messages;
    }
};
static ofstream out_state("../simulation_results/default/" + delayer_id + "_state.txt");
struct oss_sink_state{
    static ostream& sink(){
        return out_state;
    }
};

using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

int main() {
    std::unordered_map<int, int> states = std::unordered_map<int, int>();
    std::unordered_map<int, std::unordered_map<int, int>> vicinities = std::unordered_map<int, std::unordered_map<int, int>>();
    for (int i = 0; i < 4; i++) {
        states[i] = i;
        for (int j = i - 1; 0 <= j  && j <= i; j++) {
            vicinities[j][i] = 1;
        }
    }

    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> test = std::make_shared<cells_coupled<default_cell, TIME, int, int, int>>(
            "test", delayer_id, states, vicinities);

    cadmium::dynamic::engine::runner<TIME, logger_top> r(test, {0});
    r.run_until(300);
    return 0;
}

