#ifndef _BOARD_H_
#define _BOARD_H_

#include <deque>
#include "piece.h"

#define MAX_WIDTH 100
#define MAX_HEIGHT 100

class Board {
public:
    Board(int width, int height) : width_(width), height_(height) {
        for (int x = 0; x < width_; ++x) {
            for (int y = 0; y < height_; ++y) {
                grid_[x][y] = '\0';
            }
        }
    }
    bool add_piece(Piece& piece) {
        pieces_.insert(pieces_.end(), piece);
        if (piece.orient == Piece::HORZ) {
            for (int x = 0; x < piece.len; ++x)
                grid_[piece.x+x][piece.y] = piece.name;
        } else {
            for (int y = 0; y < piece.len; ++y)
                grid_[piece.x][piece.y+y] = piece.name;
        }
        return true;
    }
    bool move(int pix, bool fwd) {
        if (pix < 0 || pix >= (int) pieces_.size()) return false;
        Piece& piece = pieces_[pix];
        if (fwd) {
            if (piece.orient == Piece::HORZ) {
                int next = piece.x + piece.len;
                if (next >= width_ || grid_[next][piece.y]) return false;
                grid_[next][piece.y] = piece.name;
                grid_[piece.x][piece.y] = '\0';
                piece.x++;
            } else {
                int next = piece.y + piece.len;
                if (next >= height_ || grid_[piece.x][next]) return false;
                grid_[piece.x][next] = piece.name;
                grid_[piece.x][piece.y] = '\0';
                piece.y++;
            }
        } else {
            if (piece.orient == Piece::HORZ) {
                int next = piece.x - 1;
                if (next < 0 || grid_[next][piece.y]) return false;
                grid_[next][piece.y] = piece.name;
                grid_[piece.x+piece.len-1][piece.y] = '\0';
                piece.x--;
            } else {
                int next = piece.y - 1;
                if (next < 0 || grid_[piece.x][next]) return false;
                grid_[piece.x][next] = piece.name;
                grid_[piece.x][piece.y+piece.len-1] = '\0';
                piece.y--;
            }
        }
        return true;
    }
    void dump() const {
        printf(" "); for (int x = 0; x < width_; ++x) { printf("-"); } printf("\n");
        for (int y = 0; y < height_; ++y) {
            printf("|");
            for (int x = 0; x < width_; ++x) {
                printf("%c", grid_[x][y] ? grid_[x][y] : '.');
            }
            printf("|\n");
        }
        printf(" "); for (int x = 0; x < width_; ++x) { printf("-"); } printf("\n");
    }
private:
    int width_;
    int height_;
    char grid_[MAX_WIDTH][MAX_HEIGHT];
    std::deque<Piece> pieces_;
};

#endif // _BOARD_H_
