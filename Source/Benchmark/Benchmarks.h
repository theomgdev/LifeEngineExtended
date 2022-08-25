//
// Created by spaceeye on 16.08.22.
//

#ifndef LIFEENGINEEXTENDED_BENCHMARKS_H
#define LIFEENGINEEXTENDED_BENCHMARKS_H

#include <QScrollBar>

#include "BenchmarkUI.h"
#include "../MainWindow/WindowUI.h"
#include "../SimulationEngine/SimulationEngineModes/SimulationEngineSingleThreadBenchmark.h"

class Benchmarks: public QWidget {
    Q_OBJECT
private:
    Ui::Benchmark ui{};
    Ui::MainWindow & parent_window;
    SimulationEngineSingleThreadBenchmark benchmark{};

    bool updated_after_end = false;

    void benchmark_buttons_enabled(bool state);
    void update_result_info();

    void closeEvent(QCloseEvent * event) override;
public:
    explicit Benchmarks(Ui::MainWindow & parent_window);

    void update_();
private slots:
    void b_benchmark_produce_food_slot();
    void b_benchmark_apply_damage_slot();
    void b_benchmark_eat_food_slot();
    void b_benchmark_get_observations_slot();
    void b_benchmark_move_organism_slot();
    void b_benchmark_reserve_organism_slot();
    void b_benchmark_rotate_organism_slot();
    void b_benchmark_think_decision_slot();
    void b_benchmark_tick_lifetime_slot();
    void b_benchmark_try_make_child_slot();
    void b_run_all_benchmarks_slot();
    void b_stop_benchmarks_slot();

};


#endif //LIFEENGINEEXTENDED_BENCHMARKS_H
