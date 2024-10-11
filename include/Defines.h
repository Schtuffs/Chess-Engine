#pragma once

// ----- Library defines -----

// Allows for bigger grids, performance costs will increase
#define GRID_SIZE       8
#define TOTAL_TEXTURES  12
#define CHAR_TO_INT     97

// ----- Piece defines -----

// Stores piece information and flags
typedef int PIECE;
// Same type as piece, but used differently
typedef PIECE FLAG;

#define PIECE_INVALID   0x0
#define PIECE_PAWN      0x1
#define PIECE_KNIGHT    0x2
#define PIECE_BISHOP    0x3
#define PIECE_ROOK      0x4
#define PIECE_QUEEN     0x5
#define PIECE_KING      0x6
#define PIECE_PHANTOM   0x7FFF

#define PIECE_WHITE     0x08
#define PIECE_BLACK     0x10

#define MASK_BLACK              0b000010000
#define MASK_WHITE              0b000001000
#define MASK_TYPE               0b000000111
#define MASK_COLOUR             (MASK_BLACK | MASK_WHITE)
#define MASK_HELD               0b000100000
#define MASK_MIN_FLAGS          (MASK_HELD | MASK_COLOUR | MASK_TYPE)
#define MASK_PINNED             0b001000000

#define MASK_FLAG_1             0b010000000
#define MASK_FLAG_2             0b100000000

// Piece specific masks

#define MASK_PAWN_FIRST_MOVE    0b010000000
#define MASK_PAWN_EN_PASSENT    0b100000000
#define MASK_KING_CASTLE_KING   0b010000000
#define MASK_KING_CASTLE_QUEEN  0b100000000
#define MASK_KING_CASTLING      (MASK_KING_CASTLE_KING | MASK_KING_CASTLE_QUEEN)
#define MASK_ROOK_CAN_CASTLE    0b010000000




// ----- Board Defines -----

typedef int INDEX;

#define BOARD_BLACK_WHITE           0x30
#define BOARD_BROWN_BROWN           0x31
#define BOARD_GREEN_CREAM           0x32
#define BOARD_DBLUE_LBLUE           0x33
#define BOARD_RED_GOLD              0x34

#define TURN_WHITE                  0x101
#define TURN_BLACK                  0x102

#define BOARD_CASTLING_BLACK_KING   0x0
#define BOARD_CASTLING_BLACK_QUEEN  0x1
#define BOARD_CASTLING_WHITE_KING   0x2
#define BOARD_CASTLING_WHITE_QUEEN  0x3

constexpr char startFEN[]   = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr char startFEN12[] = "rnbnbqkbnbnr/pppppppppppp/12/12/12/12/12/12/12/12/PPPPPPPPPPPP/RNBNBQKBNBNR w KQkq - 0 1";
// Third row pawns CANNOT double move, knights blocked at start
constexpr char startFEN16[] = "rnbrnbrqkrbnrbnr/pppppppppppppppp/p1pp1pp11pp1pp1p/16/16/16/16/16/16/16/16/16/16/P1PP1PP11PP1PP1P/PPPPPPPPPPPPPPPP/RNBRNBRQKRBNRBNR w KQkq - 0 1";



// ----- Move Defines -----

#define MOVE_CONTINUE   1
#define MOVE_END        2

#define MASK_FLAGS      0b111000000

