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
#include "GridBlocks/BaseGridBlock.h"
#include "Organisms/Organism.h"
#include "BlockTypes.h"
#include "EngineControlContainer.h"
#include "EngineDataContainer.h"
#include "OrganismBlockParameters.h"
#include "Linspace.h"

struct eager_worker;

class SimulationEngine {

    EngineControlParameters& cp;
    EngineDataContainer& dc;
    OrganismBlockParameters& op;

    std::mutex& mutex;

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> fps_timer;

    std::vector<eager_worker> threads;
    std::vector<int> thread_points;

    void process_user_action_pool();

    void simulation_tick();

    void multi_threaded_tick();
    void cuda_tick();
    void build_threads();

    void thread_wrapper();

    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<int> dist;

public:
    SimulationEngine(EngineDataContainer& engine_data_container, EngineControlParameters& engine_control_parameters,
                     OrganismBlockParameters& organism_block_parameters, std::mutex& mutex);
    void threaded_mainloop();

    static void single_threaded_tick(EngineDataContainer * dc,
                                     int start_relative_x = 0,
                                     int start_relative_y = 0,
                                     int end_relative_x = 0,
                                     int end_relative_y = 0);
};

struct eager_worker {
    EngineDataContainer * dc = nullptr;
    int start_relative_x = 0;
    int start_relative_y = 0;
    int end_relative_x = 0;
    int end_relative_y = 0;

    eager_worker() = default;
    eager_worker(EngineDataContainer * dc, int start_relative_x, int start_relative_y, int end_relative_x, int end_relative_y):
            dc(dc), start_relative_x(start_relative_x), start_relative_y(start_relative_y), end_relative_x(end_relative_x), end_relative_y(end_relative_y){}
    eager_worker(const eager_worker & worker):
            dc(worker.dc), start_relative_x(worker.start_relative_x), start_relative_y(worker.start_relative_y),
            end_relative_x(worker.end_relative_x), end_relative_y(worker.end_relative_y){}

    inline void work() {
        has_work.store(true);
    }

    inline void finish() {
        while (has_work.load()) {}
    }

    inline void stop_work() {
        has_work.store(true);
        exiting.store(true);
    }

    inline ~eager_worker() { stop_thread(); }
    inline void stop_thread() {
        exiting.store(true);
        has_work.store(true);
        if (thread.joinable()) {
            thread.join();
        }
    }
private:
    std::atomic<bool> has_work{false};

    std::atomic<bool> exiting{false};
    std::atomic<bool> thread_started{false};

    std::thread thread = std::thread([this] {
        thread_started.store(true);
        while (true) {
            while (!has_work.load()) {
                if (exiting.load()) {
                    return;
                }
            }
            SimulationEngine::single_threaded_tick(dc, start_relative_x, start_relative_y, end_relative_x, end_relative_y);
            has_work.store(false);
        }
    });
};

#endif //LANGUAGES_LIFEENGINE_H
