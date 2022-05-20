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
#include <omp.h>
#include <random>
#include <boost/lexical_cast.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>

#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QLineEdit>
#include <QDialog>

#include "SimulationEngine.h"
#include "ColorContainer.h"
#include "SimulationParameters.h"
#include "EngineControlContainer.h"
#include "EngineDataContainer.h"
#include "OrganismBlockParameters.h"
#include "WindowUI.h"

enum class CursorMode {
    Food_mode,
    Wall_mode,
    Kill_mode,
};

struct pix_pos {
    int start = 0;
    int stop = 0;
    pix_pos(int start, int stop): start(start), stop(stop) {}
};

template<typename T>
struct result_struct {
    bool is_valid;
    T result;
};

template <typename T> std::string to_str(const T& t, int float_precision = 2) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(float_precision) << t;
    return stream.str();
}

class DescisionMessageBox : public QDialog {
    Q_OBJECT

private:
    QVBoxLayout *vertical_layout;
    QHBoxLayout *horizontal_layout;
    QPushButton *accept_button;
    QPushButton *decline_button;
    QLabel *content_label;
public:
    DescisionMessageBox(const QString& title, const QString& content,
                        const QString& accept_text, const QString& decline_text, QWidget* parent=0)
                        : QDialog(parent) {
    vertical_layout = new QVBoxLayout();
    horizontal_layout = new QHBoxLayout();
    accept_button = new QPushButton(accept_text, this);
    decline_button = new QPushButton(decline_text, this);
    content_label = new QLabel(content, this);

    setLayout(vertical_layout);
    vertical_layout->addWidget(content_label, 2);
    vertical_layout->addLayout(horizontal_layout, 1);
    horizontal_layout->addWidget(accept_button);
    horizontal_layout->addWidget(decline_button);

    connect(accept_button, &QPushButton::pressed, this, &QDialog::accept);
    connect(decline_button, &QPushButton::pressed, this, &QDialog::reject);

    this->setWindowTitle(title);
    }
};

struct OrganismAvgBlockInformation {
    float size = 0;
    float _mouth_blocks    = 0;
    float _producer_blocks = 0;
    float _mover_blocks    = 0;
    float _killer_blocks   = 0;
    float _armor_blocks    = 0;
    float _eye_blocks      = 0;
    float brain_mutation_rate = 0;
    float anatomy_mutation_rate = 0;
};

//TODO expand About page.

class WindowCore: public QWidget {
        Q_OBJECT
private:
    // relative_x>1
    float scaling_coefficient = 1.2;
    float scaling_zoom = 1;

    bool right_mouse_button_pressed = false;
    bool left_mouse_button_pressed = false;

    float center_x;
    float center_y;

    CursorMode cursor_mode = CursorMode::Food_mode;

    int start_height = 100;

    void move_center(int delta_x, int delta_y);
    void reset_scale_view();

    int last_mouse_x = 0;
    int last_mouse_y = 0;

    SimulationEngine* engine;

    float window_interval = 0.;
    long delta_window_processing_time = 0;
    bool allow_waiting_overhead = false;

    Ui::MainWindow _ui;
    QTimer * timer;
    //void showEvent(QShowEvent *event);
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> end;

    static auto clock_now() {return std::chrono::high_resolution_clock::now();}
    std::chrono::time_point<std::chrono::high_resolution_clock> last_window_update;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_simulation_update;
    std::chrono::time_point<std::chrono::high_resolution_clock> fps_timer;
    int window_frames = 0;
    int simulation_frames = 0;

    ColorContainer color_container;

    SimulationParameters sp;

    EngineControlParameters cp;
    EngineDataContainer dc;
    OrganismBlockParameters op;

    std::thread engine_thread;
    std::mutex engine_mutex;

    std::vector<unsigned char> image_vector;
    QGraphicsScene scene;
    QGraphicsPixmapItem pixmap_item;

    bool pause_image_construction = false;
    bool full_simulation_grid_parsed = false;

    bool stop_console_output = true;
    bool synchronise_simulation_and_window = false;

    //TODO if == 1, then prints one pixel for one simulation block. If > 1, then loads texture texture (if exists).
    int cell_size = 1;
    int new_simulation_width = 600;
    int new_simulation_height = 600;
    // if true, will create simulation grid == simulation_graphicsView.viewport().size()
    bool fill_window = false;
    bool override_evolution_controls_slot = false;
    bool reset_with_chosen = false;

    int float_precision = 4;
    int auto_reset_num = 0;

    bool resize_simulation_grid_flag = false;

    Organism * base_organism;
    Organism * chosen_organism;

