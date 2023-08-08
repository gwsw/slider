#ifndef _SOLUTION_H_
#define _SOLUTION_H_

#include <stdio.h>
#include <deque>
#include "board.h"

extern bool verbose;

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

    bool solve_moves(int num_moves) {
        Soln soln;
        for (int i = 0; i < num_moves; ++i)
            soln.push_back(Move(board_));

        bool solved = false;
        for (;;) {
            if (!next_soln(soln))
                break;
            if (stupid_soln(soln))
                continue;
            if (verbose) {
                printf("check: ");
                print_soln(soln);
            }
            if (is_winner(soln)) {
                solved = true;
                printf("solution: ");
                print_soln(soln);
                // break;
            }
        }
        return solved;
    }

private:
    bool next_soln(Soln& soln) {
        for (int i = soln.size()-1; i >= 0; --i) {
            if (soln[i].next()) return true;
        }
        return false;
    }
    bool stupid_soln(Soln const& soln) {
return false;
    }
    bool is_winner(Soln& soln) const {
        board_.reset();
        for (auto imv = soln.begin(); imv != soln.end(); ++imv)
            board_.move(imv->pix, imv->fwd);
        return board_.is_won();
    }
    void print_soln(Soln& soln) {
        for (auto it = soln.begin(); it != soln.end(); ++it)
            printf("%c%s ", board_.piece_name(it->pix), it->fwd ? "" : "'");
        printf("\n");
    }

private:
    Board& board_;
};

#endif // _SOLUTION_H_
