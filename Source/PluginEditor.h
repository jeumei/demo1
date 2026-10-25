#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// The 16:9 picture. Draws the scene's layers back to front.
class FrameView : public juce::Component
{
public:
    explicit FrameView (const Scene& s) : scene (s) {}

    void paint (juce::Graphics&) override;

private:
    const Scene& scene;
};

class YITrialEditor : public juce::AudioProcessorEditor,
                      private juce::Timer
{
public:
    explicit YITrialEditor (YITrialProcessor&);
    ~YITrialEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void selectLayer (int index);
    void setUpSlider (juce::Slider&, juce::Label&, const juce::String& name,
                      double min, double max, float Layer::* field);

    YITrialProcessor& trialProcessor;
    FrameView frame;

    juce::Label layersLabel;
    juce::OwnedArray<juce::TextButton> layerButtons;
    juce::Label scaleLabel, brightnessLabel;
    juce::Slider scaleSlider, brightnessSlider;

    int selected = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YITrialEditor)
};
