//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#ifndef CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
#define CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include <cadmium/celldevs/cell/grid_cell.hpp>
#include<iomanip>

using namespace std;
using namespace cadmium::celldevs;

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
struct sir {
    unsigned int population;
    unsigned int phase;
    int num_inf;
    int num_rec;
    float susceptible;
    vector<float> infected;
    vector<float> recovered;
    float deaths;

    sir() : population(0), phase(0), susceptible(1) {}  // a default constructor is required
    sir(unsigned int pop, unsigned int phase, int num_inf, int num_rec, float initial_infected) : population(pop), phase(phase), num_inf(num_inf), num_rec(num_rec) {
        infected.reserve(num_inf);
        recovered.reserve(num_rec);

        infected.push_back(initial_infected);
        susceptible = 1 - initial_infected;

        for(int i=1; i < num_inf; i++) {
            infected.push_back(0);
        }

        for(int i=0; i < num_rec; i++) {
            recovered.push_back(0);
        }

        deaths = 0;
    }
};
// Required for comparing states and detect any change
inline bool operator != (const sir &x, const sir &y) {
    bool neq = x.population != y.population || x.phase != y.phase || x.susceptible != y.susceptible || x.deaths != y.deaths;

    int i = 0;
    while(!neq and i < x.num_inf) {
        neq = x.infected[i] != y.infected[i];
        i += 1;
    }

    i = 0;
    while(!neq and i < x.num_rec) {
        neq = x.recovered[i] != y.recovered[i];
        i += 1;
    }

    return neq;
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const sir& lhs, const sir& rhs){ return true; }
// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const sir &x) {
    os << "<" << x.population << "," << x.phase << "," << x.num_inf << "," << x.num_rec << "," << x.susceptible;

    for(int i=0; i < x.num_inf; i++) {
        os << "," << x.infected[i];
    }

    for(int i=0; i < x.num_rec; i++) {
        os << "," << x.recovered[i];
    }

    os << "," << x.deaths;
    os << ">";
    return os;
}

/************************************/
/*****COMPLEX VICINITY STRUCTURE*****/
/************************************/
struct mc {
    float connection;
    mc() : connection(0) {}  // a default constructor is required
    mc(float c) : connection(c) {}
};


template <typename T>
class zhong_cell : public grid_cell<T, sir, mc> {
public:
    using grid_cell<T, sir, mc>::clock;
    using grid_cell<T, sir, mc>::state;
    using grid_cell<T, sir, mc>::map;
    using grid_cell<T, sir, mc>::neighbors;

    //float virulence = 0.15;
    //float recovery = 0.07;

    static const int NUM_PHASES = 1;
    static const int INF_NUM = 16;

    float disobedient = 0.0;
    int precDivider = 1000;
    float virulencyRates[NUM_PHASES][INF_NUM] = {{0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15}};
    float recoveryRates[NUM_PHASES][INF_NUM-1] = {{0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07}};
    float mortalityRates[NUM_PHASES][INF_NUM] = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
    float movilityRates[NUM_PHASES][INF_NUM] = {{0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6}};
    //int phases_durations[6] = {30, 14, 14, 14, 14, 100};

    zhong_cell() : grid_cell<T, sir, mc>() {}

    /*template <typename... Args>
    zhong_cell(cell_map<sir, mc> const &map_in, std::string const &delayer_id, Args&&... args) : grid_cell<T, sir, mc>(map_in, delayer_id, std::forward<Args>(args)...) {
        virulence = vir;
        recovery = rec;
    }*/

    template <typename... Args>
    zhong_cell(cell_map<sir, mc> const &map_in, std::string const &delayer_id, Args&&... args) :
            grid_cell<T, sir, mc>(map_in, delayer_id, std::forward<Args>(args)...) {
        check_valid_vicinity();
        check_valid_state();
    }

