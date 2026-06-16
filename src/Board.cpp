#include "Board.h"



// ----- Creation ----- Destruction -----

Board::Board(const std::string& fen)
    : m_fen(fen)
{}

Board::~Board()
{
    // Nothing todo
}



// ----- Read -----

std::string Board::Fen()
{
    return m_fen;
}



// ----- Update -----

bool Board::MakeMove(std::string_view move)
{
    (void)move;
    
    return true;
}

