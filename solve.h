#ifndef _SOLVE_H_
#define _SOLVE_H_

#include <stdio.h>
#include <assert.h>
#include <list>
#include <deque>
#include "move.h"
#include "soln.h"

extern bool verbose;
extern bool print_asap;

class Solver {
private:

public:
    class SolverMgr { public: virtual int best_depth() const = 0; };
    Solver(Board& board) : board_(board) {}

    std::list<Soln> find_solns(Soln& soln, int max_depth, SolverMgr const& mgr) {
        if (verbose) { printf("try [%d]:\n", max_depth); board_.dump(); }
        std::list<Soln> found;
        if (mgr.best_depth() <= (int) soln.size()+1) {
            // Prune unnecessary search; we already have a better solution (or as good).
            return found;
        }
        if (board_.is_won()) {
            found.push_back(soln);
            if (print_asap) soln.print();
        }
        if (max_depth <= 0)
            return found;
        Move move(board_);
        int pct_done = 0;
        for (;;) {
            if (!stupid_move(soln, move) && board_.move(move.pix, move.fwd)) {
                // Make a trial move.
                soln.push_back(move);
                // Find all solutions descending from the trial move.
                std::list<Soln> sub_solns = find_solns(soln, max_depth-1, mgr);
                if (!sub_solns.empty()) {
                    for (auto it = sub_solns.begin(); it != sub_solns.end(); ++it)
                        found.push_back(*it);
                }
                // Undo the trial move.
                if (!board_.move(move.pix, !move.fwd)) assert(false);
                soln.pop_back();
            }
            pct_done += 100 / (2 * board_.num_pieces());
            if (!move.next()) break;
        }
        return found;
    }

private:
    bool stupid_move(Soln const& soln, Move const& move) {
        Move const& last = soln.back();
        if (move.pix == last.pix && move.fwd == !last.fwd) return true;
        return false;
    }
#if 0
    bool is_winner(Soln const& soln) const {
        board_.reset();
        for (auto imv = soln.begin(); imv != soln.end(); ++imv)
            board_.move(imv->pix, imv->fwd);
        return board_.is_won();
    }
#endif

private:
    Board& board_;
};

#endif // _SOLVE_H_
