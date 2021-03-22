/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AutopannerAudioProcessor::AutopannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
    addParameter(gain = new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.5f));
    addParameter(mS = new juce::AudioParameterFloat("mS", "MilliSeconds", 10.0f, 5000.0f, 2490.0f));
}

AutopannerAudioProcessor::~AutopannerAudioProcessor()
{
}

//==============================================================================
const juce::String AutopannerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutopannerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AutopannerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AutopannerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AutopannerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutopannerAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int AutopannerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutopannerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AutopannerAudioProcessor::getProgramName(int index)
{
    return {};
}

void AutopannerAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void AutopannerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AutopannerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutopannerAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void AutopannerAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    auto *channeldataL = buffer.getWritePointer(0);
    auto *channeldataR = buffer.getWritePointer(1);

    float mSeconds = mS->get() / 1000;

    const float double_Pi = (2 * 3.1459);
    int numberSamples = getSampleRate() * mSeconds;
    const float radsPerSample = (2 * double_Pi) / numberSamples;

    float gSlider = gain->get();

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        auto inputL = channeldataL[i];
        auto inputR = channeldataR[i];

        // float sinValue = 0.5 * std::sin(nextRad) + 0.5;

        // inputL = inputL * (1 - sinValue);
        // inputR = inputR * sinValue;

        float sinValue = std::sin(nextRad) + 1;
        sinValue = (sinValue * double_Pi) / 4;

        inputL = inputL * cos(sinValue);
        inputR = inputR * sin(sinValue);

        channeldataL[i] = inputL;
        channeldataR[i] = inputR;

        nextRad += radsPerSample;
    }

    if (nextRad > numberSamples)
    {
        nextRad = 0;
    }
}

//==============================================================================
bool AutopannerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AutopannerAudioProcessor::createEditor()
{
    // return new AutopannerAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void AutopannerAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AutopannerAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AutopannerAudioProcessor();
}
