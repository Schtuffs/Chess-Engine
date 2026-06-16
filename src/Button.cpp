#include "Button.h"

#include "Utils.h"



// ----- Creation ----- Destruction -----

Button::Button(const std::string& text, FontData font, Rectangle dimensions, Color colour)
    : m_textPos({}), m_text(text), m_font(font), m_rect(dimensions), m_colour(colour), m_colourBorder(BLACK), m_borderThickness(1.f)
{
    m_textPos = Utils::CenterText(m_text.c_str(), m_font.font, m_font.fontSize, {m_rect.x + m_rect.width / 2, m_rect.y + m_rect.height / 2});
}

Button::~Button()
{}



// ----- Read -----

bool Button::IsClicked()
{
    return (CheckCollisionPointRec(GetMousePosition(), m_rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

bool Button::IsHovered()
{
    return CheckCollisionPointRec(GetMousePosition(), m_rect);
}

void Button::Render()
{
    DrawRectangleRec(m_rect, m_colour);
    DrawRectangleLinesEx(m_rect, m_borderThickness, m_colourBorder);

    DrawText(m_text.c_str(), m_textPos.x, m_textPos.y, m_font.fontSize, m_font.fontColour);
}

std::string Button::Text()
{
    return m_text;
}



// ----- Update -----

Button& Button::ColourInside(Color newColour)
{
    m_colour = newColour;
    return *this;
}

Button& Button::ColourBorder(Color newColour)
{
    m_colourBorder = newColour;
    return *this;
}

Button& Button::Dimension(Rectangle newDimensions)
{
    m_rect = newDimensions;
    m_textPos = Utils::CenterText(m_text.c_str(), m_font.font, m_font.fontSize, {m_rect.x + m_rect.width / 2, m_rect.y + m_rect.height / 2});
    return *this;
}

Button& Button::Font(FontData newFont)
{
    m_font = newFont;
    m_textPos = Utils::CenterText(m_text.c_str(), m_font.font, m_font.fontSize, {m_rect.x + m_rect.width / 2, m_rect.y + m_rect.height / 2});
    return *this;
}

Button& Button::Text(const std::string& newText)
{
    m_text = newText;
    m_textPos = Utils::CenterText(m_text.c_str(), m_font.font, m_font.fontSize, {m_rect.x + m_rect.width / 2, m_rect.y + m_rect.height / 2});
    return *this;
}

Button& Button::Thickness(float newThickness)
{
    m_borderThickness = newThickness;
    return *this;
}

