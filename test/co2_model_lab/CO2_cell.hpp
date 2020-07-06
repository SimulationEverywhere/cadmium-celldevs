//
// Model developed by Hoda Khalil in Cell-DEVS CD++
// Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
// Date 28/05/2020.
//

#ifndef CADMIUM_CELLDEVS_CO2_CELL_HPP
#define CADMIUM_CELLDEVS_CO2_CELL_HPP

#include <cmath>
#include <assert.h>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using namespace cadmium::celldevs;

// Model Variables
int totalStudents = 5; //Total CO2_Source in the model
int studentGenerateCount = 5; //Student generate speed (n count/student)

std::list<std::pair<char,std::pair<int,int>>> nextActionList; //List include the next action for CO2_Source movement <action(+:Appear CO2_Source;-:Remove CO2_Source),<xPosition,yPosition>>
std::list<std::pair<std::pair<int,char>,std::pair<int,int>>> studentsList; //List include all CO2_Source that generated <<StudentID,state(+:Join;-:Leaving)>,<xPosition,yPosition>>

std::list<std::pair<int,std::pair<int,int>>> workstationsList; //List include the information of exist workstations <workStationID,<xPosition,yPosition>>
int workstationNumber = 0; //Total number of exist workstations

int counter = 0; //counter for studentGenerateCount
int studentGenerated = 0; //Record the number of students the already generated

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
enum CELL_TYPE {AIR=-100, CO2_SOURCE=-200, IMPERMEABLE_STRUCTURE=-300, DOOR=-400, WINDOW=-500, VENTILATION=-600, WORKSTATION=-700};
struct co2 {
    int counter;
    int concentration;
    CELL_TYPE type;
    co2() : counter(-1), concentration(500), type(AIR) {}  // a default constructor is required
    co2(int i_counter, int i_concentration, CELL_TYPE i_type) : counter(i_counter), concentration(i_concentration), type(i_type) {}
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
class co2_lab_cell : public grid_cell<T, co2> {
public:
    using grid_cell<T, co2, int>::state;
    using grid_cell<T, co2, int>::map;

     // CO2 sources have their concentration continually increased by default by 12.16 ppm every 5 seconds.
    float  concentration_increase = 121.6*2;
 
    co2_lab_cell() : grid_cell<T, co2, int>() {
    }

    template <typename... Args>
    co2_lab_cell(cell_map<co2, int> const &map_in, std::string const &delayer_id, Args&&... args) :
            grid_cell<T, co2>(map_in, delayer_id, std::forward<Args>(args)...) {

    }
     template <typename... Args>
    co2_lab_cell(cell_map<co2, int> const &map_in, std::string const &delayer_id, float con_inc, Args&&... args) :
            grid_cell<T, co2>(map_in, delayer_id, std::forward<Args>(args)...) {
                concentration_increase = con_inc;

    }

    co2 local_computation() const override {
        co2 new_state = state.current_state;

        std::pair<int,int> currentLocation;
        currentLocation.first = this->map.location[0];
        currentLocation.second = this->map.location[1];

        switch(state.current_state.type){
            case IMPERMEABLE_STRUCTURE: 
                new_state.concentration = 0;
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
                int concentration = 0;
                int num_neighbors = 0;                
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        concentration += neighbors.second.concentration;
                        num_neighbors +=1;
                    }
                }
                new_state.concentration = concentration/num_neighbors;

