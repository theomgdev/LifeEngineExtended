// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by spaceeye on 20.03.2022.
//

#ifndef THELIFEENGINECPP_ANATOMY_H
#define THELIFEENGINECPP_ANATOMY_H

#include <utility>
#include <boost/unordered_map.hpp>
#include <random>

#include "Rotation.h"
#include "../../Containers/CPU/OrganismBlockParameters.h"
#include "../../Stuff/BlockTypes.hpp"
#include "../../GridBlocks/BaseGridBlock.h"
#include "../../PRNGS/lehmer64.h"
#include "../../Stuff/Vector2.h"

struct ProducerAdjacent {
    int producer = -1;
};

struct BaseSerializedContainer {
public:
    int relative_x;
    int relative_y;

    BaseSerializedContainer()=default;
    BaseSerializedContainer(int relative_x, int relative_y):
    relative_x(relative_x), relative_y(relative_y) {}

    inline Vector2<int> get_pos(Rotation rotation) {
        switch (rotation) {
            case Rotation::UP:    return Vector2<int>{relative_x, relative_y};
            case Rotation::LEFT:  return Vector2<int>{relative_y, -relative_x};
            case Rotation::DOWN:  return Vector2<int>{-relative_x, -relative_y};
            case Rotation::RIGHT: return Vector2<int>{-relative_y, relative_x};
            default: return Vector2<int>{relative_x, relative_y};
        }
    }
};

struct SerializedOrganismBlockContainer: BaseSerializedContainer {
    BlockTypes type;
    //for now only for eye blocks
    //local rotation of a block
    Rotation rotation = Rotation::UP;

    SerializedOrganismBlockContainer()=default;
    SerializedOrganismBlockContainer(BlockTypes type, Rotation rotation, int relative_x, int relative_y):
            BaseSerializedContainer(relative_x, relative_y), type(type), rotation(rotation) {}
    Rotation get_block_rotation_on_grid(Rotation organism_rotation) {
        uint_fast8_t new_int_rotation = static_cast<uint_fast8_t>(organism_rotation) + static_cast<uint_fast8_t>(rotation);
        return static_cast<Rotation>(new_int_rotation%4);
    }
};

struct SerializedAdjacentSpaceContainer: BaseSerializedContainer {
    SerializedAdjacentSpaceContainer()=default;
    SerializedAdjacentSpaceContainer(int relative_x, int relative_y):
    BaseSerializedContainer(relative_x, relative_y) {}
};

struct SerializedOrganismStructureContainer {
    std::vector<SerializedOrganismBlockContainer> organism_blocks;
    std::vector<std::vector<SerializedAdjacentSpaceContainer>> producing_space;
    std::vector<SerializedAdjacentSpaceContainer> eating_space;
    std::vector<SerializedAdjacentSpaceContainer> killing_space;

    int32_t mouth_blocks{};
    int32_t producer_blocks{};
    int32_t mover_blocks{};
    int32_t killer_blocks{};
    int32_t armor_blocks{};
    int32_t eye_blocks{};

    SerializedOrganismStructureContainer()=default;
    SerializedOrganismStructureContainer(
            std::vector<SerializedOrganismBlockContainer> organism_blocks,
            std::vector<std::vector<SerializedAdjacentSpaceContainer>> producing_space,
            std::vector<SerializedAdjacentSpaceContainer> eating_space,
            std::vector<SerializedAdjacentSpaceContainer> killing_space,

            int32_t mouth_blocks,
            int32_t producer_blocks,
            int32_t mover_blocks,
            int32_t killer_blocks,
            int32_t armor_blocks,
            int32_t eye_blocks):
            organism_blocks                (std::move(organism_blocks)),
            producing_space                (std::move(producing_space)),
            eating_space                   (std::move(eating_space)),
            killing_space                  (std::move(killing_space)),

            mouth_blocks(mouth_blocks),
            producer_blocks(producer_blocks),
            mover_blocks(mover_blocks),
            killer_blocks(killer_blocks),
            armor_blocks(armor_blocks),
            eye_blocks(eye_blocks)
            {}
};

class Anatomy {
private:
    static void set_single_adjacent(int x, int y, int x_offset, int y_offset,
                                    boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                    boost::unordered_map<int, boost::unordered_map<int, bool>> &single_adjacent_space,
                                    const BaseGridBlock &block);

    static void set_single_diagonal_adjacent(int x, int y, int x_offset, int y_offset,
                                             boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                             boost::unordered_map<int, boost::unordered_map<int, bool>>& single_adjacent_space,
                                             boost::unordered_map<int, boost::unordered_map<int, bool>>& single_diagonal_adjacent_space);

    static void create_single_adjacent_space(
            boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
            boost::unordered_map<int, boost::unordered_map<int, bool>> &single_adjacent_space);

