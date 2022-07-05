//
// Created by spaceeye on 20.03.2022.
//

#ifndef THELIFEENGINECPP_BASEGRIDBLOCK_H
#define THELIFEENGINECPP_BASEGRIDBLOCK_H

#include <iostream>
#include "../Stuff/BlockTypes.hpp"
#include "../Organism/CPU/Rotation.h"

class Organism;

struct BaseGridBlock{
public:
    BlockTypes type = BlockTypes::EmptyBlock;
    Rotation rotation = Rotation::UP;

    BaseGridBlock()=default;
    BaseGridBlock(const BaseGridBlock & block): type(block.type),
                                                rotation(block.rotation) {}
    explicit BaseGridBlock(BlockTypes type, Rotation rotation = Rotation::UP): type(type),
                                                                               rotation(rotation) {}
    BaseGridBlock& operator=(const BaseGridBlock & block) = default;
};
#endif //THELIFEENGINECPP_BASEGRIDBLOCK_H
