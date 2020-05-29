//
// Model developed by Hoda Khalil in Cell-DEVS CD++
// Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
// Date 28/05/2020.
//

#ifndef CADMIUM_CELLDEVS_CO2_CELL_HPP
#define CADMIUM_CELLDEVS_CO2_CELL_HPP

#include <cmath>
#include <assert.h>
#include "cell/grid_cell.hpp"


/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
enum CELL_TYPE {AIR, CO2_SOURCE, IMPERMEABLE_STRUCTURE, DOOR, WINDOW, VENTILATION, WORKSTATION};
struct co2 {
    int counter;
    float concentration;
    CELL_TYPE type;
    co2() : counter(-1), concentration(500), type(AIR) {}  // a default constructor is required
    co2(int i_counter, float i_concentration, CELL_TYPE i_type) : counter(i_counter), concentration(i_concentration), type(i_type) {}
};
// Required for comparing states and detect any change
inline bool operator != (const co2 &x, const co2 &y) {
    return x.counter != y.counter || x.concentration != y.concentration || x.type != y.type;
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const co2& lhs, const co2& rhs){ return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const co2 &x) {
    os << "<" << x.counter << "," << x.concentration << "," << x.type <<">";
    return os;
}

// Each cell is 25cm x 25cm x 25cm = 15.626 Liters of air each
template <typename T>
class co2_lab_cell : public grid_cell<T, co2, int> {
public:
    using grid_cell<T, co2, int>::state;
    using grid_cell<T, co2, int>::map;

 
    co2_lab_cell() : grid_cell<T, co2, int>() {}

   
    template <typename... Args>
    co2_lab_cell(cell_map<co2, int> const &map_in, std::string const &delayer_id, Args&&... args) :
            grid_cell<T, co2, int>(map_in, delayer_id, std::forward<Args>(args)...) {
    }

    // user must define this function. It returns the next cell state and its corresponding timeout
    co2 local_computation() const override {
        co2 new_state = state.current_state;
        switch(state.current_state.type){
            case IMPERMEABLE_STRUCTURE: 
                new_state.concentration = -10;
                break;
            case DOOR:    
                new_state.concentration = 500;
                break;
            case WINDOW:
                new_state.concentration = 400;
                break;
            case VENTILATION:
                new_state.concentration = 300;
                break;
            case WORKSTATION:{
                float concentration = 0;
                int counter = 0;
                for(auto neighbors: state.neighbors_state) {
                    concentration += neighbors.second.concentration;
                    counter +=1;
                }
                new_state.concentration = concentration/counter;
                
                    
                if (state.current_state.counter <= 30) {
                    new_state.counter += 1;                   
                }
                if (state.current_state.counter == 30){
                    new_state.type = CO2_SOURCE; 
                }
                break;
            }
            case AIR:{
                if (new_state.concentration < 0){ 
                    assert(false && "concentration on an AIR cell cannot be negative");
                }
                float concentration = 0;
                int counter = 0;
                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.concentration>0){
                        concentration += neighbors.second.concentration;
                        counter +=1; 
                    }                    
                }
                if (counter != 0){
                    new_state.concentration = concentration/counter;
                }
                break;             
            }
            case CO2_SOURCE:{
                float concentration = 0;
                int counter = 0;
                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.concentration>0){
                        concentration += neighbors.second.concentration;
                        counter +=1; 
                    }                    
                }
                // CO2 sources have their concentration continually increased by 12.16 ppm every 5 seconds.
                new_state.concentration = (concentration/counter) + (121.6*2);
                new_state.counter += 1;
                if (state.current_state.counter >= 250) {
                    new_state.type = WORKSTATION; //It is strange: and air cell is now a work-station?
                }
                break;
            }
        }
      
        return new_state;

    }

    
    // It returns the delay to communicate cell's new state.
    T output_delay(co2 const &cell_state) const override {
        switch(cell_state.type){
            case CO2_SOURCE: return 5000;
            default: return 1000;
        }
    }

};

#endif //CADMIUM_CELLDEVS_CO2_CELL_HPP