    std::mt19937 mt;

    bool menu_hidden = false;
    bool allow_menu_hidden_change = true;

    bool disable_warnings = false;

    void mainloop_tick();
    void window_tick();
    void set_simulation_interval(int max_simulation_fps);
    void set_window_interval(int max_window_fps);
    void update_fps_labels(int fps, int sps);
    void resize_image();
    void set_image_pixel(int x, int y, QColor & color);
    bool compare_pixel_color(int x, int y, QColor & color);

    void unpause_engine();

    void create_image();

    QColor inline &get_color(BlockTypes type);

    bool wait_for_engine_to_pause();
    void parse_simulation_grid(std::vector<int> & lin_width, std::vector<int> & lin_height);
    void parse_full_simulation_grid(bool parse);
    void clear_organisms();
    void make_walls();

    void set_simulation_num_threads(uint8_t num_threads);

    void set_cursor_mode(CursorMode mode);
    void set_simulation_mode(SimulationModes mode);

    void update_statistics_info(OrganismAvgBlockInformation info);

    //TODO sometimes the program SEGFAULTS when resizing, and I have no idea why.
    void resize_simulation_space();

    void inline calculate_linspace(std::vector<int> & lin_width, std::vector<int> & lin_height,
                            int start_x,  int end_x, int start_y, int end_y, int image_width, int image_height);
    void inline calculate_truncated_linspace(int image_width, int image_height,
                                      std::vector<int> & lin_width,
                                      std::vector<int> & lin_height,
                                      std::vector<int> & truncated_lin_width,
                                      std::vector<int> & truncated_lin_height);
    void inline
    image_for_loop(int image_width, int image_height, std::vector<int> &lin_width, std::vector<int> &lin_height,
                   std::vector<int> &truncated_lin_width, std::vector<int> &truncated_lin_height);

    void partial_clear_world();
    void reset_world();
    void clear_world();

    void initialize_gui_settings();

    OrganismAvgBlockInformation calculate_organisms_info();

    void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

    template<typename T>
    result_struct<T> try_convert_message_box_template(const std::string& message, QLineEdit *line_edit, T &fallback_value);
    int display_dialog_message(const std::string& message);
    static void display_message(const std::string& message);
private slots:
    void tb_pause_slot(bool paused);
    void tb_stoprender_slot(bool stopped_render);

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

    void le_num_threads_slot();
    void le_food_production_probability_slot();
    void le_lifespan_multiplier_slot();
    void le_look_range_slot();
    void le_auto_food_drop_rate_slot();
    void le_extra_reproduction_cost_slot();
    void le_global_mutation_rate_slot();
    void le_add_cell_slot();
    void le_change_cell_slot();
    void le_remove_cell_slot();
    void le_max_sps_slot();
    void le_max_fps_slot();
    void le_cell_size_slot();
    void le_simulation_width_slot();
    void le_simulation_height_slot();
    void le_min_reproducing_distance_slot();
    void le_max_reproducing_distance_slot();
    void le_max_organisms_slot();
    void le_float_number_precision_slot();
    void le_killer_damage_amount_slot();
    void le_produce_food_every_n_slot();
    void le_anatomy_mutation_rate_delimiter_slot();
    void le_brain_mutation_rate_delimiter_slot();

    void cb_reproduction_rotation_enabled_slot(bool state);
    void cb_on_touch_kill_slot(bool state);
    void cb_use_evolved_anatomy_mutation_rate_slot(bool state);
    void cb_movers_can_produce_food_slot(bool state);
    void cb_food_blocks_reproduction_slot(bool state);
    void cb_stop_console_output_slot(bool state);
    void cb_synchronise_simulation_and_window_slot(bool state);
    void cb_fill_window_slot(bool state);
    void cb_reset_on_total_extinction_slot(bool state);
    void cb_pause_on_total_extinction_slot(bool state);
    void cb_clear_walls_on_reset_slot(bool state);
    void cb_override_evolution_controls_slot(bool state);
    void cb_generate_random_walls_on_reset_slot(bool state);
    void cb_runtime_rotation_enabled_slot(bool state);
    void cb_fix_reproduction_distance_slot(bool state);
    void cb_use_evolved_brain_mutation_rate_slot(bool state);
    void cb_disable_warnings_slot(bool state);
    void cb_self_organism_blocks_block_sight_slot(bool state);

public:
    WindowCore(int simulation_width, int simulation_height,
               int window_fps, int simulation_fps, int simulation_num_threads,
               QWidget *parent);
};


#endif //LANGUAGES_UIWINDOW_H