    // user must define this function. It returns the next cell state and its corresponding timeout
    sir local_computation() const override {
        sir res = state.current_state;

        /*std::cout << "---------------";
        std::cout << clock << "\n";
        std::cout << std::setprecision(10) << res.susceptible << "\n";
        std::cout << std::setprecision(10)<< res.infected << "\n";
        std::cout << std::setprecision(10)<< res.recovered << "\n";*/

        float new_i = std::round(get_phase_penalty(res.phase) * new_infections() * precDivider) / precDivider;
        float new_r = res.infected[res.num_inf-1];
        //  - (res.infected[res.num_inf-1] * mortalityRates[res.num_inf-1]))
        float new_s = 1;
        //float new_d = res.deaths;

        for (int i = 0; i < res.num_inf - 1; ++i) {
            //new_r += std::round((res.infected[i] - (res.infected[i] * mortalityRates[i])) * recoveryRates[i] * precDivider) / precDivider;
            new_r += std::round(res.infected[i] * recoveryRates[res.phase][i] * precDivider) / precDivider;
        }

        for (int i = res.num_inf - 1; i > 0; --i) {
            float curr_inf = res.infected[i-1];
            //float curr_deaths = res.infected[i-1] * mortalityRates[i-1];
            //curr_inf -= curr_deaths;
            curr_inf *= 1 - recoveryRates[res.phase][i-1];
            res.infected[i] = std::round(curr_inf * precDivider) / precDivider;
            new_s -= res.infected[i];
            //new_d += curr_deaths;
        }
        res.infected[0] = new_i;
        new_s -= new_i;

        for (int i = res.num_rec - 1; i > 0; --i) {
            res.recovered[i] = res.recovered[i-1];
            new_s -= res.recovered[i];
        }
        res.recovered[0] = new_r;
        new_s -= new_r;
        if(new_s > -0.001 && new_s < 0) new_s = 0;  // float precision issues

        //res.deaths = std::round(new_d * precDivider) / precDivider;
        //new_s -= res.deaths;
        /*std::cout << std::setprecision(10) << new_s << "\n";
        std::cout << std::setprecision(10) << res.infected << "\n";
        std::cout << std::setprecision(10) << res.recovered << "\n";*/
        assert(new_s >= 0);
        res.susceptible = new_s;

        /*std::cout << clock << " - " << map.location << "\n"
                << "New_inf: " << new_i << "\n"
                << "New_rec: " << new_r << "\n"
                << "Susceptible: " << res.susceptible << "\n";*/
        //assert(res.susceptible >= 0);
        res.phase = next_phase(res.phase);

        return res;
    }
    // It returns the delay to communicate cell's new state.
    T output_delay(sir const &cell_state) const override {
        return 1;
    }

    void check_valid_vicinity() {
        auto vicinities = state.neighbors_vicinity;
        for (auto neighbor: vicinities) {
            mc v = neighbor.second;
            assert(v.connection >= 0 && v.connection <= 1);
            //assert(v.movement >= 0 && v.movement <= 1);
        }
    }

    void check_valid_state() const {
        sir init = state.current_state;
        float sum = init.susceptible;
        assert(init.susceptible >= 0 && init.susceptible <= 1);
        for(int i=0; i < init.num_inf; i++) {
            assert(init.infected[i] >= 0 && init.infected[i] <= 1);
            sum += init.infected[i];
        }
        for(int i=0; i < init.num_rec; i++) {
            assert(init.recovered[i] >= 0 && init.recovered[i] <= 1);
            sum += init.recovered[i];
        }
        assert(sum == 1);
    }

    float new_infections() const {
        float inf = 0;
        sir cstate = state.current_state;

        // internal infected
        for (int i = 0; i < cstate.num_inf; ++i) {
            inf += virulencyRates[cstate.phase][i] * cstate.infected[i];
        }
        inf *= cstate.susceptible;

        // external infected
        for(auto neighbor: neighbors) {
            sir nstate = state.neighbors_state.at(neighbor);
            mc v = state.neighbors_vicinity.at(neighbor);

            for (int i = 0; i < nstate.num_inf; ++i) {
                inf += v.connection * movilityRates[cstate.phase][i] * cstate.susceptible *
                       ((float)nstate.population / (float)cstate.population) *
                        virulencyRates[cstate.phase][i] * nstate.infected[i];
            }
        }

        return std::min(cstate.susceptible, inf);
    }

    float get_phase_penalty(unsigned int phase) const {
        assert(0 <= phase && phase < NUM_PHASES);
        return 1;
        /*assert(0 <= phase && phase < NUM_PHASES);
        switch (phase) {
            case 0:
                return 1;
            case 1:
                return 0.2;
            case 2:
                return 0.4;
            case 3:
                return 0.6;
            case 4:
                return 0.8;
            case 5:
                return 1;
        }*/
    }

    unsigned int next_phase(unsigned int phase) const {
        return 0;
    }
};

#endif //CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
