// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by spaceeye on 16.03.2022.
//

#ifndef LANGUAGES_UIWINDOW_H
#define LANGUAGES_UIWINDOW_H

#include <iostream>
#include <chrono>
#include <cmath>
#include <cfloat>
#include <sstream>
#include <iomanip>
#include <thread>
#include <random>
#include <fstream>
#ifndef __WIN32
#include <filesystem>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>
#include <boost/nondet_random.hpp>
#include <boost/random.hpp>
#include <boost/property_tree/ptree.hpp>
#include "../CustomJsonParser/json_parser.hpp"


#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QLineEdit>
#include <QDialog>
#include <QFont>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QToolBar>
#include <QWheelEvent>
//#include <QtCharts>


#include "../SimulationEngine/SimulationEngine.h"
#include "../Containers/CPU/ColorContainer.h"
#include "../Containers/CPU/SimulationParameters.h"
#include "../Containers/CPU/EngineControlContainer.h"
#include "../Containers/CPU/EngineDataContainer.h"
#include "../Containers/CPU/OrganismBlockParameters.h"
#include "../OrganismEditor/OrganismEditor.h"
#include "../PRNGS/lehmer64.h"
#include "../Stuff/textures.h"
#include "../Stuff/MiscFuncs.h"
#include "../Stuff/CursorMode.h"
#include "../Stuff/Vector2.h"

#include "WindowUI.h"
#include "../Statistics/StatisticsCore.h"


#if __CUDA_USED__
#include "../Stuff/cuda_image_creator.cuh"
#include "../Stuff/get_device_count.cuh"
#include "../Stuff/OrganismData.h"

#endif

#if defined(__WIN32)
#include <windows.h>
#endif

struct OrganismInfoHolder {
    double size = 0;
    double _organism_lifetime = 0;
    double _organism_age    = 0;
    double _mouth_blocks    = 0;
    double _producer_blocks = 0;
    double _mover_blocks    = 0;
    double _killer_blocks   = 0;
    double _armor_blocks    = 0;
    double _eye_blocks      = 0;
    double brain_mutation_rate = 0;
    double anatomy_mutation_rate = 0;
    int total = 0;
};

struct OrganismAvgBlockInformation {

    uint64_t total_size_organism_blocks = 0;
    uint64_t total_size_producing_space = 0;
    uint64_t total_size_eating_space    = 0;
    uint64_t total_size_single_adjacent_space = 0;
    uint64_t total_size_single_diagonal_adjacent_space = 0;
    uint64_t total_size = 0;

    OrganismInfoHolder total_avg{};
    OrganismInfoHolder station_avg{};
    OrganismInfoHolder moving_avg{};

    double move_range = 0;
    int moving_organisms = 0;
    int organisms_with_eyes = 0;

    double total_total_mutation_rate = 0;
};

class MainWindow: public QWidget {
        Q_OBJECT
private:
    CursorMode cursor_mode = CursorMode::ModifyFood;

#if __CUDA_USED__
    CUDAImageCreator cuda_creator{};
#endif

    std::thread engine_thread;
    Textures textures{};

    std::vector<unsigned char> image_vector;
    QGraphicsScene scene;
    QGraphicsPixmapItem pixmap_item;

    ColorContainer cc;
    SimulationParameters sp;
    EngineControlParameters ecp;
    EngineDataContainer edc;
    OrganismBlockParameters bp;

    Ui::MainWindow _ui{};
    QTimer * timer;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_window_update;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_simulation_update;
    std::chrono::time_point<std::chrono::high_resolution_clock> fps_timer;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_event_execution;

    SimulationEngine* engine;
    OrganismEditor ee;
    StatisticsCore s;

    // coefficient of a zoom
    float scaling_coefficient = 1.2;
    // actual zoom
    float scaling_zoom = 1;
    float center_x = 0;
    float center_y = 0;
    float window_interval = 0.;
    float keyboard_movement_amount = 0.5;
    float SHIFT_keyboard_movement_multiplier = 2;

