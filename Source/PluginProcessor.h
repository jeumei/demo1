#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "Scene.h"

class YITrialProcessor : public juce::AudioProcessor
{
public:
    YITrialProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    // Writes the smoothed audio level into the bound layer field. Message thread only.
    void applyAudioToScene();

    Scene scene = makeTestScene(); // message thread only

private:
    std::atomic<float> level { 0.0f }; // smoothed, 0 = silence, 1 = full scale
    float smoothedLevel = 0.0f;        // audio thread only
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YITrialProcessor)
};
