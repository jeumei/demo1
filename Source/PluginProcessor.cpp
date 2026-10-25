#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    constexpr float smoothingSeconds = 0.1f;
    constexpr float floorDb = -60.0f;
}

YITrialProcessor::YITrialProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void YITrialProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    smoothedLevel = 0.0f;
    level.store (0.0f);
}

bool YITrialProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == out;
}

void YITrialProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numInputs = getTotalNumInputChannels();
    const auto numSamples = buffer.getNumSamples();

    for (auto ch = numInputs; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    if (numInputs == 0 || numSamples == 0)
        return;

    // Read only: the audio itself is left untouched.
    float sumOfSquares = 0.0f;
    for (int ch = 0; ch < numInputs; ++ch)
    {
        const auto rms = buffer.getRMSLevel (ch, 0, numSamples);
        sumOfSquares += rms * rms;
    }

    const auto rms = std::sqrt (sumOfSquares / (float) numInputs);
    const auto db = juce::Decibels::gainToDecibels (rms, floorDb);
    const auto target = juce::jlimit (0.0f, 1.0f, juce::jmap (db, floorDb, 0.0f, 0.0f, 1.0f));

    const auto alpha = 1.0f - std::exp (-(float) numSamples / (float) (currentSampleRate * smoothingSeconds));
    smoothedLevel += alpha * (target - smoothedLevel);
    level.store (smoothedLevel);
}

void YITrialProcessor::applyAudioToScene()
{
    const auto& binding = scene.audio;
    scene.layers[(size_t) binding.layer].*binding.field = juce::jmap (level.load(), binding.atSilence, binding.atLoud);
}

juce::AudioProcessorEditor* YITrialProcessor::createEditor()
{
    return new YITrialEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YITrialProcessor();
}
