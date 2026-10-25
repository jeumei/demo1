#pragma once

#include <juce_core/juce_core.h>
#include <vector>

enum class Shape { Field, Band, Dots };

// One layer of the picture. x and y are the layer's centre as a fraction of the frame.
struct Layer
{
    juce::String name;
    Shape shape;
    float x = 0.5f;
    float y = 0.5f;
    float scale = 1.0f;
    float brightness = 1.0f;
};

// Which layer field the audio level drives, and the value at silence and at full level.
struct AudioBinding
{
    int layer;
    float Layer::* field;
    float atSilence;
    float atLoud;
};

struct Scene
{
    std::vector<Layer> layers; // back to front, fixed order
    AudioBinding audio;
};

// Temporary test picture. When the real artwork arrives, replace the layers and the binding here.
inline Scene makeTestScene()
{
    Scene scene;
    scene.layers = {
        { "Background", Shape::Field, 0.5f, 0.5f },
        { "Sky",        Shape::Band,  0.2f, 0.3f },
        { "Yellow",     Shape::Dots,  0.5f, 0.7f },
    };
    scene.audio = { 1, &Layer::x, 0.2f, 0.8f };
    return scene;
}
