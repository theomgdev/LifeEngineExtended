// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by spaceeye on 27.03.2022.
//

#ifndef THELIFEENGINECPP_SIMULATIONPARAMETERS_H
#define THELIFEENGINECPP_SIMULATIONPARAMETERS_H

struct SimulationParameters {
    //evolution controls
    float food_production_probability = 0.01; //0.005;
    int   produce_food_every_n_life_ticks = 1;
    float lifespan_multiplier = 100;
    int   look_range = 50;
    int   auto_produce_food_every_n_ticks = 0;
    int   auto_produce_n_food = 0;
    float extra_reproduction_cost = 0;
    float extra_mover_reproductive_cost = 0;
    float global_anatomy_mutation_rate = 0.05;
    float global_brain_mutation_rate = 0.1;
    float killer_damage_amount = 1;

    int   min_reproducing_distance = 0;
    int   max_reproducing_distance = 3;

    float anatomy_mutations_rate_mutation_step = 0.01;
    float anatomy_min_possible_mutation_rate = 0.01;
    float anatomy_mutation_rate_delimiter = 0.5;

    float brain_mutation_rate_mutation_step = 0.01;
    float brain_min_possible_mutation_rate = 0.01;
    float brain_mutation_rate_delimiter = 0.5;

    int   min_move_range = 1;
    int   max_move_range = 5;
    float move_range_delimiter = 0.5;
    bool  set_fixed_move_range = false;
    int   min_organism_size = 1;

    //Probabilities of creating child with doing:
        int add_cell = 33;
        int change_cell = 33;
        int remove_cell = 33;

    int   perlin_octaves = 2;
    float perlin_persistence = 0.5;
    float perlin_upper_bound = 0.503;
    float perlin_lower_bound = 0.497;
    float perlin_x_modifier = 0.015; //0.005 for 1000 size
    float perlin_y_modifier = 0.015; //0.005

    bool reproduction_rotation_enabled = true;
    bool on_touch_kill = false;
    bool use_anatomy_evolved_mutation_rate = true;
    bool use_brain_evolved_mutation_rate = true;
    bool movers_can_produce_food = false;
    bool food_blocks_reproduction = false;
    //world controls
    bool reset_on_total_extinction = true;
    bool pause_on_total_extinction = false;
    bool clear_walls_on_reset = false;
    bool generate_random_walls_on_reset = false;
    bool reproduction_distance_fixed = false;
    bool runtime_rotation_enabled = true;
    bool organism_self_blocks_block_sight = false;
    bool failed_reproduction_eats_food = true;
    bool rotate_every_move_tick = false;
    bool multiply_food_production_prob = true;
    bool simplified_food_production = false;
    bool stop_when_one_food_generated = false;
    bool eat_then_produce = true;
    bool check_if_path_is_clear = true;
    bool food_blocks_movement = false;
    bool use_new_child_pos_calculator = true;
    bool no_random_decisions = false;
};

#endif //THELIFEENGINECPP_SIMULATIONPARAMETERS_H