    static void create_single_diagonal_adjacent_space(
            boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
            boost::unordered_map<int, boost::unordered_map<int, bool>>& single_adjacent_space,
            boost::unordered_map<int, boost::unordered_map<int, bool>>& single_diagonal_adjacent_space);

    static void create_producing_space(boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                       boost::unordered_map<int, boost::unordered_map<int, ProducerAdjacent>>& producing_space,
                                       boost::unordered_map<int, boost::unordered_map<int, bool>>& single_adjacent_space,
                                       std::vector<int> & num_producing_space,
                                       int32_t producer_blocks);

    static void create_eating_space(boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                    boost::unordered_map<int, boost::unordered_map<int, bool>>& eating_space,
                                    boost::unordered_map<int, boost::unordered_map<int, bool>>& single_adjacent_space,
                                    int32_t mouth_blocks);

    static void create_killing_space(boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                     boost::unordered_map<int, boost::unordered_map<int, bool>>& killing_space,
                                     boost::unordered_map<int, boost::unordered_map<int, bool>>& single_adjacent_space,
                                     int32_t killer_blocks);

    static void reset_organism_center(std::vector<SerializedOrganismBlockContainer> & _organism_blocks,
                                      boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                      int & x, int & y);

    static SerializedOrganismStructureContainer * serialize(
            const boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
            const boost::unordered_map<int, boost::unordered_map<int, ProducerAdjacent>>& producing_space,
            const boost::unordered_map<int, boost::unordered_map<int, bool>>& eating_space,
            const boost::unordered_map<int, boost::unordered_map<int, bool>>& killing_space,

            const std::vector<int> & num_producing_space,

            int32_t mouth_blocks,
            int32_t producer_blocks,
            int32_t mover_blocks,
            int32_t killer_blocks,
            int32_t armor_blocks,
            int32_t eye_blocks);

    static inline void serialize_producing_space(const boost::unordered_map<int, boost::unordered_map<int, ProducerAdjacent>> &producing_space,
                                                 const std::vector<int> &num_producing_space,
                                                 std::vector<std::vector<SerializedAdjacentSpaceContainer>> &_producing_space);

    static inline void serialize_organism_blocks(const boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
                                                 std::vector<SerializedOrganismBlockContainer> &_organism_blocks);

    static inline void serialize_eating_space(const boost::unordered_map<int, boost::unordered_map<int, bool>> &eating_space,
                                              std::vector<SerializedAdjacentSpaceContainer> &_eating_space);

    static inline void serialize_killing_space(const boost::unordered_map<int, boost::unordered_map<int, bool>> &killing_space,
                                               std::vector<SerializedAdjacentSpaceContainer> &_killing_space);

    template<typename T>
    static int get_map_size(boost::unordered_map<int, boost::unordered_map<int, T>> map);

    SerializedOrganismStructureContainer *
    add_block(BlockTypes type, int block_choice, Rotation rotation, int x_, int y_,
              boost::unordered_map<int, boost::unordered_map<int, BaseGridBlock>> &organism_blocks,
              std::vector<SerializedAdjacentSpaceContainer> &_single_adjacent_space,
              std::vector<SerializedAdjacentSpaceContainer> &_single_diagonal_adjacent_space,
              boost::unordered_map<int, boost::unordered_map<int, bool>> &single_adjacent_space);
    SerializedOrganismStructureContainer *change_block(BlockTypes type, int block_choice, lehmer64 *gen);
    SerializedOrganismStructureContainer *remove_block(int block_choice);

    int configure_block_counters(BlockTypes type, int x, int y);

public:
    std::vector<SerializedOrganismBlockContainer> _organism_blocks;
    std::vector<std::vector<SerializedAdjacentSpaceContainer>> _producing_space;
    std::vector<SerializedAdjacentSpaceContainer> _eating_space;
    std::vector<SerializedAdjacentSpaceContainer> _killing_space;

    int32_t _mouth_blocks    = 0;
    int32_t _producer_blocks = 0;
    int32_t _mover_blocks    = 0;
    int32_t _killer_blocks   = 0;
    int32_t _armor_blocks    = 0;
    int32_t _eye_blocks      = 0;

    explicit Anatomy(SerializedOrganismStructureContainer *structure);
    Anatomy(const Anatomy& anatomy);
    Anatomy()=default;
    Anatomy & operator=(Anatomy const & other_anatomy)=default;
    Anatomy & operator=(Anatomy & other_anatomy);

    SerializedOrganismStructureContainer * add_random_block(OrganismBlockParameters& block_parameters, lehmer64 &mt);
    SerializedOrganismStructureContainer * change_random_block(OrganismBlockParameters& block_parameters, lehmer64 &gen);
    SerializedOrganismStructureContainer * remove_random_block(lehmer64 &gen);

    void set_block(BlockTypes type, Rotation rotation, int x, int y);
    void set_many_blocks(std::vector<SerializedOrganismBlockContainer> & blocks);
};


#endif //THELIFEENGINECPP_ANATOMY_H
