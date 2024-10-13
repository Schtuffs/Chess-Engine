#pragma once

// ----- Library defines -----

#define GRID_SIZE       8
#define TOTAL_TEXTURES  12
#define CHAR_TO_INT     97
#define CODE_INVALID    -1



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
#define PIECE_BLOCK     0x7
#define PIECE_PHANTOM   0x7FFF

#define PIECE_WHITE     0x08
#define PIECE_BLACK     0x10

#define MASK_BLACK              0b0000000000010000
#define MASK_WHITE              0b0000000000001000
#define MASK_TYPE               0b0000000000000111
#define MASK_COLOUR             (MASK_BLACK | MASK_WHITE)
#define MASK_HELD               0b0000000000100000
#define MASK_MIN_FLAGS          (MASK_HELD | MASK_COLOUR | MASK_TYPE)
#define MASK_PINNED             0b0000000001000000

#define MASK_FLAG_1             0b0001000000000000
#define MASK_FLAG_2             0b0010000000000000

// Piece specific masks

#define MASK_PAWN_FIRST_MOVE    0b0001000000000000
#define MASK_PAWN_EN_PASSENT    0b0010000000000000
#define MASK_KING_CASTLE_KING   0b0001000000000000
#define MASK_KING_CASTLE_QUEEN  0b0010000000000000
#define MASK_KING_CASTLING      (MASK_KING_CASTLE_KING | MASK_KING_CASTLE_QUEEN)
#define MASK_ROOK_CAN_CASTLE    0b0010000000000000




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

constexpr char testFEN1[]    = "r2qk2r/Ppp2ppP/8/8/8/8/pPP2PPp/R2QK2R w KQkq - 0 1";
constexpr char testFEN2[]    = "8/PPPPPPPP/pppppppp/8/8/PPPPPPPP/pppppppp/8 w - - 0 1";

constexpr char startFEN[]   = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";



// ----- Move Defines -----

#define MOVE_CONTINUE       1
#define MOVE_END            2
#define MOVE_CAPTURE_KING   3

#define MOVE_KING_CAPTURED      true
#define MOVE_KING_NOT_CAPTURED  false

#define MASK_MOVE_START          0b0000000000111111
#define MASK_MOVE_TARGET         0b0000111111000000
#define MASK_MOVE_FLAGS          0b1111000000000000

 

