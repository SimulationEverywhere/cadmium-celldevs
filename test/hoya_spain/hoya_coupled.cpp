//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#include <fstream>
#include <stdlib.h>

#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include <cadmium/celldevs/utils/grid_utils.hpp>
#include <cadmium/celldevs/coupled/grid_coupled.hpp>
#include "hoya_cell.hpp"

using namespace std;
using namespace cadmium;
using namespace cadmium::celldevs;

using TIME = float;

std::string delayer_id = "transport";
float vir = 0.6;
float rec = 0.4;

/*************** Loggers *******************/
static ofstream out_messages("../simulation_results/hoya_spain/" + delayer_id + "_output_messages.txt");
struct oss_sink_messages{
    static ostream& sink(){
        return out_messages;
    }
};
static ofstream out_state("../simulation_results/hoya_spain/" + delayer_id + "_state.txt");
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

int WIDTH = 50;
int HEIGHT = 50;
int INIT_MODE = 0;
int RAND_SEED = 0;

int main() {

    grid_scenario<sir, mc> scenario = grid_scenario<sir,mc>({WIDTH, HEIGHT}, sir(100, 1, 0, 1, 0, 0), true);

    if(INIT_MODE == 0) {  // classic
        scenario.set_initial_state({24, 24}, sir(100, 1, 0, 0.7, 0.3, 0));

    } else if(INIT_MODE == 1) {  // random offsets
        srand(RAND_SEED);
        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                int offset = rand() % 14;

                if (i == (int) (WIDTH / 2) && j == (int) (HEIGHT / 2)) {
                    scenario.set_initial_state({i, j}, sir(100, 1, offset, 0.7, 0.3, 0));
                } else {
                    scenario.set_initial_state({i, j}, sir(100, 1, offset, 1, 0, 0));
                }
            }
        }
    }

    scenario.set_von_neumann_neighborhood(1, mc(1, 0.5));
    scenario.add_neighborhood({{0, 0}}, mc(1, 1));

    grid_coupled<TIME, sir, mc> test = grid_coupled<TIME, sir, mc>("test");
    test.add_lattice<hoya_cell>(scenario, delayer_id, vir, rec);
    test.couple_cells();

    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> t = std::make_shared<grid_coupled<TIME, sir, mc>>(test);

    cadmium::dynamic::engine::runner<TIME, logger_top> r(t, {0});
    r.run_until(300);
    return 0;
}
