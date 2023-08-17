#ifndef _WORKER_H_
#define _WORKER_H_

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "solve.h"

class Worker {
public:
    class WorkerMgr {
        public:
        virtual bool get_job(Soln& soln, int& max_depth) = 0;
        virtual void announce_winner(Soln const& soln) = 0;
        virtual Solver::SolverMgr& smgr() = 0;
    };

    Worker(Board const& board, WorkerMgr& mgr)
        : board_(board), mgr_(mgr), stop_(false), thread_(std::thread(run_s, this)) {
    }
    ~Worker() {
        stop();
        join();
    }
    void stop() {
        std::unique_lock lock(thread_lock_);
        stop_ = true;
    }
    void join() {
        thread_.join();
    }

private:
    void run() {
        for (;;) {
            {
            std::unique_lock lock(thread_lock_);
            if (stop_) break;
            }
            Soln base_soln (board_);
            int max_depth;
            if (!mgr_.get_job(base_soln, max_depth)) break;
            Board board = board_;
            bool ok = true;
            for (auto it = base_soln.begin(); it != base_soln.end(); ++it)
                if (!board.move(it->pix, it->fwd)) {
                    ok = false;
                    break;
            }
            if (ok) {
                Solver solver (board);
                std::list<Soln> solns = solver.find_solns(base_soln, max_depth, mgr_.smgr());
                for (auto it = solns.begin(); it != solns.end(); ++it)
                    mgr_.announce_winner(*it);
            }
        }
    }
    static void run_s(Worker* worker) { worker->run(); }

private:
    Board board_;
    WorkerMgr& mgr_;
    bool stop_;
    std::thread thread_;
    mutable std::mutex thread_lock_;
};

#endif // _WORKER_H_
