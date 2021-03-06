/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Basic_OscillatorAudioProcessor::Basic_OscillatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
}

Basic_OscillatorAudioProcessor::~Basic_OscillatorAudioProcessor()
{
}

//==============================================================================
const juce::String Basic_OscillatorAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool Basic_OscillatorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool Basic_OscillatorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool Basic_OscillatorAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double Basic_OscillatorAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int Basic_OscillatorAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int Basic_OscillatorAudioProcessor::getCurrentProgram()
{
	return 0;
}

void Basic_OscillatorAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Basic_OscillatorAudioProcessor::getProgramName(int index)
{
	return {};
}

void Basic_OscillatorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Basic_OscillatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// initalizes the oscillator & assign values to attributes (Mine)
	juce::dsp::ProcessSpec mySpec;
	mySpec.maximumBlockSize = samplesPerBlock;
	mySpec.sampleRate = sampleRate;
	mySpec.numChannels = getTotalNumInputChannels();

	// prepares the oss
	osc.prepare(mySpec);
	gainKnob.prepare(mySpec);
	osc.setFrequency(220.0f);
	gainKnob.setGainLinear(0.01f);

}

void Basic_OscillatorAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Basic_OscillatorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void Basic_OscillatorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());


	juce::dsp::AudioBlock<float> audioBlock{ buffer };
	/* When we pass our audioblock in the method,
	   the result is going to replace whatever was in the audioblock/buffer before.
	   (mine)
	*/

	// After process is ran, audioBlock will contain the info our our sine wave (mine)
	osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
	// this then turns down the gain of the sine wave in audioBlock
	gainKnob.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

}
//==============================================================================
bool Basic_OscillatorAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Basic_OscillatorAudioProcessor::createEditor()
{
	return new Basic_OscillatorAudioProcessorEditor(*this);
}

//==============================================================================
void Basic_OscillatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void Basic_OscillatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new Basic_OscillatorAudioProcessor();
}
