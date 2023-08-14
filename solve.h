#ifndef _SOLUTION_H_
#define _SOLUTION_H_

#include <stdio.h>
#include <assert.h>
#include <list>
#include <deque>
#include "board.h"

static bool print_any = false;
extern bool verbose;
extern bool print_all;
extern bool show_moves;
extern bool dump_solns;
extern bool draw_solns;

class Solver {
private:
    struct Move {
        Move(Board const& board, int pix = 0, bool fwd = true) : pix(pix), fwd(fwd), board(board) {}
        bool next() {
            if (fwd) {
                fwd = false;
            } else if (pix+1 < board.num_pieces()) {
                ++pix;
                fwd = true;
            } else {
                pix = 0;
                fwd = true;
                return false;
            }
            return true;
        }
        int pix;
        bool fwd;
    private:
        Board const& board;
    };
    typedef std::deque<Move> Soln;

public:
    Solver(Board& board) : board_(board) {}

    bool solve(int max_depth, void (*progress)(void*,int), void* ctx) {
        Soln empty_soln;
        std::list<Soln> found = find_solns(empty_soln, max_depth, progress, ctx);
        if (found.empty())
            return false;
        auto best = found.end();
        for (auto it = found.begin(); it != found.end(); ++it) {
            if (print_all) {
                printf("WINNER: "); print_soln(*it, dump_solns, draw_solns);
            }
            if (best == found.end() || it->size() < best->size())
                best = it;
        }
        if (best == found.end()) {
            printf("no solution in %d moves\n", max_depth);
        } else if (!print_all) { // if print_all, already printed
            printf("WINNER: "); print_soln(*best, dump_solns, draw_solns);
        }
        return true;
    }

private:
    std::list<Soln> find_solns(Soln& soln, int max_depth, void (*progress)(void*,int) = NULL, void* ctx = NULL) {
        if (verbose) { printf("try [%d]:\n", max_depth); board_.dump(); }
        std::list<Soln> found;
        if (board_.is_won()) {
            found.push_back(soln);
            if (print_any) return found;
        }
        if (max_depth <= 0)
            return found;
        Move move(board_);
        int pct_done = 0;
        for (;;) {
            if (progress) progress(ctx, pct_done);
            if (board_.move(move.pix, move.fwd)) {
                // Make a trial move.
                soln.push_back(move);
                // Find all solutions descending from the trial move.
                std::list<Soln> sub_solns = find_solns(soln, max_depth-1);
                if (!sub_solns.empty()) {
                    for (auto it = sub_solns.begin(); it != sub_solns.end(); ++it)
                        found.push_back(*it);
                    if (print_any) return found;
                }
                // Undo the trial move.
                if (!board_.move(move.pix, !move.fwd))
                    assert(false);
                soln.pop_back();
            }
            pct_done += 100 / (2 * board_.num_pieces());
            if (!move.next()) break;
        }
        if (progress) { progress(ctx, 100); printf("\n"); }
        return found;
    }
    bool is_winner(Soln& soln) const {
        board_.reset();
        for (auto imv = soln.begin(); imv != soln.end(); ++imv)
            board_.move(imv->pix, imv->fwd);
        return board_.is_won();
    }
    void print_soln(Soln& soln, bool dump, bool draw) {
        print_moves(soln);
        printf("\n");
        if (show_moves) {
            Board board (board_.get_width(), board_.get_height());
            printf("Start:\n");
            int nmove = 0;
            for (auto imv = soln.begin(); imv != soln.end(); ++imv) {
                print_board(board, dump, draw);
                printf("\n");
                board.move(imv->pix, imv->fwd);
                printf("Move %d:\n", ++nmove);
            }
        }
        print_board(board_, dump, draw);
    }
    void print_moves(Soln& soln) {
        for (auto it = soln.begin(); it != soln.end(); ++it)
            printf("%c%s ", board_.piece_name(it->pix), it->fwd ? "" : "'");
    }
    void print_board(Board const& board, bool dump, bool draw) {
        if (draw) board.draw();
        if (dump) board.dump();
    }

private:
    Board& board_;
};

#endif // _SOLUTION_H_
