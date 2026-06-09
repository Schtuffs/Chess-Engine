#include "Piece.h"

#include "Utils.h"

Piece::Piece(Enums::Colour colour, Enums::Type type)
    : m_colour(colour), m_type(type), m_isSetup(false)
{
    // Load piece texture
    m_texture = Utils::LoadTexture(m_colour, m_type);

    // Check
    if (IsTextureValid(m_texture)) {
        m_isSetup = true;
    }
}

Piece::~Piece() {
    if (m_isSetup) {
        Utils::UnloadTexture(m_texture, m_colour, m_type);
    }
}

void Piece::Render(int x, int y) {
    DrawTexture(m_texture, x, y, WHITE);
}

