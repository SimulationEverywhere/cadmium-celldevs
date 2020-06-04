//
// Created by Román Cárdenas Rodríguez on 26/05/2020.
//

#ifndef CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
#define CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP

#include <cmath>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using namespace cadmium::celldevs;

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
struct sir {
    unsigned int population;
    unsigned int phase;
    float susceptible;
    float infected;
    float recovered;
    int offset;
    sir() : population(0), phase(0), offset(0), susceptible(1), infected(0), recovered(0) {}  // a default constructor is required
    sir(unsigned int pop, unsigned int phase, int o, float s, float i, float r) : population(pop), phase(phase), offset(o), susceptible(s), infected(i), recovered(r) {}
};
// Required for comparing states and detect any change
inline bool operator != (const sir &x, const sir &y) {
    return x.population != y.population || //x.phase != y.phase ||
    x.susceptible != y.susceptible || x.infected != y.infected || x.recovered != y.recovered;
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const sir& lhs, const sir& rhs){ return true; }
// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const sir &x) {
    os << "<" << x.population << "," << x.phase  << "," << x.offset << "," << x.susceptible << "," << x.infected << "," << x.recovered <<">";
    return os;
}

/************************************/
/*****COMPLEX VICINITY STRUCTURE*****/
/************************************/
struct mc {
    float connection;
    float movement;
    mc() : connection(0), movement(0) {}  // a default constructor is required
    mc(float c, float m) : connection(c), movement(m) {}
};


template <typename T>
class hoya_cell : public grid_cell<T, sir, mc> {
public:
    using grid_cell<T, sir, mc>::clock;
    using grid_cell<T, sir, mc>::state;
    using grid_cell<T, sir, mc>::map;
    using grid_cell<T, sir, mc>::neighbors;

    float virulence = 0.6;
    float recovery = 0.4;
    float disobedient = 0.0;
    int phases_durations[6] = {30, 14, 14, 14, 14, 100};

    hoya_cell() : grid_cell<T, sir, mc>() {}

    template <typename... Args>
    hoya_cell(cell_map<sir, mc> const &map_in, std::string const &delayer_id, float vir, float rec, Args&&... args) : grid_cell<T, sir, mc>(map_in, delayer_id, std::forward<Args>(args)...) {
        virulence = vir;
        recovery = rec;
    }

    template <typename... Args>
    hoya_cell(cell_map<sir, mc> const &map_in, std::string const &delayer_id, Args&&... args) :
            grid_cell<T, sir, mc>(map_in, delayer_id, std::forward<Args>(args)...) {
        check_valid_vicinity();
        check_valid_state();
    }

    // user must define this function. It returns the next cell state and its corresponding timeout
    sir local_computation() const override {
        sir res = state.current_state;
        float new_i = new_infections();
        float new_r = res.infected * recovery;
        res.recovered = std::round((res.recovered + new_r) * 100) / 100;
        res.infected = std::round((res.infected + new_i - new_r) * 100) / 100;
        res.susceptible = 1 - res.infected - res.recovered;
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
            assert(v.movement >= 0 && v.movement <= 1);
        }
    }

    void check_valid_state() const {
        sir init = state.current_state;
        assert(init.susceptible >= 0 && init.susceptible <= 1);
        assert(init.infected >= 0 && init.infected <= 1);
        assert(init.recovered >= 0 && init.recovered <= 1);
        assert(init.susceptible + init.infected + init.recovered == 1);
    }

    float new_infections() const {
        float aux = 0;
        for(auto neighbor: neighbors) {
            sir n = state.neighbors_state.at(neighbor);
            mc v = state.neighbors_vicinity.at(neighbor);
            float penalty = get_phase_penalty(n.phase);
            aux += (1 - disobedient) * penalty * n.infected * (float) n.population * v.movement * v.connection;
            aux += disobedient * n.infected * (float) n.population * v.movement * v.connection;
        }
        sir s = state.current_state;
        return std::min(s.susceptible, s.susceptible * virulence * aux / (float) s.population);
    }

    float get_phase_penalty(unsigned int phase) const {
        assert(0 <= phase && phase <= 5);
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
        }
    }

    unsigned int next_phase(unsigned int phase) const {
        int days_sum = 0;
        for(int phase_duration: phases_durations) {
            days_sum += phase_duration;
        }

        int aux = (int)(clock + state.current_state.offset) % (days_sum);
        int i = 0;

        while (aux >= phases_durations[i]) {
            aux -= phases_durations[i];
            i++;
        }

        return i;
    }
};

#endif //CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
