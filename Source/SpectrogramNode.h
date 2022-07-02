//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#pragma once

#include <vector>

#include <ProcessorHeaders.h>
#include "SpectrogramEditor.h"

//namespace must be an unique name for your plugin
namespace SpectrogramViewer
{
	class SpectrogramNode : public GenericProcessor
	{
	public:
		static const int PARAM_CHANNEL = 0;
		static const int PARAM_MAX_SHOWN_FREQ = 1;
		static const int PARAM_STEP_LENGTH_SEC = 2;
		static const int PARAM_CHART_LENGTH_SEC = 3;

		/** The class constructor, used to initialize any members. */
		SpectrogramNode();

		/** The class destructor, used to deallocate memory */
		~SpectrogramNode();

		/** Indicates if the processor has a custom editor. Defaults to false */
		bool hasEditor() const { return true; }

		/** If the processor has a custom editor, this method must be defined to instantiate it. */
		AudioProcessorEditor* createEditor() override;

		/** Called immediately prior to the start of data acquisition, once all processors in the signal chain have indicated they are ready to process data.*/
		virtual bool enable() override;

		/** Called immediately after the end of data acquisition.*/
		virtual bool disable() override;

		/** Returns true if a processor is ready to process data (e.g., all of its parameters are initialized, and its data source is connected).*/
		virtual bool isReady() override { return selectedChannel >= 0; }

		/** Defines the functionality of the processor.

		The process method is called every time a new data buffer is available.

		Processors can either use this method to add new data, manipulate existing
		data, or send data to an external target (such as a display or other hardware).

		Continuous signals arrive in the "buffer" variable, event data (such as TTLs
		and spikes) is contained in the "events" variable.
		*/
		void process(AudioSampleBuffer& buffer) override;

		/** Handles events received by the processor

		Called automatically for each received event whenever checkForEvents() is called from process()		
		*/
		//void handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition) override;

		/** Handles spikes received by the processor

		Called automatically for each received event whenever checkForEvents(true) is called from process()
		*/
		//void handleSpike(const SpikeChannel* spikeInfo, const MidiMessage& event, int samplePosition) override;

		/** The method that standard controls on the editor will call.
		It is recommended that any variables used by the "process" function
		are modified only through this method while data acquisition is active. */
		void setParameter(int parameterIndex, float newValue) override;

		/** Returns the current value of a parameter with a given index.
		Currently set to always return 1. See getParameterVar below*/
		float getParameter(int parameterIndex) override;

		/** Returns the number of user-editable parameters for this processor.*/
		int getNumParameters() override { return 4; }

		/** Returns the name of the parameter with a given index.*/
		const String getParameterName(int parameterIndex) override;

		/** Saving custom settings to XML. */
		//void saveCustomParametersToXml(XmlElement* parentElement) override;

		/** Load custom settings from XML*/
		//void loadCustomParametersFromXml() override;

		/** Optional method called every time the signal chain is refreshed or changed in any way.

		Allows the processor to handle variations in the channel configuration or any other parameter
		passed down the signal chain. The processor can also modify here the settings structure, which contains
		information regarding the input and output channels as well as other signal related parameters. Said
		structure shouldn't be manipulated outside of this method.

		*/
		//void updateSettings() override;

		float getMaxShownFrequency() const { return maxShownFrequency; }
		float getStepLengthSec() const { return stepLengthSec; }
		float getChartLengthSec() const { return chartLengthSec; }

		const std::vector<float>& getSpectrogram() const { return spectrogram; }
		int getNumFreqsPerSpectrigramColumn() const { return freqsPerSpectrogramColumn; }
		int getNumSpectrogramColumns() const { return chartLengthSec / stepLengthSec; }
		int64 getLastDataUpdateTime() const { return lastDataUpdateTime; }

	private:
		int selectedChannel;
		float maxShownFrequency = 300;
		float stepLengthSec = 0.1;
		float chartLengthSec = 5;

		std::vector<float> fftInBuffer;
		int leftoverSamples = 0;

		std::vector<float> spectrogram;
		int freqsPerSpectrogramColumn;
		float sqrtBandwidth;

		int64 lastDataUpdateTime = 0;

		void resizeBuffers();

		void calcSpectrogram(
			const std::vector<float>& inBuf,
			std::vector<float>& outBuf,
			float sqrtBandwidth) const;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramNode);
	};
}
