//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#include <fstream>

#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include "utils/grid_utils.hpp"
#include "coupled/grid_coupled.hpp"
#include "grid_base.hpp"

using TIME = float;
std::string delayer_id = "transport";
/*************** Loggers *******************/
static ofstream out_messages("../simulation_results/grid_base/" + delayer_id + "_output_messages.txt");
struct oss_sink_messages{
    static ostream& sink(){
        return out_messages;
    }
};
static ofstream out_state("../simulation_results/grid_base/" + delayer_id + "_state.txt");
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
    std::vector<cell_position> n = grid_scenario<int, int>::von_neumann_neighborhood(2, 1);
    grid_scenario<int, int> scenario = grid_scenario<int,int>({3, 3}, -1, n, true);
    for (int i = 0; i < 3; i ++) {
        for (int j = 0; j < 3; j++) {
            scenario.set_initial_state({i, j}, i + j);
        }
    }

    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> test = std::make_shared<grid_coupled<grid_base, TIME, int, int>>(
            "test", delayer_id, scenario);

    cadmium::dynamic::engine::runner<TIME, logger_top> r(test, {0});
    r.run_until(300);
    return 0;
}

