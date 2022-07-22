// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by spaceeye on 16.03.2022.
//

#ifndef LANGUAGES_LIFEENGINE_H
#define LANGUAGES_LIFEENGINE_H

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <algorithm>

#include <boost/nondet_random.hpp>
#include <boost/random.hpp>

#include "../GridBlocks/BaseGridBlock.h"
#include "../Organism/CPU/Organism.h"
#include "../Stuff/BlockTypes.hpp"
#include "../Stuff/Vector2.h"
#include "../Containers/CPU/EngineControlContainer.h"
#include "../Containers/CPU/EngineDataContainer.h"
#include "../Containers/CPU/OrganismBlockParameters.h"
#include "../Stuff/Linspace.h"
#include "../Stuff/PerlinNoise.hpp"
#include "../PRNGS/lehmer64.h"
#include "../OrganismEditor/OrganismEditor.h"
#include "SimulationEngineModes/SimulationEnginePartialMultiThread.h"
#include "SimulationEngineModes/SimulationEngineSingleThread.h"

//TODO move simulation grid translation to here
class SimulationEngine {
    EngineControlParameters& cp;
    EngineDataContainer& dc;
    OrganismBlockParameters& op;
    SimulationParameters& sp;

    uint32_t auto_food_drop_index = 0;
    std::vector<Vector2<int>> auto_food_drop_coordinates_shuffled{};

    siv::PerlinNoise perlin;

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> fps_timer;

    void process_user_action_pool();

    void simulation_tick();
    void partial_multi_threaded_tick();

    void change_mode();
    static bool check_if_out_of_bounds(EngineDataContainer *dc, int x, int y);

    void random_food_drop();

    //lehmer is like 2 times faster than mt19937
    lehmer64 gen;

    void try_kill_organism(int x, int y, std::vector<Organism*> & temp);
    void try_remove_food(int x, int y);
public:
    SimulationEngine(EngineDataContainer &engine_data_container, EngineControlParameters &engine_control_parameters,
                     OrganismBlockParameters &organism_block_parameters, SimulationParameters &simulation_parameters);
    void threaded_mainloop();

    void make_random_walls();

    void set_wall(std::vector<Organism *> &temp, const Action &action);
    void clear_walls();

    //TODO make getters and setters for it.
    void reinit_organisms();

    void reset_world();
    void partial_clear_world();
    void clear_organisms();
    void make_walls();

    void init_auto_food_drop(int width, int height);
};

#endif //LANGUAGES_LIFEENGINE_H
