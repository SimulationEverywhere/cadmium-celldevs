//
// Model developed by Hoda Khalil in Cell-DEVS CD++
// Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
// Date 28/05/2020.
//

#include <fstream>
#include <iostream>
#include <string>

#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include <cadmium/celldevs/utils/grid_utils.hpp>
#include <cadmium/celldevs/coupled/grid_coupled.hpp>
#include "CO2_cell.hpp"

 #include <unistd.h>

using namespace std;
using namespace cadmium;
using namespace cadmium::celldevs;

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

    grid_scenario<co2,int> scenario = grid_scenario<co2, int>({38, 57}, co2(), false);
    
    ifstream myfile;
    string line;    
    myfile.open("test/co2_model_lab/computer_lab.txt");
    if (!myfile.is_open()){
         assert(false && "error opening input file");
    }
    while ( getline(myfile,line) ){
        string cell;
        int cell_x;
        int cell_y;        
        co2 input;
        int type;

        std::string delimiter = "=";
        size_t pos = line.find(delimiter);
        cell = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        delimiter = ",";
        pos = cell.find(delimiter);
        cell_x = std::stoi(cell.substr(0, pos));
        
        cell.erase(0, pos + delimiter.length());
        cell_y = std::stoi(cell);
     
        pos = line.find(delimiter);
        input.concentration = std::stoi(line.substr(0, pos));
   
        line.erase(0, pos + delimiter.length());
        pos = line.find(delimiter);
        type = std::stoi(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
        input.counter = std::stoi(line);
   
        switch(type){
            case -300: 
                input.type = IMPERMEABLE_STRUCTURE;
                break;
            case -400:    
               input.type = DOOR;
                break;
            case -500:
                input.type = WINDOW;
                break;
            case -600:
               input.type = VENTILATION;
                break;
            case -700:
               input.type = WORKSTATION;
                break;
            case -100:
                input.type = AIR;
                break;             
            case -200:
                input.type = CO2_SOURCE;
                break;
            default:
                assert(false && "invalid input");
            }
                
 
        scenario.set_initial_state({cell_x, cell_y}, co2(input.counter, input.concentration, input.type));
    }
    myfile.close();
    scenario.set_von_neumann_neighborhood(1, 1);
    
    grid_coupled<TIME, co2> CO2_model = grid_coupled<TIME, co2>("CO2_model");
    CO2_model.add_lattice<co2_lab_cell>(scenario, delayer_id);
    CO2_model.couple_cells();

    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> t = std::make_shared<grid_coupled<TIME, co2, int>>(CO2_model);

    cadmium::dynamic::engine::runner<TIME, logger_top> r(t, {0});
    r.run_until(400);
    cout << "Simulation finished" << endl;
    return 0;
}