                break;
            }
            case AIR:{
                int concentration = 0;
                int num_neighbors = 0;                
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        concentration += neighbors.second.concentration;
                        num_neighbors +=1;
                    }
                }
                new_state.concentration = concentration/num_neighbors;

                if (counter == 0 && studentGenerated < totalStudents && studentGenerated < workstationNumber){
                    if(currentLocation.first == 32 && currentLocation.second == 8){ // CO2_Source Generation Location (32,8)

                        //Given student ID and record the location
                        std::pair<std::pair<int,char>,std::pair<int,int>> studentID;
                        studentID.first.first = studentGenerated;
                        studentID.first.second = '+';
                        studentID.second = currentLocation;
                        studentsList.push_back(studentID);

                        //Arrange the next action
                        std::pair<char,std::pair<int,int>> newAction;
                        newAction.first = '-';
                        newAction.second = currentLocation;
                        nextActionList.push_back(newAction);

                        studentGenerated++;
                        new_state.type = CO2_SOURCE;
                    }
                }

                //Appear CO2_Source at currentLocation
                if(nextActionList.front().first == '+' && currentLocation == nextActionList.front().second) {

                    //Arrange the next action
                    std::pair<char,std::pair<int,int>> newAction;
                    newAction.first = '-';
                    newAction.second = currentLocation;
                    nextActionList.push_back(newAction);

                    nextActionList.pop_front();
                    new_state.type = CO2_SOURCE;
                }
                break;             
            }
            case CO2_SOURCE:{
                int concentration = 0;
                int num_neighbors = 0;
                for(auto neighbors: state.neighbors_state) {
                  if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                      if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        concentration += neighbors.second.concentration;
                        num_neighbors +=1;
                    }               
                }
                
                new_state.concentration = (concentration/num_neighbors) + (concentration_increase);
                new_state.counter += 1;

                //Remove CO2_Source at currentLocation
                if(nextActionList.front().first == '-' && currentLocation == nextActionList.front().second) {
                    std::list<std::pair<std::pair<int, char>, std::pair<int, int>>>::iterator i;
                    for (i = studentsList.begin(); i != studentsList.end(); i++) {
                        if (i->second == currentLocation) { //Find the corresponding student
                            if(state.current_state.counter >= 250){
                                i->first.second = '-';
                            }
                            std::pair<int, int> nextLocation = setNextRoute(currentLocation, i->first);
                            std::pair<char,std::pair<int,int>> newAction;
                            i->second = nextLocation;

                            if(nextLocation == currentLocation){
                                //Arrangement next action
                                newAction.first = '-';
                                newAction.second = nextLocation;
                                nextActionList.push_back(newAction);
                            }else if(nextLocation.first == -1 && nextLocation.second == -1){
                                new_state.type = AIR;
                            }else {
                                //Arrangement next action
                                newAction.first = '+';
                                newAction.second = nextLocation;
                                nextActionList.push_back(newAction);

                                new_state.type = AIR;
                            }
                            counter = (counter + 1) % studentGenerateCount;
                            nextActionList.pop_front();
                        }
                    }
                }
                break;
            }
            default:{
                assert(false && "should never happen");
            }
        }
      
        return new_state;

    }

    /*
     * Calculate the position after the movement
     *
     * return: nextLocation
     */
    [[nodiscard]] std::pair<int,int> setNextRoute(std::pair<int,int> location, std::pair<int, char> studentIDNumber) const {
        std::pair<int, int> nextLocation;
        std::pair<int, int> destination;
        std::pair<int, int> locationChange;
        int destinationWSNum = studentIDNumber.first % workstationNumber;

        if(studentIDNumber.second == '-'){
            destination.first = 33;
            destination.second = 8;

            if(doorNearby(destination)){
                nextLocation.first = -1;
                nextLocation.second = -1;
                return nextLocation;
            }
        }else {
            //Get destination workstation location
            for (auto const i:workstationsList) {
                if (i.first == destinationWSNum) {
                    destination = i.second;
                }
            }

            if(WSNearby(destination)){
                nextLocation = location;
                return nextLocation;
            }
        }

        int x_diff = abs(location.first - destination.first);
        int y_diff = abs(location.second - destination.second);

        if(x_diff >= y_diff) { // x as priority direction
            if (destination.first < location.first) { //move left
                locationChange = navigation(location,'x','-');
            }else{//move right
                locationChange = navigation(location,'x','+');
            }
        }else{ // y as priority direction
            if (destination.second < location.second) { //move up
                locationChange = navigation(location,'y','-');
            }else{//move down
                locationChange = navigation(location,'y','+');
            }
        }

        nextLocation.first = location.first + locationChange.first;
        nextLocation.second = location.second + locationChange.second;

        return nextLocation;
    }

    /*
     * Check if the destination workstation is nearby
     */
    [[nodiscard]] bool WSNearby(std::pair<int, int> destination) const {
        for(auto const neighbors: state.neighbors_state) {
            if(neighbors.second.type == WORKSTATION) {
                if (neighbors.first[0] == destination.first) {
                    if (neighbors.first[1] == destination.second) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /*
     * Check if the destination DOOR is nearby
     */
    [[nodiscard]] bool doorNearby(std::pair<int, int> destination) const{
        for(auto const neighbors: state.neighbors_state) {
            if(neighbors.second.type == DOOR) {
                if (neighbors.first[0] == destination.first) {
                    if (neighbors.first[1] == destination.second) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /*
     * Using movement rules to do the navigation
     *
     * return: the location change
     */
    [[nodiscard]] std::pair<int,int> navigation(std::pair<int,int> location, char priority, char direction) const {
        std::pair<int,int> locationChange;
        locationChange.first = 0;
        locationChange.second = 0;

        int change;
        if(direction == '-'){
            change = -1;
        } else{
            change = 1;
        }

        if(priority == 'x'){
            if(moveCheck(location.first + change, location.second)){
                locationChange.first = change;
            }else if(moveCheck(location.first, location.second + change)){
                locationChange.second = change;
            }else if(moveCheck(location.first, location.second - change)){
                locationChange.second = 0 - change;
            }else if(moveCheck(location.first - change, location.second)){
                locationChange.first = change;
            }
        }else{
            if(moveCheck(location.first, location.second + change)){
                locationChange.second = change;
            }else if(moveCheck(location.first + change, location.second)){
                locationChange.first = change;
            }else if(moveCheck(location.first - change, location.second)){
                locationChange.first = 0 - change;
            }else if(moveCheck(location.first, location.second - change)){
                locationChange.second = change;
            }
        }
        return locationChange;
    }

    /*
     * Check if the next location is occupied
     */
    [[nodiscard]] bool moveCheck(int xNext,int yNext) const {
        bool moveCheck = false;
        for(auto const neighbors: state.neighbors_state) {
            if(neighbors.first[0] == xNext){
                if(neighbors.first[1] == yNext){
                    if(neighbors.second.type == AIR) {
                        moveCheck = true;
                    }
                }
            }
        }

        for(auto const student: studentsList){
            if(student.second.first == xNext){
                if(student.second.second == yNext){
                    moveCheck = false;
                }
            }
        }

        return moveCheck;
    }

    // It returns the delay to communicate cell's new state.
    T output_delay(co2 const &cell_state) const override {
        switch(cell_state.type){
            case CO2_SOURCE: return 5;
            default: return 1;
        }
    }

};

#endif //CADMIUM_CELLDEVS_CO2_CELL_HPP