    bool wheel_mouse_button_pressed = false;
    bool right_mouse_button_pressed = false;
    bool left_mouse_button_pressed = false;
    bool change_main_simulation_grid = false;
    bool change_editing_grid = false;
    bool use_cuda = true;
    bool synchronise_info_update_with_window_update = false;
    bool wait_for_engine_to_stop_to_render = false;
    // do not use textures (now for eyes only)
    bool simplified_rendering = false;
    //TODO redundant?
    bool resize_simulation_grid_flag = false;
    bool menu_hidden = false;
    //is needed to prevent multiple switches when pressing button
    bool allow_menu_hidden_change = true;
    //TODO remove?
    bool disable_warnings = false;
    // if true, will create simulation grid == simulation_graphicsView.viewport().size()
    bool fill_window = false;
    bool reset_with_chosen = false;
    //stops copying from main simulation grid to secondary grid from which image is constructed
    bool pause_grid_parsing = false;
    bool synchronise_simulation_and_window = false;
    bool really_stop_render = false;

    bool W_pressed = false;
    bool A_pressed = false;
    bool S_pressed = false;
    bool D_pressed = false;
    bool SHIFT_pressed = false;

    int last_mouse_x_pos = 0;
    int last_mouse_y_pos = 0;
    int window_frames = 0;
    // if fill_window, then size of a cell on a screen should be around this value
    int starting_cell_size_on_resize = 1;
    uint32_t new_simulation_width = 200;
    uint32_t new_simulation_height = 200;
    // visual only. Controls precision of floats in labels
    int float_precision = 4;
    int brush_size = 2;
    int update_info_every_n_milliseconds = 100;
    //Will give a warning if num is higher than this.
    int max_loaded_num_organisms = 1'000'000;
    int max_loaded_world_side = 10'000;

    static auto clock_now() {return std::chrono::high_resolution_clock::now();}

    // moves the center of viewpoint
    void move_center(int delta_x, int delta_y);
    void reset_scale_view();

    void write_json_data(const std::string &path);
    void json_write_controls(boost::property_tree::ptree &controls, boost::property_tree::ptree &killable_neighbors,
                             boost::property_tree::ptree &edible_neighbors,
                             boost::property_tree::ptree &growableNeighbors,
                             boost::property_tree::ptree &cell, boost::property_tree::ptree &value) const;

    void json_write_fossil_record(boost::property_tree::ptree &fossil_record) const;

    void json_write_organisms(boost::property_tree::ptree &organisms, boost::property_tree::ptree &cell,
                              boost::property_tree::ptree &anatomy, boost::property_tree::ptree &cells,
                              boost::property_tree::ptree &j_organism, boost::property_tree::ptree &brain);

    void json_write_grid(boost::property_tree::ptree &grid, boost::property_tree::ptree &cell,
                         boost::property_tree::ptree &food, boost::property_tree::ptree &walls);

    void json_read_organism_data(boost::property_tree::ptree &root);

    void json_read_simulation_parameters(const boost::property_tree::ptree &root);

    void json_read_grid_data(boost::property_tree::ptree &root);
    void read_json_data(const std::string &path);

    //https://stackoverflow.com/questions/28492517/write-and-load-vector-of-structs-in-a-binary-file-c
    void write_data(std::ofstream& os);
    void write_version(std::ofstream& os);
    void write_simulation_parameters(std::ofstream& os);
    void write_organisms_block_parameters(std::ofstream& os);
    void write_data_container_data(std::ofstream& os);
    //    void write_color_container(); TODO: ?
    void write_simulation_grid(std::ofstream& os);
    void write_organisms(std::ofstream& os);
    void write_organism_data(std::ofstream& os, Organism * organism);
    void write_organism_brain(std::ofstream& os, Brain * brain);
    void write_organism_anatomy(std::ofstream& os, Anatomy * anatomy);

    void recover_state(const SimulationParameters &recovery_sp,
                       const OrganismBlockParameters &recovery_bp,
                       uint32_t recovery_simulation_width,
                       uint32_t recovery_simulation_height);

