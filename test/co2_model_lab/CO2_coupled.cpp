//
// Model developed by Hoda Khalil in Cell-DEVS CD++
// Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
// Date 28/05/2020.
//

#include <fstream>

#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include "utils/grid_utils.hpp"
#include "coupled/grid_coupled.hpp"
#include "CO2_cell.hpp"

using TIME = float;

std::string delayer_id = "transport";


/*************** Loggers *******************/
static ofstream out_messages("simulation_results/CO2/" + delayer_id + "_output_messages.txt");
struct oss_sink_messages{
    static ostream& sink(){
        return out_messages;
    }
};
static ofstream out_state("simulation_results/CO2/" + delayer_id + "_state.txt");
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

    grid_scenario<co2, int> scenario = grid_scenario<co2,int>({38, 57}, co2(), false);
    
    //scenario.set_initial_state({24, 24}, sir(100, 0.7, 0.3, 0));

    scenario.set_von_neumann_neighborhood(1, 1);
    
    grid_coupled<TIME, co2> CO2_model = grid_coupled<TIME, co2>("CO2_model");
    CO2_model.add_lattice<co2_lab_cell>(scenario, delayer_id);
    CO2_model.couple_cells();

    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> t = std::make_shared<grid_coupled<TIME, co2, int>>(CO2_model);

    cadmium::dynamic::engine::runner<TIME, logger_top> r(t, {0});
    r.run_until(30000);
    cout << "Simulation finished" << endl;
    return 0;
}

