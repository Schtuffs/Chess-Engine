#include "Piece.h"

#include "Utils.h"

Piece::Piece(Enums::Colour colour, Enums::Type type)
    : m_colour(colour), m_type(type), m_isSetup(false)
{}

Piece::~Piece()
{}