    void read_data(std::ifstream& is);
    bool read_version(std::ifstream& is);
    void read_simulation_parameters(std::ifstream& is);
    void read_organisms_block_parameters(std::ifstream& is);
    bool read_data_container_data(std::ifstream& is);
    //    void read_color_container(); TODO: ?
    void read_simulation_grid(std::ifstream& is);
    bool read_organisms(std::ifstream& is);
    void read_organism_data(std::ifstream& is, OrganismData & data);
    void read_organism_brain(std::ifstream& is, Brain * brain);
    void read_organism_anatomy(std::ifstream& is, Anatomy * anatomy);
    void update_table_values();

    bool cuda_is_available();

    void mainloop_tick();
    void window_tick();
    void set_simulation_interval(int max_simulation_fps);
    void set_window_interval(int max_window_fps);
    void update_fps_labels(int fps, int sps);
    void resize_image();
    void inline set_image_pixel(int x, int y, const color &color);

    // for fill_view
    void calculate_new_simulation_size();
    Vector2<int> calculate_cursor_pos_on_grid(int x, int y);

    void pause_engine();
    void unpause_engine();

    void create_image();

    color & get_color_simplified(BlockTypes type);
    color & get_texture_color(BlockTypes type, Rotation rotation, float relative_x_scale, float relative_y_scale);

    bool wait_for_engine_to_pause();
    bool wait_for_engine_to_pause_processing_user_actions();
    bool wait_for_engine_to_pause_force();

    // parses actual simulation grid to grid from which image is created
    void parse_simulation_grid(const std::vector<int> &lin_width, const std::vector<int> &lin_height);
    void parse_full_simulation_grid(bool parse);

    void change_main_grid_left_click();
    void change_main_grid_right_click();

    void change_editing_grid_left_click();
    void change_editing_grid_right_click();

    void set_simulation_num_threads(uint8_t num_threads);

    void set_cursor_mode(CursorMode mode);
    void set_simulation_mode(SimulationModes mode);

    void update_statistics_info(const OrganismAvgBlockInformation &info);

    void resize_simulation_grid();

    // creates simple colored cells
    void simplified_image_creation(int image_width, int image_height,
                                   const std::vector<int> &lin_width,
                                   const std::vector<int> &lin_height);

    // creates cells with textures (right now only eye)
    void complex_image_creation(const std::vector<int> &lin_width, const std::vector<int> &lin_height);

    static void calculate_linspace(std::vector<int> & lin_width, std::vector<int> & lin_height,
                            int start_x,  int end_x, int start_y, int end_y, int image_width, int image_height);
    static void calculate_truncated_linspace(int image_width, int image_height,
                                             const std::vector<int> &lin_width,
                                             const std::vector<int> &lin_height,
                                             std::vector<int> & truncated_lin_width,
                                             std::vector<int> & truncated_lin_height);

    void clear_world();

    void update_simulation_size_label();

    // fills ui line edits with values from code so that I don't need to manually change ui file when changing some values in code.
    void initialize_gui();

    // converts num bytes to string of shortened number with postfix (like 14 KiB)
    static std::string convert_num_bytes(uint64_t num_bytes);

    OrganismAvgBlockInformation parse_organisms_info();

    void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

private slots:
    void tb_pause_slot(bool state);
    void tb_stoprender_slot(bool state);
    void tb_open_statistics_slot(bool state);
    void tb_open_organism_editor_slot(bool state);

    void b_clear_slot();
    void b_reset_slot();
    void b_pass_one_tick_slot();
    void b_reset_view_slot();
    void b_resize_and_reset_slot();
    void b_generate_random_walls_slot();
    void b_clear_all_walls_slot();
    void b_save_world_slot();
    void b_load_world_slot();
    void b_kill_all_organisms_slot();

    void rb_food_slot();
    void rb_wall_slot();
    void rb_kill_slot();
    void rb_single_thread_slot();
    void rb_multi_thread_slot();
    void rb_cuda_slot();
    void rb_partial_multi_thread_slot();

