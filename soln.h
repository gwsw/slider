#ifndef _SOLN_H_
#define _SOLN_H_ 1

#include <string>
#include <deque>
#include "board.h"
#include "move.h"

extern bool dump_solns;
extern bool draw_solns;
extern bool show_moves;

class Soln : public std::deque<Move> {
public:
    Soln(Board const& board) : board_(board) {}
    Soln(Soln const& soln) : board_(soln.board_) { *this = soln; }
    Soln(Board const& board, std::string const& move_str) : board_(board) {
        for (int i = 0; i < (int) move_str.size(); ++i) {
            char piece_name = move_str.at(i);
            int pix = board.piece_pix(piece_name);
            if (pix < 0) continue; // skip invalid chars
            bool prime = (i+1 < (int) move_str.size() && move_str.at(i+1) == '\'');
            if (prime) ++i;
            Move move (board, pix, !prime);
            push_back(move);
        }
    }
    Soln& operator=(Soln const& soln) {
        std::deque<Move>::operator=(soln);
        return *this;
    }
    std::string to_string() const {
        std::string str;
        for (auto it = begin(); it != end(); ++it) {
            str.push_back(board_.piece_name(it->pix));
            if (!it->fwd) str.push_back('\'');
        }
        return str;
    }
    void print() const {
        printf("%s\n", to_string().c_str());
        if (!dump_solns && !draw_solns) return;
        Board board = board_;
        board.reset();
        for (auto imv = begin(); imv != end(); ++imv) {
            if (show_moves) {
                if (draw_solns) board.draw();
                if (dump_solns) board.dump();
                printf("\n");
            }
            board.move(imv->pix, imv->fwd);
        }
        if (draw_solns) board.draw();
        if (dump_solns) board.dump();
    }

private:
    Board const& board_;
};

#endif // _SOLN_H_
