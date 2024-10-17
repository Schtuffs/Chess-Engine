#pragma once

// ----- Generic defines -----

#define GRID_SIZE       8
#define TOTAL_TEXTURES  12
#define CHAR_TO_INT     97
#define CODE_INVALID    0x7fff



// ----- Player Defines

#define PLAYER_TYPE_HUMAN       0b0000000000000001
#define PLAYER_TYPE_AI          0b0000000000000010

#define PLAYER_COLOUR_WHITE     0b0000000000001000
#define PLAYER_COLOUR_BLACK     0b0000000000010000

#define PLAYER_TYPE             (PLAYER_TYPE_AI | PLAYER_TYPE_HUMAN)
#define PLAYER_COLOUR           (PLAYER_COLOUR_BLACK | PLAYER_COLOUR_WHITE)



// ----- Window Defines -----

// Only affects the refreshing of the window, not the total speed of the program
// Allows for more calculations per second for everything else
#define WINDOW_MAX_FPS          120
#define WINDOW_SIZE_REGULAR     800
#define WINDOW_SIZE_MICRO       120



// ----- Piece defines -----

// Stores piece information and flags
typedef unsigned short PIECE;
// Must be at least as large as a piece
typedef unsigned short FLAG;
// Stores indexes for indexing grids
typedef short INDEX;

#define PIECE_INVALID           0x0
#define PIECE_PAWN              0x1
#define PIECE_KNIGHT            0x2
#define PIECE_BISHOP            0x3
#define PIECE_ROOK              0x4
#define PIECE_QUEEN             0x5
#define PIECE_KING              0x6
#define PIECE_PHANTOM           0x7

#define PIECE_WHITE             0x08
#define PIECE_BLACK             0x10

#define MASK_BLACK              0b0000000000010000
#define MASK_WHITE              0b0000000000001000
#define MASK_TYPE               0b0000000000000111
#define MASK_COLOUR             (MASK_BLACK | MASK_WHITE)
#define MASK_HELD               0b0000000000100000
#define MASK_MIN_FLAGS          (MASK_HELD | MASK_COLOUR | MASK_TYPE)
#define MASK_KING_IN_CHECK      0b1000000000000000

#define MASK_FLAG_1             0b0001000000000000
#define MASK_FLAG_2             0b0010000000000000
#define MASK_FLAG_3             0b0100000000000000
#define MASK_FLAG_4             0b1000000000000000
#define MASK_FLAG_ALL           (MASK_FLAG_1 | MASK_FLAG_2 | MASK_FLAG_3 | MASK_FLAG_4)



// ----- Board Defines -----

#define BOARD_BLACK_WHITE           0x30
#define BOARD_BROWN_BROWN           0x31
#define BOARD_GREEN_CREAM           0x32
#define BOARD_DBLUE_LBLUE           0x33
#define BOARD_RED_GOLD              0x34

#define BOARD_CASTLING_BLACK_KING   0x0
#define BOARD_CASTLING_BLACK_QUEEN  0x1
#define BOARD_CASTLING_WHITE_KING   0x2
#define BOARD_CASTLING_WHITE_QUEEN  0x3

constexpr char testFEN1[]    = "r2qk2r/Ppp2ppP/8/8/8/8/pPP2PPp/R2QK2R w KQkq - 0 1";
constexpr char testFEN2[]    = "8/PPPPPPPP/pppppppp/8/8/PPPPPPPP/pppppppp/8 w - - 0 1";

constexpr char startFEN[]   = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";



// ----- Move Defines -----

#define MOVE_CONTINUE           1
#define MOVE_END                2
#define MOVE_CAPTURE_KING       3

#define MOVE_KING_CAPTURED      true
#define MOVE_KING_NOT_CAPTURED  false

#define MASK_MOVE_START         0b0000000000111111
#define MASK_MOVE_TARGET        0b0000111111000000
#define MASK_MOVE_FLAGS         0b1111000000000000

// Piece specific move masks

#define MOVE_PAWN_FIRST_MOVE    0b0001000000000000
#define MOVE_PAWN_MOVE_TWO      0b0010000000000000
#define MOVE_PAWN_ATTACK        0b0100000000000000
#define MOVE_KING_CASTLE_KING   0b0001000000000000
#define MOVE_KING_CASTLE_QUEEN  0b0010000000000000
#define MOVE_KING_CASTLING      (MOVE_KING_CASTLE_KING | MOVE_KING_CASTLE_QUEEN)
#define MOVE_ROOK_CAN_CASTLE    0b0001000000000000
#define MOVE_CHECK              0b1000000000000000

