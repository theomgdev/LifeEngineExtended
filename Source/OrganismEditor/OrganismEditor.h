// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by spaceeye on 25.06.22.
//

#ifndef THELIFEENGINECPP_ORGANISMEDITOR_H
#define THELIFEENGINECPP_ORGANISMEDITOR_H

#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QWheelEvent>

#include "../Stuff/Linspace.h"
#include "../Organism/CPU/Organism.h"
#include "../Organism/CPU/Anatomy.h"
#include "../Organism/CPU/Brain.h"
#include "../Organism/CPU/Rotation.h"
#include "../GridBlocks/BaseGridBlock.h"
#include "EditorUI.h"
#include "../MainWindow/WindowUI.h"
#include "../Stuff/Vector2.h"
#include "../Containers/CPU/ColorContainer.h"
#include "../Stuff/textures.h"
#include "../Stuff/CursorMode.h"
#include "../Stuff/MiscFuncs.h"

struct EditBlock : BaseGridBlock {
    //For when cursor is hovering above block
    bool hovering = false;
    EditBlock()=default;
    explicit EditBlock(BlockTypes type, Rotation rotation = Rotation::UP) :
            BaseGridBlock(type, rotation){}
};

class OrganismEditor: public QWidget {
    Q_OBJECT
private:
    Ui::MainWindow * _parent_ui = nullptr;

    std::vector<std::vector<EditBlock>> edit_grid;
    std::vector<unsigned char> edit_image;
    Organism * editor_organism;
    CursorMode * c_mode = nullptr;

    Textures textures{};

    ColorContainer * color_container;

    BlockTypes chosen_block_type = BlockTypes::MouthBlock;

    float scaling_coefficient = 1.2;
    float scaling_zoom = 1;

    float center_x;
    float center_y;

    int editor_width = 15;
    int editor_height = 15;

    int new_editor_width = 15;
    int new_editor_height = 15;

    QGraphicsPixmapItem pixmap_item;
    QGraphicsScene scene;

    std::chrono::time_point<std::chrono::milliseconds> point;

    void closeEvent(QCloseEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
    void wheelEvent(QWheelEvent *event) override;

    void calculate_linspace(std::vector<int> &lin_width, std::vector<int> &lin_height, int start_x, int end_x, int start_y,
                            int end_y, int image_width, int image_height);

    void place_organism_on_a_grid();
    void clear_grid();

public:
    Ui::Editor _ui{};

    OrganismEditor()=default;

    void init(int width, int height, Ui::MainWindow *parent_ui, ColorContainer *color_container,
              SimulationParameters *sp, OrganismBlockParameters *bp, CursorMode * cursor_mode);

    void set_block(int x, int y, BaseGridBlock block);
    BaseGridBlock get_block(int x, int y);
    void set_organism(Organism organism);
    Organism get_organism();

    void resize_editing_grid(int width, int height);
    void resize_image();

    void move_center(int delta_x, int delta_y);
    void reset_scale_view();

    void create_image();

    void complex_for_loop(std::vector<int> &lin_width, std::vector<int> &lin_height);

    void set_image_pixel(int x, int y, color &color);

    color &get_texture_color(BlockTypes type, Rotation rotation, float relative_x_scale, float relative_y_scale);

private slots:
    void b_load_organism_slot();
    void b_reset_editing_view_slot();
    void b_resize_editing_grid_slot();
    void b_save_organism_slot();
    void le_anatomy_mutation_rate_slot();
    void le_grid_height_slot();
    void le_grid_width_slot();
    void le_move_range_slot();

    void rb_armor_slot();
    void rb_eye_slot();
    void rb_killer_slot();
    void rb_mouth_slot();
    void rb_mover_slot();
    void rb_producer_slot();
    void rb_edit_anatomy_slot();
    void rb_edit_brain_slot();
public slots:
    void rb_place_organism_slot();
    void rb_choose_organism_slot();
};

#endif //THELIFEENGINECPP_ORGANISMEDITOR_H
