#include <cmath>

#include "pocketfft_hdronly.h"
#include "SpectrogramNode.h"

using namespace SpectrogramViewer;

//Change all names for the relevant ones, including "Processor Name"
SpectrogramNode::SpectrogramNode() : GenericProcessor("Spectrogram")
{
	setProcessorType(PROCESSOR_TYPE_SINK);
}

SpectrogramNode::~SpectrogramNode()
{
}

AudioProcessorEditor* SpectrogramNode::createEditor()
{
	editor = new SpectrogramEditor(this);
	return editor;
}

void SpectrogramNode::process(AudioSampleBuffer& buffer)
{
	if (selectedChannel < 0)
	{
		return;
	}

	int numInSamples = getNumSamples(selectedChannel);
	int samplesPerStep = fftInBuffer.size();
	auto channelData = buffer.getReadPointer(selectedChannel);
	int numTotalSamples = numInSamples + leftoverSamples;
	int numSteps = numTotalSamples / samplesPerStep;

	int fromInSample = 0;
	int toInSample = std::min(numInSamples, samplesPerStep - leftoverSamples);

	std::copy(&channelData[fromInSample], &channelData[toInSample], fftInBuffer.begin() + leftoverSamples);

	if (numSteps == 0)
	{
		// Can't do any calculations.
		leftoverSamples += numInSamples;
		return;
	}

	// Clear the space in the output vector.
	auto fromIndexToKeep = freqsPerSpectrogramColumn * numSteps;
	std::copy(spectrogram.begin() + fromIndexToKeep, spectrogram.end(), spectrogram.begin());

	// Compute the spectrgram of the input data.
	std::vector<float> fftOutBuffer(freqsPerSpectrogramColumn);
	auto fromOutIndex = spectrogram.size() - freqsPerSpectrogramColumn * numSteps;

	do
	{
		calcSpectrogram(fftInBuffer, fftOutBuffer, sqrtBandwidth);

		std::copy(
			fftOutBuffer.begin(),
			fftOutBuffer.begin() + freqsPerSpectrogramColumn,
			spectrogram.begin() + fromOutIndex);

		fromOutIndex += freqsPerSpectrogramColumn;

		// Prep the next input to calcSpectrogram().
		fromInSample = toInSample;
		toInSample += samplesPerStep;
		toInSample = std::min(toInSample, numInSamples);

		std::copy(&channelData[fromInSample], &channelData[toInSample], fftInBuffer.begin());

	} while (toInSample - fromInSample == samplesPerStep);

	// If all samples were processed, then after the last iteration of the 
	// loop above, we should have ended up with toInSample == fromInSample.
	// Otherwise, we'll have some leftover samples that should be combined with the
	// next batch of samples.
	leftoverSamples = toInSample - fromInSample;
}

void SpectrogramNode::setParameter(int paramIndex, float newValue)
{
	switch (paramIndex)
	{
	case PARAM_CHANNEL:
		selectedChannel = int(newValue);
		break;
	case PARAM_MAX_SHOWN_FREQ:
		maxShownFrequency = newValue;
		break;
	case PARAM_STEP_LENGTH_SEC:
		stepLengthSec = newValue;
		break;
	case PARAM_CHART_LENGTH_SEC:
		chartLengthSec = newValue;
		break;
	}

	resizeBuffers();
}

float SpectrogramNode::getParameter(int parameterIndex)
{
	switch (parameterIndex)
	{
	case PARAM_CHANNEL:
		return selectedChannel;
	case PARAM_MAX_SHOWN_FREQ:
		return maxShownFrequency;
	case PARAM_STEP_LENGTH_SEC:
		return stepLengthSec;
	case PARAM_CHART_LENGTH_SEC:
		return chartLengthSec;
	}

	return 0;
}

const String SpectrogramNode::getParameterName(int parameterIndex)
{
	switch (parameterIndex)
	{
	case PARAM_CHANNEL:
		return "PARAM_CHANNEL";
	case PARAM_MAX_SHOWN_FREQ:
		return "PARAM_MAX_SHOWN_FREQ";
	case PARAM_STEP_LENGTH_SEC:
		return "PARAM_STEP_LENGTH_SEC";
	case PARAM_CHART_LENGTH_SEC:
		return "PARAM_CHART_LENGTH_SEC";
	}

	return "";
}

bool SpectrogramNode::enable()
{
	return true;
}

bool SpectrogramNode::disable()
{
	return true;
}

void SpectrogramNode::resizeBuffers()
{
	if (selectedChannel < 0)
	{
		return;
	}

	auto sampleRate = getDataChannel(selectedChannel)->getSampleRate();
	int samplesPerStep = std::round(sampleRate * stepLengthSec);
	fftInBuffer.assign(samplesPerStep, 0);
	
	freqsPerSpectrogramColumn = std::floor(maxShownFrequency * stepLengthSec) + 1;
	sqrtBandwidth = std::sqrt(1 / stepLengthSec);

	int numStepsToShow = std::round(chartLengthSec / stepLengthSec);
	spectrogram.assign(numStepsToShow * freqsPerSpectrogramColumn, NAN);

	leftoverSamples = 0;
}

void SpectrogramNode::calcSpectrogram(
	const std::vector<float>& inBuf,
	std::vector<float>& outBuf,
	float sqrtBandwidth) const
{
	pocketfft::detail::shape_t shape_in{ inBuf.size() };
	pocketfft::detail::stride_t stride_in(1);
	pocketfft::detail::stride_t stride_out(1);

	std::vector<std::complex<float>> fftResult(inBuf.size() / 2 + 1);
	bool forward = true;
	pocketfft::detail::r2c(
		shape_in, stride_in, stride_out, 0, forward, inBuf.data(), fftResult.data(), 1 / sqrtBandwidth);

	for (int i = 0; i < outBuf.size(); i++)
	{
		outBuf[i] = std::abs(fftResult[i]);
	}
}