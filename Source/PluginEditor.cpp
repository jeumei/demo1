#include "PluginEditor.h"

namespace
{
    const juce::Colour skyBlue { 0xff2f6fd8 };
    const juce::Colour dotYellow { 0xfff2c200 };

    void drawLayer (juce::Graphics& g, juce::Rectangle<float> frame, const Layer& layer)
    {
        const juce::Point<float> centre { frame.getX() + layer.x * frame.getWidth(),
                                          frame.getY() + layer.y * frame.getHeight() };
        const auto shade = [&] (juce::Colour c) { return c.withMultipliedBrightness (layer.brightness); };

        switch (layer.shape)
        {
            case Shape::Field:
                g.setColour (shade (juce::Colours::white));
                g.fillRect (juce::Rectangle<float> (frame.getWidth() * layer.scale,
                                                    frame.getHeight() * layer.scale).withCentre (centre));
                break;

            case Shape::Band:
                g.setColour (shade (skyBlue));
                g.fillRect (juce::Rectangle<float> (frame.getWidth() * 0.3f * layer.scale,
                                                    frame.getHeight() * 0.12f * layer.scale).withCentre (centre));
                break;

            case Shape::Dots:
            {
                static const juce::Point<float> offsets[] { { -0.16f,  0.02f }, { -0.08f, -0.06f }, { 0.0f, 0.03f },
                                                            {  0.08f, -0.05f }, {  0.16f,  0.01f } };
                const auto diameter = frame.getHeight() * 0.07f * layer.scale;

                g.setColour (shade (dotYellow));
                for (auto offset : offsets)
                {
                    const juce::Point<float> p { centre.x + offset.x * frame.getWidth() * layer.scale,
                                                 centre.y + offset.y * frame.getHeight() * layer.scale };
                    g.fillEllipse (juce::Rectangle<float> (diameter, diameter).withCentre (p));
                }
                break;
            }
        }
    }
}

void FrameView::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    g.fillAll (juce::Colours::black);

    for (const auto& layer : scene.layers)
        drawLayer (g, bounds, layer);

    auto caption = getLocalBounds().removeFromBottom (24);
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillRect (caption);
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (13.0f));
    g.drawText ("Test picture. Not the final artwork.", caption.reduced (8, 0), juce::Justification::centredLeft);
}

YITrialEditor::YITrialEditor (YITrialProcessor& p)
    : AudioProcessorEditor (p), trialProcessor (p), frame (p.scene)
{
    addAndMakeVisible (frame);

    layersLabel.setText ("Layers", juce::dontSendNotification);
    addAndMakeVisible (layersLabel);

    for (int i = 0; i < (int) p.scene.layers.size(); ++i)
    {
        auto* button = layerButtons.add (new juce::TextButton (p.scene.layers[(size_t) i].name));
        button->onClick = [this, i] { selectLayer (i); };
        addAndMakeVisible (button);
    }

    setUpSlider (scaleSlider, scaleLabel, "Scale", 0.25, 2.0, &Layer::scale);
    setUpSlider (brightnessSlider, brightnessLabel, "Brightness", 0.0, 1.0, &Layer::brightness);

    selectLayer (0);

    setSize (960, 440);
    startTimerHz (60);
}

YITrialEditor::~YITrialEditor()
{
    stopTimer();
}

void YITrialEditor::setUpSlider (juce::Slider& slider, juce::Label& label, const juce::String& name,
                                 double min, double max, float Layer::* field)
{
    label.setText (name, juce::dontSendNotification);
    addAndMakeVisible (label);

    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 20);
    slider.setRange (min, max, 0.01);
    slider.setDoubleClickReturnValue (true, 1.0);
    slider.onValueChange = [this, &slider, field]
    {
        trialProcessor.scene.layers[(size_t) selected].*field = (float) slider.getValue();
        frame.repaint();
    };
    addAndMakeVisible (slider);
}

void YITrialEditor::selectLayer (int index)
{
    selected = index;

    for (int i = 0; i < layerButtons.size(); ++i)
        layerButtons[i]->setToggleState (i == index, juce::dontSendNotification);

    const auto& layer = trialProcessor.scene.layers[(size_t) index];
    scaleSlider.setValue (layer.scale, juce::dontSendNotification);
    brightnessSlider.setValue (layer.brightness, juce::dontSendNotification);
}

void YITrialEditor::timerCallback()
{
    trialProcessor.applyAudioToScene();
    frame.repaint();
}

void YITrialEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void YITrialEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    auto panel = area.removeFromRight (280);
    area.removeFromRight (12);

    auto width = area.getWidth();
    auto height = width * 9 / 16;
    if (height > area.getHeight())
    {
        height = area.getHeight();
        width = height * 16 / 9;
    }
    frame.setBounds (area.withSizeKeepingCentre (width, height));

    layersLabel.setBounds (panel.removeFromTop (24));
    for (auto* button : layerButtons)
    {
        button->setBounds (panel.removeFromTop (30));
        panel.removeFromTop (4);
    }

    panel.removeFromTop (16);
    scaleLabel.setBounds (panel.removeFromTop (20));
    scaleSlider.setBounds (panel.removeFromTop (28));
    panel.removeFromTop (8);
    brightnessLabel.setBounds (panel.removeFromTop (20));
    brightnessSlider.setBounds (panel.removeFromTop (28));
}
