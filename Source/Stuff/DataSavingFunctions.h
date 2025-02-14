//
// Created by spaceeye on 31.07.22.
//

#ifndef LIFEENGINEEXTENDED_DATASAVINGFUNCTIONS_H
#define LIFEENGINEEXTENDED_DATASAVINGFUNCTIONS_H

#include <cstdint>
#include <ostream>
#include <fstream>

#include "../SimulationEngine/SimulationEngineModes/SimulationEngineSingleThread.h"
#include "BlockTypes.hpp"
#include "../Containers/CPU/SimulationParameters.h"
#include "../Containers/CPU/EngineDataContainer.h"
#include "../Containers/CPU/OrganismBlockParameters.h"
#include "../Organism/CPU/Organism.h"
#include "../Organism/CPU/Anatomy.h"
#include "../Organism/CPU/Brain.h"
#include "../SimulationEngine/OrganismsController.h"

#include "../Stuff/rapidjson/document.h"
#include "../Stuff/rapidjson/writer.h"
#include "../Stuff/rapidjson/stringbuffer.h"

namespace DataSavingFunctions {
    struct WorldBlocks {
        uint32_t x;
        uint32_t y;
        BlockTypes type;
        WorldBlocks()=default;
        WorldBlocks(uint32_t x, uint32_t y, BlockTypes type): x(x), y(y), type(type) {}
    };

    void write_version(std::ofstream &os);
    void write_simulation_parameters(std::ofstream & os, SimulationParameters &sp);
    void write_organisms_block_parameters(std::ofstream & os, OrganismBlockParameters &bp);
    void write_data_container_data(std::ofstream & os, EngineDataContainer &edc);
    void write_simulation_grid(std::ofstream & os, EngineDataContainer &edc);
    void write_organism(std::ofstream& os, Organism * organism);
    void write_organisms(std::ofstream & os, EngineDataContainer &edc);
    void write_organism_data(std::ofstream & os, Organism * organism);
    void write_organism_brain(std::ofstream & os, Brain * brain);
    void write_organism_anatomy(std::ofstream & os, Anatomy * anatomy);

    bool read_version(std::ifstream &is);
    void read_simulation_parameters(std::ifstream& is, SimulationParameters &sp);
    void read_organisms_block_parameters(std::ifstream& is, OrganismBlockParameters &bp);
    void read_data_container_data(std::ifstream& is, EngineDataContainer &edc, uint32_t &sim_width, uint32_t &sim_height);
    void read_simulation_grid(std::ifstream& is, EngineDataContainer &edc);
    void read_organism(std::ifstream &is, SimulationParameters &sp, OrganismBlockParameters &bp,
                       Organism *organism);
    bool read_organisms(std::ifstream& is, EngineDataContainer &edc, SimulationParameters &sp, OrganismBlockParameters &bp, uint32_t num_organisms);
    void read_organism_data(std::ifstream& is, OrganismData & data);
    void read_organism_brain(std::ifstream& is, Brain * brain);
    void read_organism_anatomy(std::ifstream& is, Anatomy * anatomy);


    void write_json_data(const std::string &path, EngineDataContainer &edc, SimulationParameters &sp, double total_total_mutation_rate);
    void json_write_grid(rapidjson::Document &d, EngineDataContainer &edc);
    void json_write_organisms(rapidjson::Document &d, EngineDataContainer &edc, SimulationParameters &sp);
    void write_json_organism(rapidjson::Document &d, Organism * organism, rapidjson::Value &j_organism, SimulationParameters &sp);
    void json_write_fossil_record(rapidjson::Document &d);
    void json_write_controls(rapidjson::Document &d, SimulationParameters &sp);

//    void json_read_simulation_parameters(rapidjson::Document & d, SimulationParameters &sp);
    void json_read_organism(rapidjson::GenericValue<rapidjson::UTF8<>> &organism, SimulationParameters &sp,
                            OrganismBlockParameters &bp, Organism *new_organism);
    void json_read_organisms_data(rapidjson::Document *d_, SimulationParameters *sp_, OrganismBlockParameters *bp_, EngineDataContainer *edc_);

    void json_read_simulation_parameters(rapidjson::Document * d, SimulationParameters * sp);
}

#endif //LIFEENGINEEXTENDED_DATASAVINGFUNCTIONS_H
