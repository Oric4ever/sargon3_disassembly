#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#define WHITE 0
#define BLACK 0x10
#define EMPTY -1
#define END   -1
#define ERROR 1
#define OK    0

enum Types { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum Pieces{ PAWN1, PAWN2, PAWN3, PAWN4, PAWN5, PAWN6, PAWN7, PAWN8, KNIGHT1, KNIGHT2, BISHOP1, BISHOP2, ROOK1, ROOK2, QUEEN1, KING1 };

typedef struct { int from, to; } Move;
typedef Move Moves[100];
typedef int8_t Board[64];       // piece numbers : 0-15 white pieces, 16-31 black pieces
typedef int8_t Locations[32];   // locations of the 32 pieces
typedef int8_t Piece_types[32];
Piece_types piece_type = {
    PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,
    KNIGHT, KNIGHT, BISHOP, BISHOP, ROOK, ROOK, QUEEN, KING,
    PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,
    KNIGHT, KNIGHT, BISHOP, BISHOP, ROOK, ROOK, QUEEN, KING
};

Board board;
Locations piece_location = {
    010, 011, 012, 013, 014, 015, 016, 017, // white pawns
    001, 006, 002, 005, 000, 007, 003, 004, // white pieces
    060, 061, 062, 063, 064, 065, 066, 067, // black pawns
    071, 076, 072, 075, 070, 077, 073, 074  // black pieces
};

void print_board() {
    for (int y=7; y>=0; y--) {
        printf("\n");
        for (int x=0; x<8; x++) {
            int i = x + 8*y;
            printf(board[i] < 0 ? "%d ":"%02x ", board[i]);
        }
    }
    for (int i=0; i<32; i++) {
        if (i % 16 == 0) printf("\n");
        if (piece_location[i]!=EMPTY) printf("%c%d ", piece_location[i]%8+'a', piece_location[i]/8+1);
        else printf("xx ");
    }
    printf("\n");
}

int max(int x, int y) { return x < y ? y : x; }
int abs(int x) { return x < 0 ? -x : x; }
int sgn(int x) { return x < 0 ? -1
                      : x > 0 ? +1
                      : 0;
}

bool      odd(int x  ) { return x % 2 == 1;          }
bool is_empty(int pos) { return board[pos] == EMPTY; }
int     color(int pos) { return board[pos] & 0x10;   }
int     piece(int pos) { assert(board[pos]!=EMPTY); return piece_type[board[pos]];   }
int   adverse(int col) { return col ^ 0x10;          }
bool is_white(int pos) { return color(pos) == WHITE; }
bool is_black(int pos) { return color(pos) == BLACK; }
bool is_alive(int pce) { return piece_location[pce] != EMPTY; }

bool are_same_color(int from, int to) {
    assert( !is_empty(from) );
    return !is_empty(to) && color(from)==color(to);
}

bool can_pawn_take(int from, int to) {
    assert( !is_empty(from) );
    assert( piece(from) == PAWN );
    if (are_same_color(from, to)) return false;

    int x1 = from % 8, y1 = from / 8;
    int x2 =  to  % 8, y2 =  to  / 8;
    int forward_dir = is_white(from) ? +1 : -1;
    return abs(x1-x2)==1 && y2-y1 == forward_dir;
}

bool is_pawn_move(int from, int to) {
    assert( piece(from) == PAWN );
    assert( is_empty(to) );
    int forward_step = is_white(from) ? +8 : -8;
    return to-from == forward_step;
}

bool is_pawn_entry(int from, int to) {
    assert( piece(from) == PAWN );
    assert( is_empty(to) );
    int y1 = from / 8;
    int forward_step = is_white(from) ? +8 : -8;
    int start_row = is_white(from) ? 1 : 6;
    return y1 == start_row && is_empty(from+forward_step) && to-from == 2*forward_step;
}

bool is_knight_move(int from, int to) {
    assert( piece(from) == KNIGHT );

    if (are_same_color(from, to)) return false;

    int x1 = from % 8, y1 = from / 8;
    int x2 =  to  % 8, y2 =  to  / 8;
    return (abs(x1-x2)==1 && abs(y1-y2)==2)
        || (abs(x1-x2)==2 && abs(y1-y2)==1);
}

bool is_bishop_move(int from, int to) {
    assert( piece(from) == BISHOP );

    if (are_same_color(from, to)) return false;

    int x1 = from % 8, y1 = from / 8;
    int x2 =  to  % 8, y2 =  to  / 8;
    if (abs(x1-x2) != abs(y1-y2)) return false;

    int step = sgn(x2-x1) + 8*sgn(y2-y1);
    for (int n=1; n < abs(x1-x2); n++)
        if (!is_empty(from+n*step)) return false;
    return true;
}

bool is_rook_move(int from, int to) {
    assert( piece(from) == ROOK );

    if (are_same_color(from, to)) return false;

    int x1 = from % 8, y1 = from / 8;
    int x2 =  to  % 8, y2 =  to  / 8;
    if (x1 != x2 && y1 != y2) return false;

    int step = sgn(x2-x1) + 8*sgn(y2-y1);
    for (int n=1; n < max( abs(x1-x2), abs(y1-y2) ); n++)
        if (!is_empty(from+n*step)) return false;
    return true;
}

bool is_queen_move(int from, int to) {
    assert( piece(from) == QUEEN );

    if (are_same_color(from, to)) return false;

    int x1 = from % 8, y1 = from / 8;
    int x2 =  to  % 8, y2 =  to  / 8;
    if (abs(x1-x2) != abs(y1-y2) && x1 != x2 && y1 != y2) return false;

    int step = sgn(x2-x1) + 8*sgn(y2-y1);
    for (int n=1; n < max( abs(x1-x2), abs(y1-y2) ); n++)
        if (!is_empty(from+n*step)) return false;
    return true;
}

bool is_king_move(int from, int to) {
    assert( piece(from) == KING );

    if (are_same_color(from, to)) return false;
    int x1 = from % 8, y1 = from / 8;
    int x2 =  to  % 8, y2 =  to  / 8;
    if (abs(x1-x2) > 1 || abs(y1-y2) > 1) return false;

    return true;
}

bool can_move(int from, int to) {
   assert( !is_empty(from) );
   switch (piece(from)) {
       case PAWN  : return is_pawn_move(from, to);
       case KNIGHT: return is_knight_move(from, to);
       case BISHOP: return is_bishop_move(from, to);
       case ROOK  : return is_rook_move(from, to);
       case QUEEN : return is_queen_move(from, to);
       case KING  : return is_king_move(from, to);
       default    : return false;
   }
}

bool can_take(int from, int pos) {
   assert( !is_empty(from) );
   switch (piece(from)) {
       case PAWN  : return can_pawn_take(from, pos);
       case KNIGHT: return is_knight_move(from, pos);
       case BISHOP: return is_bishop_move(from, pos);
       case ROOK  : return is_rook_move(from, pos);
       case QUEEN : return is_queen_move(from, pos);
       case KING  : return is_king_move(from, pos);
       default    : return false;
   }
}

bool is_threaten(int pos, int attacker) {
    // see if a piece (or pawn) threatens
    for (int i = 0; i < 16; i++) {
        int from = piece_location[attacker+i];
        if (is_alive(attacker+i) && can_take(from, pos)) return true;
    }
    return false;
}

bool is_protected(int pos, int color) {
    bool protected = false;
    // pretend there is something to take at pos
    int save = board[pos];
    board[pos] = adverse(color);

    for (int i = 0; i < 16; i++) {
        int from = piece_location[color+i];
        if (is_alive(color+i) && can_take(from, pos)) protected = true;
    }

    board[pos] = save;
    return protected;
}

int find_king_attacker(int king_color) {
    int attacker = adverse(king_color);
    int king_pos = piece_location[king_color+15];

    for (int i = 0; i < 15; i++) {
        int from = piece_location[attacker+i];
        if (is_alive(attacker+i) && can_take(from, king_pos)) return from;
    }
    assert(false);
}


void do_move(Move m);

Moves moves;
int nb_moves;
void register_move(int from, int to) {
    assert( !is_empty(from) );
    Move this_move = { .from = from, .to = to };
    moves[nb_moves++] = this_move;
}

void try_to_take(int pos) {
    assert( !is_empty(pos) );
    int defenser = color(pos);
    int attacker = adverse(defenser);
    for (int i = PAWN1; i <= KING1; i++) {
        int from = piece_location[attacker+i];
        if (is_alive(attacker+i) && can_take(from, pos)) {
            register_move(from, pos);
            if (piece(from)!=PAWN) continue;
            if ((attacker==WHITE && pos >= 070)
             || (attacker==BLACK && pos <  010))
                for (int i=0; i<3; i++)
                    register_move(from, pos); // register underpromotion
        }
    }
    /*
    // try to take with a pawn first
    for (int pawn = attacker+P1; pawn <= attacker+P8; pawn++) {
        int from = piece_location[pawn];
        if (is_alive(pawn) && can_pawn_take(from, pos)) {
            register_move(from, pos);
            if ((attacker==WHITE && pos >= 070)
             || (attacker==BLACK && pos <  010))
                for (int i=0; i<3; i++)
                    register_move(from, pos); // register underpromotion
        }
    }
    // then with a piece
    if (is_alive(attacker+N1) && is_knight_move(piece_location[attacker+N1], pos)) register_move(piece_location[attacker+N1], pos);
    if (is_alive(attacker+N2) && is_knight_move(piece_location[attacker+N2], pos)) register_move(piece_location[attacker+N2], pos);
    if (is_alive(attacker+B1) && is_bishop_move(piece_location[attacker+B1], pos)) register_move(piece_location[attacker+B1], pos);
    if (is_alive(attacker+B2) && is_bishop_move(piece_location[attacker+B2], pos)) register_move(piece_location[attacker+B2], pos);
    if (is_alive(attacker+R1) &&   is_rook_move(piece_location[attacker+R1], pos)) register_move(piece_location[attacker+R1], pos);
    if (is_alive(attacker+R2) &&   is_rook_move(piece_location[attacker+R2], pos)) register_move(piece_location[attacker+R2], pos);
    if (is_alive(attacker+Q ) &&  is_queen_move(piece_location[attacker+Q ], pos)) register_move(piece_location[attacker+Q ], pos);
    if (is_alive(attacker+K ) &&   is_king_move(piece_location[attacker+K ], pos)) register_move(piece_location[attacker+K ], pos);
    */
}

void try_to_move_a_piece_to(int pos, int color) {
    assert( is_empty(pos) );
    
    for (int i = KNIGHT1; i <= KING1; i++) {
        int from = piece_location[color+i];
        if (is_alive(color+i) && can_move(from, pos))
            register_move(from, pos);
    }
}

void try_to_move_a_promoted_piece_to(int pos, int color) {
    assert( is_empty(pos) );
    
    for (int i = PAWN1; i <= PAWN8; i++) {
        int from = piece_location[color+i];
        if (is_alive(color+i) && piece(from)!=PAWN && can_move(from, pos))
            register_move(from, pos);
    }
}

void try_king_take(int king_pos) {
    int turn = color(king_pos), opponent = adverse(turn);
    static int dx[] = { -1, +1,  0, -1, +1,  0, -1, +1 };
    static int dy[] = {  0, -1, -1, -1, +1, +1, +1,  0 };
    for (int dir = 7; dir >= 0 ; dir--) {
        int x = king_pos % 8, y = king_pos / 8;
        int x2 = x + dx[dir], y2 = y + dy[dir];
        if (x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7) continue;

        int king_to = x2 + y2*8;
        if (is_empty(king_to) || are_same_color(king_pos, king_to)) continue;
        if (!is_protected(king_to, opponent)) register_move(king_pos, king_to);
    }
}

void try_king_move(int king_pos) {
    int turn = color(king_pos), opponent = adverse(turn);
    static int dx[] = { -1, +1,  0, -1, +1,  0, -1, +1 };
    static int dy[] = {  0, -1, -1, -1, +1, +1, +1,  0 };
    for (int dir = 7; dir >= 0 ; dir--) {
        int x = king_pos % 8, y = king_pos / 8;
        int x2 = x + dx[dir], y2 = y + dy[dir];
        if (x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7) continue;

        int king_to = x2 + y2*8;
        if (!is_empty(king_to)) continue;
        if (!is_threaten(king_to, opponent)) register_move(king_pos, king_to);
    }
}

void search_moves_under_check(int turn, Move last) {
    int king_pos = piece_location[turn+KING1];
    int attacker = adverse(turn);

    int nb_checks = 0;
    for (int i=PAWN1; i <= QUEEN1; i++) {
        int from = piece_location[attacker+i];
        if (is_alive(attacker+i) && can_take(from, king_pos)) nb_checks++;
    }
    if (nb_checks > 1) { // double check => can only try to evade
        try_king_take(king_pos);
        try_king_move(king_pos);
        return;
    }

    // Single check

    // TODO: handle king threatened by pawn entry move
    // TODO: handle removing threat by pawn promotion taking attacker

    // see if some pieces (not the king himself) can take the attacker
    int attacker_pos = find_king_attacker(turn);
    for (int i = PAWN1; i <= QUEEN1; i++) {
        int from = piece_location[turn+i];
        if (is_alive(turn+i) && can_take(from, attacker_pos))
            register_move(from, attacker_pos);  // TODO: handle promotion at the same time
    }

    // then try to have the King eat a white unprotected neighbor
    try_king_take(king_pos);

    // then try every shield position between the attacker and the king
    if (piece(attacker_pos) != PAWN && piece(attacker_pos) != KNIGHT) {
        int x1 = attacker_pos % 8, y1 = attacker_pos / 8;
        int x2 =     king_pos % 8, y2 =     king_pos / 8;
        int dx = sgn(x2-x1), dy = sgn(y2-y1);
        int step = dx + dy*8;
        for (int shield_pos = attacker_pos + step; shield_pos != king_pos; shield_pos += step) {
            for (int i = KNIGHT1; i <= QUEEN1; i++) // first try to move normal pieces
                if (is_alive(turn+i) && can_move(piece_location[turn+i], shield_pos))
                    register_move(piece_location[turn+i], shield_pos);
            for (int i=PAWN1; i <= PAWN8; i++) // then try to move promoted pawns
                if (is_alive(turn+i) && piece_type[turn+i]!=PAWN && can_move(piece_location[turn+i], shield_pos))
                    register_move(piece_location[turn+i], shield_pos);
            for (int i=PAWN1; i <= PAWN8; i++) { // then try to move pawns
                if (is_alive(turn+i) && piece_type[turn+i]==PAWN && is_pawn_move(piece_location[turn+i], shield_pos))
                    register_move(piece_location[turn+i], shield_pos);
                if (is_alive(turn+i) && piece_type[turn+i]==PAWN && is_pawn_entry(piece_location[turn+i], shield_pos))
                    register_move(piece_location[turn+i], shield_pos);
            }
        }
    }

    // finally, try evading the King
    try_king_move(king_pos);
}

void verify_board(void) {
    for (int i=0; i<32; i++) {
        int pos = piece_location[i];
        if (pos != EMPTY && board[pos]!=i)
        { fprintf(stderr, "BOARD ERROR!!\n"); print_board(); exit(1); }
    }

    for (int pos = 0; pos <= 077; pos++) {
        if (board[pos] != EMPTY && piece_location[board[pos]] != pos)
        { fprintf(stderr, "BOARD ERROR!!\n"); print_board(); exit(1); }
    }
}

void search_moves(int turn, Move last) {
    verify_board();

    int ennemy = adverse(turn);

    nb_moves = 0;

    if (is_threaten(piece_location[turn+KING1], ennemy)) {
        search_moves_under_check(turn, last);
        return;
    }

    // first see if en-passant take is possible
    int last_moved_piece = piece(last.to);
    if (abs(last.to-last.from)==2*8 && last_moved_piece==PAWN) {
        // pretend the pawn only did a single step
        int pretend_location = color(last.to) == WHITE ? last.to-8 : last.to+8;
        int pawn = board[last.to];
        board[pretend_location] = pawn;
        board[last.to]     = EMPTY;
        piece_location[pawn] = pretend_location;

        // try to take this pawn with a pawn
        for (int my_pawn = turn+PAWN1; my_pawn <= turn+PAWN8; my_pawn++) {
            int from = piece_location[my_pawn];
            if (is_alive(my_pawn) && piece(from)==PAWN && can_pawn_take(from, pretend_location))
                register_move(from, pretend_location);
        }

        // restore the pawn at its right place
        board[last.to] = pawn;
        board[pretend_location] = EMPTY;
        piece_location[pawn] = last.to;
    }

    // then try to take material
    for (int taken = ennemy+QUEEN1 ; taken >= ennemy+PAWN1; taken--) {
        int pos = piece_location[taken];
        if (is_alive(taken)) try_to_take(pos);
    }

    // then try to promote pawn
    for (int pawn=turn+PAWN1; pawn <= turn+PAWN8; pawn++) {
        int from = piece_location[pawn];
        if (is_alive(pawn) && piece(from)==PAWN) {
            int from = piece_location[pawn];
            int to   = turn==WHITE ? from+8 : from-8;
            if (is_empty(to) && (to >= 070 || to < 010))
                for (int i=0; i<4; i++)
                    register_move(from, to); // register promotion and underpromotions
        }
    }

    // then try to castle
    int king_location = piece_location[turn+KING1];
    int row = turn == WHITE ? 0 : 070;
    if (king_location == 004+row && !is_threaten(004+row, ennemy)   // TODO: check they haven't moved
     && is_alive(turn+ROOK2) && piece_location[turn+ROOK2]==007+row
     && is_empty(005+row) && !is_threaten(005+row, ennemy)
     && is_empty(006+row) && !is_threaten(006+row, ennemy))
        register_move(004+row, 006+row);
    if (king_location == 004+row && !is_threaten(004+row, ennemy)
     && is_alive(turn+ROOK1) && piece_location[turn+ROOK1]==000+row
     && is_empty(001+row)
     && is_empty(002+row) && !is_threaten(002+row, ennemy)
     && is_empty(003+row) && !is_threaten(003+row, ennemy))
        register_move(004+row, 002+row);

    // then try to move a piece to priorized locations...
    static int next_location[64] = {
        001, 002, 003, 004, 005, 006, 007, 077,
        060, 061, 062, 063, 064, 065, 066, 067,
        050, 051, 052, 053, 054, 055, 056, 057,
        040, 041, 042, 043, 044, 045, 046, 047,
        020, 021, 022, 034, 035, 024, 026, 027,
        014, 030, 037, 036, 025, 023, 032, 031,
        000, 017, 016, 015, 013, 012, 011, 010,
        END, 070, 071, 072, 073, 074, 075, 076
    };

    for (int pos = 033; pos != END; pos = next_location[pos])
        if (is_empty(pos)) try_to_move_a_piece_to(pos, turn);

    for (int pos = 033; pos != END; pos = next_location[pos])
        if (is_empty(pos)) try_to_move_a_promoted_piece_to(pos, turn);

    // ... then move pawns
    for (int pawn = turn+PAWN8; pawn >= turn+PAWN1; pawn--) {
        int from = piece_location[pawn];
        if (is_alive(pawn) && piece(from)==PAWN) { // TODO: handle promoted pawns
            int from = piece_location[pawn];
            int forward_step = turn==WHITE ? +8 : -8;
            int start_row = turn==WHITE ? 1 : 6;
            int y =  from / 8;
            int to = from + forward_step;
            if (is_empty(to)) {
                if (to >= 070 || to < 010) continue; // promotions were already registered
                register_move(from, to);
                if (y==start_row && is_empty(to+forward_step))
                    register_move(from, to+forward_step);
            }
        }
    }
}

void init_board() {
    for (int pos = 0; pos < 64; pos++) board[pos] = EMPTY;
    for (int piece = 0; piece < 32; piece++) {
        board[piece_location[piece]] = piece;
    }
}

void print_move(int ply, Move m) {
    for (int i = 0; i < ply; i++) printf("     ");
    printf("%2d. ", ply);
//    if (ply % 2 == 1) printf("  ..  ");
    int x1 = m.from % 8, y1 = m.from / 8;
    int x2 = m.to   % 8, y2 = m.to   / 8;
    char separator = is_empty(m.to) ? '-' : 'x';
    printf("%c%c%c%c%c",'A'+x1,'1'+y1,separator,'A'+x2,'1'+y2);
}

void print_moves(int ply) {
    for (int i=0; i<nb_moves; i++) {
        printf("%02x: ", i+1);
        print_move(ply, moves[i]);
        printf("\n");
    }
}

void do_move(Move m) {
    assert( !is_empty(m.from) );
    int turn  = color(m.from), opponent = adverse(turn);
    int p     = board[m.from];
    int taken = board[m.to];
    int x1 = m.from % 8, y1 = m.from / 8;
    int x2 = m.to   % 8;
    bool pawn_move = piece(m.from) == PAWN;

    if (pawn_move && taken == EMPTY && abs(x1-x2)==1) { // en-passant
        taken = board[x2 + y1 * 8];
        board[x2 + y1 * 8] = EMPTY;
    } 
    if (taken != EMPTY) piece_location[taken] = EMPTY;
    board[m.from] = EMPTY;
    board[m.to] = p;
    piece_location[p] = m.to;

    if (piece(m.to) == KING && m.to == m.from + 2) { // small castle
        int rook = board[m.to + 1];
        board[m.to + 1] = EMPTY;
        board[m.to - 1] = rook;  // move rook
        piece_location[rook] = m.to-1;
    }
    if (piece(m.to) == KING && m.to == m.from - 2)  { // big castle
        int rook = board[m.to - 2];
        board[m.to - 2] = EMPTY;
        board[m.to + 1] = rook;
        piece_location[rook] = m.to+1;
    }
    if (pawn_move && (m.to >= 070 || m.to < 010)) { // promotion
        piece_type[board[m.to]] = QUEEN;    // no underpromotion for now
        printf("=Q");
    }
    if (is_threaten(piece_location[opponent+KING1], turn)) putchar('+');
}

Board board_backup[200];
Locations locations_backup[200];
Piece_types types_backup[200];

uint8_t book[100000];
int     book_indx;

void init_book(char *name) {
    FILE *file = fopen(name,"r");
    if (!file) {
        fprintf(stderr, "%s not found\n", name);
        exit(1);
    }
    fread(book, sizeof book, 1, file);
    fclose(file);
}

int skip_branch(int flags) {
//    if (!flags) return ERROR;
    int level = 1;
    while (level != 0) {
        printf("%03x: %02x %+d\n", book_indx, book[book_indx], level);
/*
        if (level == 1) {
            if (book[book_indx] == 0xC0) printf("c0 %02x\n", book[book_indx+1]);
            else if (book[book_indx] != 0xC5) printf("%02x\n", book[book_indx]);
        }
*/              
//printf("level=%d, %03x: %02x\n",level,book_indx,book[book_indx]);
        switch (book[book_indx] & 0xC0) {
            case 0x00: break;
            case 0x40: level--; break;
            case 0x80: level++; break;
            case 0xC0: if (book[book_indx] & 7) { book_indx += 4; level--; }
                       else level++;
                       break;
        }
        book_indx ++;
    }
    return 0;
}

void print_sub_book_name() {
    printf(" => ");
    for (int i=0; i<4; i++)
        putchar(book[++book_indx]);
}

void skip_all_branches(int flags) {
   if (book[book_indx] == 0xC5) {
       print_sub_book_name();
       book_indx += 1;
       return;
   }

   printf(".....\n");
   while (flags & 0x80) {
       skip_branch(flags);
       flags = book[book_indx];
   }
   // skip last branch too
   skip_branch(flags);
}

int print_variations() {
    int save  = book_indx;
    int count = 0;

    book_indx-=2;
    skip_branch(0xC0);

    book_indx = save;
    return count;
}

void decode_variations(int depth, Move last) {
    int turn = odd(depth) ? BLACK : WHITE;

    // save current position
    memcpy(board_backup[depth], board, sizeof board);
    memcpy(locations_backup[depth], piece_location, sizeof piece_location);
    memcpy(types_backup[depth], piece_type, sizeof piece_type);

    int flags;
    do {
        // restore position
        memcpy(board, board_backup[depth], sizeof board);
        memcpy(piece_location, locations_backup[depth], sizeof piece_location);
        memcpy(piece_type, types_backup[depth], sizeof piece_type);

        flags     = book[book_indx] & 0xC0;
        if (flags == 0xC0) {
//printf("\n%03x: %02x", book_indx, book[book_indx]);
            if (book[book_indx] & 7) { print_sub_book_name(); book_indx += 1; break; } // 5 normally
            book_indx += 1;
            assert( (book[book_indx] & 0xC0) == 0 );
        }
        printf("\n");

        search_moves(turn, last);
//        print_moves(depth);

//printf("%03x: %02x\n", book_indx, book[book_indx]);
        assert( (book[book_indx] & 0xC0) != 0xC0 );
        int move_indx = book[book_indx] & 0x3F;
if (move_indx == 0) { break; }
if (move_indx > nb_moves) printf("%02x > %02x !!!\n", move_indx, nb_moves);

        Move m = moves[move_indx-1];
        print_move(depth, m);
        do_move(m);
        if (flags == 0xC0) putchar('!'); // recommended move

        book_indx++;
        if (flags == 0x40) break;

        decode_variations(depth + 1, m);

    } while (flags & 0x80);
    // restore position
    memcpy(board, board_backup[depth], sizeof board);
    memcpy(piece_location, locations_backup[depth], sizeof piece_location);
    memcpy(piece_type, types_backup[depth], sizeof piece_type);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s opening_book_file\n", argv[0]);
        exit(1);
    }

    init_book(argv[1]);

/*
    skip_branch(book[book_indx] & 0xc0);
    printf("End at %03x\n", book_indx);
    book_indx = 0;
*/

    Move none = { 0, 0};
    init_board();
    //while (book[book_indx])
    {
        decode_variations(0, none);
    }
    printf("\nEnd at %03x\n", book_indx);
    return 0;
}
