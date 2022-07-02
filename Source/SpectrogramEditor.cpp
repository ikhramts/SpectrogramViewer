#include "SpectrogramEditor.h"
#include "SpectrogramCanvas.h"
#include "SpectrogramNode.h"

using namespace SpectrogramViewer;

SpectrogramEditor::SpectrogramEditor(SpectrogramNode* processor)
	:VisualizerEditor(processor, false)
{

	tabText = "Spectrogram";
	desiredWidth = 205;

	lastMaxFreqString = String(roundFloatToInt(processor->getMaxShownFrequency()));
	lastStepLengthString = String(roundFloatToInt(processor->getStepLengthSec() * 1000));
	lastChartLengthString = String(roundFloatToInt(processor->getChartLengthSec() * 1000));

	// Channel picker
	channelLabel = new Label("ChannelLabel", "Channel");
	channelLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	channelLabel->setBounds(10, 25, 85, 20);
	channelLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(channelLabel);

	channelSelector = new ComboBox("Channel ComboBox");
	channelSelector->setBounds(105, 25, 55, 22);
	channelSelector->addListener(this);
	channelSelector->addItem("-", 1);
	channelSelector->setSelectedId(1, dontSendNotification);
	addAndMakeVisible(channelSelector);

	// Max frequency textbox
	maxFreqLabel = new Label("maxFreqLabel", "Max frequency");
	maxFreqLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	maxFreqLabel->setBounds(10, 50, 85, 20);
	maxFreqLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(maxFreqLabel);

	maxFreqTextbox = new Label("maxFreqTextbox", lastMaxFreqString);
	maxFreqTextbox->setBounds(105, 50, 55, 22);
	maxFreqTextbox->addListener(this);
	maxFreqTextbox->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	maxFreqTextbox->setColour(Label::textColourId, Colours::black);
	maxFreqTextbox->setColour(Label::backgroundColourId, Colours::lightgrey);
	maxFreqTextbox->setEditable(true);
	maxFreqTextbox->setTooltip("Maximum frequency to display on the spectrogram");
	addAndMakeVisible(maxFreqTextbox);

	maxFreqUnitLabel = new Label("maxFreqUnitLabel", "Hz");
	maxFreqUnitLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	maxFreqUnitLabel->setBounds(160, 50, 25, 20);
	maxFreqUnitLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(maxFreqUnitLabel);

	// Step length textbox
	stepLengthLabel = new Label("stepLengthLabel", "Step length");
	stepLengthLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	stepLengthLabel->setBounds(10, 75, 85, 20);
	stepLengthLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(stepLengthLabel);

	stepLengthTextbox = new Label("stepLengthTextbox", lastStepLengthString);
	stepLengthTextbox->setBounds(105, 75, 55, 22);
	stepLengthTextbox->addListener(this);
	stepLengthTextbox->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	stepLengthTextbox->setColour(Label::textColourId, Colours::black);
	stepLengthTextbox->setColour(Label::backgroundColourId, Colours::lightgrey);
	stepLengthTextbox->setEditable(true);
	stepLengthTextbox->setTooltip("Time step in each spectrogram vertical bar");
	addAndMakeVisible(stepLengthTextbox);

	stepLengthUnitLabel = new Label("stepLengthUnitLabel", "ms");
	stepLengthUnitLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	stepLengthUnitLabel->setBounds(160, 75, 25, 20);
	stepLengthUnitLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(stepLengthUnitLabel);

	// Chart length textbox
	chartLengthLabel = new Label("chartLengthLabel", "Chart history");
	chartLengthLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	chartLengthLabel->setBounds(10, 100, 85, 20);
	chartLengthLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(chartLengthLabel);

	chartLengthTextbox = new Label("chartLengthTextbox", lastChartLengthString);
	chartLengthTextbox->setBounds(105, 100, 55, 22);
	chartLengthTextbox->addListener(this);
	chartLengthTextbox->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	chartLengthTextbox->setColour(Label::textColourId, Colours::black);
	chartLengthTextbox->setColour(Label::backgroundColourId, Colours::lightgrey);
	chartLengthTextbox->setEditable(true);
	chartLengthTextbox->setTooltip("Length of the displayed spectrogram history");
	addAndMakeVisible(chartLengthTextbox);

	chartLengthUnitLabel = new Label("chartLengthUnitLabel", "ms");
	chartLengthUnitLabel->setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
	chartLengthUnitLabel->setBounds(160, 100, 25, 20);
	chartLengthUnitLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(chartLengthUnitLabel);

}

SpectrogramEditor::~SpectrogramEditor()
{
}

// Not used yet...
void SpectrogramEditor::buttonEvent(Button* button)
{

}

void SpectrogramEditor::comboBoxChanged(ComboBox* comboBox)
{
	if (comboBox == channelSelector)
	{
		int channelNum = channelSelector->getSelectedId() - 2;
		getProcessor()->setParameter(SpectrogramNode::PARAM_CHANNEL, channelNum);
	}
}

void SpectrogramEditor::labelTextChanged(Label* label)
{
	auto processor = (SpectrogramNode*)getProcessor();
	auto rawValue = label->getTextValue();
	auto value = float(rawValue.getValue());

	if (label == maxFreqTextbox)
	{
		if (value < 2 || value > 1000)
		{
			CoreServices::sendStatusMessage("Max spectrogram frequency out of range.");
			label->setText(lastMaxFreqString, dontSendNotification);
			return;
		}

		processor->setParameter(SpectrogramNode::PARAM_MAX_SHOWN_FREQ, value);
		lastMaxFreqString = label->getText();
		return;
	}

	if (label == stepLengthTextbox)
	{
		if (value < 2 || value > 1000)
		{
			CoreServices::sendStatusMessage("Spectrogram step length out of range.");
			label->setText(lastStepLengthString, dontSendNotification);
			return;
		}

		processor->setParameter(SpectrogramNode::PARAM_STEP_LENGTH_SEC, value / 1000);
		lastStepLengthString = label->getText();
		return;
	}

	if (label == chartLengthTextbox)
	{
		if (value < 100 || value > 30000)
		{
			CoreServices::sendStatusMessage("Spectrogram chart length out of range.");
			label->setText(lastChartLengthString, dontSendNotification);
			return;
		}

		processor->setParameter(SpectrogramNode::PARAM_CHART_LENGTH_SEC, value / 1000);
		lastChartLengthString = label->getText();
		return;
	}
}

Visualizer* SpectrogramEditor::createNewCanvas()
{
	auto processor = (SpectrogramNode*)getProcessor();
	canvas = new SpectrogramCanvas(processor);
	return canvas;
}


void SpectrogramEditor::updateSettings()
{
	auto processor = getProcessor();
	
	channelSelector->clear();
	channelSelector->addItem("-", 1);

	for (int i = 0; i < processor->getTotalDataChannels(); i++)
	{
		channelSelector->addItem(String(i + 1), i + 2);
	}

	if (processor->getTotalDataChannels() > 0)
	{
		channelSelector->setSelectedId(2, sendNotification);
	}
}
