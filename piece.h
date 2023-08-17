#ifndef _PIECE_H_
#define _PIECE_H_

class Piece {
    friend class Board;
public:
    enum Orient { HORZ, VERT };
    Piece(int x, int y, int len, Orient orient, char name = '\0') : ox_(x), oy_(y), x_(x), y_(y), len_(len), orient_(orient), name_(name ? name : next_name_++) {}
    void reset() { x_ = ox_; y_ = oy_; }
private:
    int ox_;
    int oy_;
    int x_;
    int y_;
    int len_;
    Orient orient_;
    char name_;
    static char next_name_;
};

#endif // _PIECE_H_
