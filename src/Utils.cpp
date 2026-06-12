#include "Utils.h"

#include <cstdio>

typedef struct TextureValuePair {
    char count = 0;
    Texture2D texture {};
} TextureValuePair;

static TextureValuePair textureValuePairs[12];

int CalculateIndex(Enums::Colour colour, Enums::Type type)
{
    return type * 2 + colour;
}

Texture2D Utils::LoadTexture(Enums::Colour colour, Enums::Type type, int size)
{
    // Check size
    if (size < 1) {
        TraceLog(LOG_ERROR, "Utils::LoadTexture received invalid size: %d", size);
        return Texture2D{};
    }
    
    // Get pair data
    int index = CalculateIndex(colour, type);
    TextureValuePair& pair = textureValuePairs[index];

    // Determine if should load texture
    if (pair.count == 0) {
        // Load and check image
        char path[50];
        snprintf(path, sizeof(path), "%s/%s_%s.png", PATH_PIECES, Enums::ToString::Type[type], Enums::ToString::Colour[colour]);
        Image image = LoadImage(path);
        if (!IsImageValid(image)) {
            TraceLog(LOG_ERROR, "Utils::LoadTexture Failed to load image.");
            return Texture2D{};
        }
        
        // Change image size and load to texture
        ImageResizeNN(&image, size, size);
        Texture2D texture = LoadTextureFromImage(image);
        if (!IsTextureValid(texture)) {
            TraceLog(LOG_ERROR, "Utils::LoadTexture Failed to convert image to texture.");
            return Texture2D{};
        }

        pair.texture = texture;
        UnloadImage(image);
        SetTextureFilter(pair.texture, TEXTURE_FILTER_POINT);
    }

    // Account and return
    pair.count++;
    return pair.texture;
}

void Utils::UnloadTexture(Texture2D& texture, Enums::Colour colour, Enums::Type type)
{
    if (IsTextureValid(texture)) {
        ::UnloadTexture(texture);
        texture.id = 0;
        textureValuePairs[CalculateIndex(colour, type)].count--;
    }
}

