#ifndef _MGR_H_
#define _MGR_H_

#include <list>
#include <mutex>
#include "worker.h"

extern bool dump_solns;
extern bool draw_solns;

class Mgr : public Worker::WorkerMgr, public Solver::SolverMgr {
public:
    Mgr(Board const& board, int max_depth, void (*progress)(void*,int) = NULL, void* ctx = NULL, int num_threads = 1, int branch_levels = 3) :
            board_(board), max_depth_(max_depth-branch_levels), best_(board), progress_(progress), ctx_(ctx), num_done_(0) {
        Soln soln (board);
        init_branches(board, soln, branch_levels);
        for (int w = 0; w < num_threads; ++w) {
            workers_.push_back(new Worker(board, *this));
        }
    }
    ~Mgr() {
        while (!workers_.empty()) {
            Worker* worker = workers_.back();
            workers_.pop_back();
            delete worker;
        }
    }
    void init_branches(Board const& board, Soln& soln, int branch_levels) {
        if (branch_levels <= 0) return;
        for (Move move (board);;) {
            if (!Solver::stupid_move(soln, move)) {
                soln.push_back(move);
                if (branch_levels == 1) {
                    branches_.push_back(soln);
                }
                init_branches(board, soln, branch_levels-1);
                soln.pop_back();
            }
            if (!move.next()) break;
        }
        total_branches_ = (int) branches_.size();
    }
    virtual bool get_job(Soln& soln, int& max_depth) override /* WorkerMgr */ {
        std::unique_lock lock(job_lock_);
        if (branches_.empty()) return false;
        soln = branches_.back();
        branches_.pop_back();
        max_depth = max_depth_;
        if (progress_) progress_(ctx_, 100* (total_branches_ - branches_.size()) / total_branches_);
        return true;
    }
    virtual void announce_winner(Soln const& soln) override /* WorkerMgr */ {
        if (best_.size() == 0 || soln.size() < best_.size())
            best_ = soln;
    }
    virtual Solver::SolverMgr& smgr() override /* WorkerMgr */ {
        return *this;
    }
    virtual int best_depth() const override /* SolverMgr */ {
        int depth = (int) best_.size();
        if (depth == 0) return 9999999;
        return depth;
    }
    bool run() {
        while (!workers_.empty()) {
            Worker* worker = workers_.back();
            worker->join();
            workers_.pop_back();
        }
        if (best_.size() > 0) {
            printf("Winner: ");
            best_.print(dump_solns, draw_solns);
            return true;
        }
        return false;
    }

private:
    Board const& board_;
    int max_depth_;
    Soln best_;
    int total_branches_;
    void (*progress_)(void*,int);
    void* ctx_;
    int num_done_;
    std::list<Soln> branches_;
    std::list<Worker*> workers_;
    std::mutex job_lock_;
};

#endif // _MGR_H_
