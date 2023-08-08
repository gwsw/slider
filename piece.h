#ifndef _PIECE_H_
#define _PIECE_H_

class Piece {
    friend class Board;
public:
    enum Orient { HORZ, VERT };
    Piece(int x, int y, int len, Orient orient, char name = '\0') : ox(x), oy(y), x(x), y(y), len(len), orient(orient), name(name ? name : next_name_++) {}
    void reset() { x = ox; y = oy; }
private:
    int ox;
    int oy;
    int x;
    int y;
    int len;
    Orient orient;
    char name;
    static char next_name_;
};

#endif // _PIECE_H_
