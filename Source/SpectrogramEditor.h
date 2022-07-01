#pragma once

#include <VisualizerEditorHeaders.h>

class Visualizer;

namespace SpectrogramViewer
{

class SpectrogramNode;

class SpectrogramEditor
    : public VisualizerEditor
    , public ComboBox::Listener
    , Label::Listener
{
public:
	SpectrogramEditor(SpectrogramNode*);
	~SpectrogramEditor();

	void buttonEvent(Button* button) override;
    void comboBoxChanged(ComboBox* comboBox) override;
    void labelTextChanged(Label* label) override;

    /** Called by the base class VisualizerEditor to display the canvas
        when the user chooses to display one

        @see VisualizerEditor::buttonClicked
     */
    Visualizer* createNewCanvas() override;

    /** Called by the update() method to allow the editor to update its custom settings.*/
    virtual void updateSettings() override;

private:
    ScopedPointer<Label> channelLabel;
    ScopedPointer<ComboBox> channelSelector;
    
    String lastMaxFreqString;
    ScopedPointer<Label> maxFreqLabel;
    ScopedPointer<Label> maxFreqTextbox;
    ScopedPointer<Label> maxFreqUnitLabel;

    String lastStepLengthString;
    ScopedPointer<Label> stepLengthLabel;
    ScopedPointer<Label> stepLengthTextbox;
    ScopedPointer<Label> stepLengthUnitLabel;

    String lastChartLengthString;
    ScopedPointer<Label> chartLengthLabel;
    ScopedPointer<Label> chartLengthTextbox;
    ScopedPointer<Label> chartLengthUnitLabel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramEditor);
};




}