    //Evolution Controls
    void le_food_production_probability_slot();
    void le_lifespan_multiplier_slot();
    void le_look_range_slot();
    void le_auto_food_drop_rate_slot();
    void le_extra_reproduction_cost_slot();
    void le_global_anatomy_mutation_rate_slot();
    void le_global_brain_mutation_rate_slot();
    void le_add_cell_slot();
    void le_change_cell_slot();
    void le_remove_cell_slot();
    void le_min_reproducing_distance_slot();
    void le_max_reproducing_distance_slot();
    void le_killer_damage_amount_slot();
    void le_produce_food_every_n_slot();
    void le_anatomy_mutation_rate_delimiter_slot();
    void le_brain_mutation_rate_delimiter_slot();
    void le_max_move_range_slot();
    void le_min_move_range_slot();
    void le_move_range_delimiter_slot();
    void le_auto_produce_food_every_n_tick_slot();
    void le_anatomy_min_possible_mutation_rate_slot();
    void le_brain_min_possible_mutation_rate_slot();
    void le_extra_mover_reproduction_cost_slot();
    void le_anatomy_mutation_rate_step_slot();
    void le_brain_mutation_rate_step_slot();
    void le_keyboard_movement_amount_slot();
    //Settings
    void le_num_threads_slot();
    void le_update_info_every_n_milliseconds_slot();
    void le_menu_height_slot();
    void le_perlin_octaves_slot();
    void le_perlin_persistence_slot();
    void le_perlin_upper_bound_slot();
    void le_perlin_lower_bound_slot();
    void le_perlin_x_modifier_slot();
    void le_perlin_y_modifier_slot();
    void le_font_size_slot();
    void le_float_number_precision_slot();
    void le_scaling_coefficient_slot();
    //Other
    void le_max_sps_slot();
    void le_max_fps_slot();
    void le_cell_size_slot();
    void le_simulation_width_slot();
    void le_simulation_height_slot();
    void le_max_organisms_slot();
    void le_brush_size_slot();

    //Evolution Controls
    void cb_reproduction_rotation_enabled_slot(bool state);
    void cb_on_touch_kill_slot(bool state);
    void cb_use_evolved_anatomy_mutation_rate_slot(bool state);
    void cb_movers_can_produce_food_slot(bool state);
    void cb_food_blocks_reproduction_slot(bool state);
    void cb_reset_on_total_extinction_slot(bool state);
    void cb_pause_on_total_extinction_slot(bool state);
    void cb_runtime_rotation_enabled_slot(bool state);
    void cb_fix_reproduction_distance_slot(bool state);
    void cb_use_evolved_brain_mutation_rate_slot(bool state);
    void cb_self_organism_blocks_block_sight_slot(bool state);
    void cb_set_fixed_move_range_slot(bool state);
    void cb_failed_reproduction_eats_food_slot(bool state);
    void cb_rotate_every_move_tick_slot(bool state);
    void cb_multiply_food_production_prob_slot(bool state);
    void cb_simplified_food_production_slot(bool state);
    void cb_stop_when_one_food_generated(bool state);
    void cb_eat_then_produce_slot(bool state);
    void cb_food_blocks_movement_slot(bool state);
    void cb_use_new_child_pos_calculator_slot(bool state);
    void cb_check_if_path_is_clear_slot(bool state);
    //Other
    void cb_synchronise_simulation_and_window_slot(bool state);
    void cb_fill_window_slot(bool state);
    void cb_clear_walls_on_reset_slot(bool state);
    void cb_generate_random_walls_on_reset_slot(bool state);
    //Settings
    void cb_disable_warnings_slot(bool state);
    void cb_wait_for_engine_to_stop_slot(bool state);
    void cb_synchronise_info_with_window_slot(bool state);
    void cb_use_nvidia_for_image_generation_slot(bool state);
    void cb_simplified_rendering_slot(bool state);
    void cb_really_stop_render_slot(bool state);
    //Windows
    void cb_statistics_always_on_top_slot(bool state);
    void cb_editor_always_on_top_slot(bool state);

    //Evolution Controls
    void table_cell_changed_slot(int row, int col);
public:
    MainWindow(QWidget *parent);

    void process_keyboard_events();
};


#endif //LANGUAGES_UIWINDOW_H
