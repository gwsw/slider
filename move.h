#ifndef _MOVE_H_
#define _MOVE_H_ 1

#include "board.h"

struct Move {
    Move(Board const& board, int pix = 0, bool fwd = true) : pix(pix), fwd(fwd), num_pieces(board.num_pieces()) {}
    Move const& operator=(Move const& m) {
        pix = m.pix;
        fwd = m.fwd;
        return *this;
    }
    bool next() {
        if (fwd) {
            fwd = false;
        } else if (pix+1 < num_pieces) {
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
    int num_pieces;
};

#endif // _MOVE_H_
