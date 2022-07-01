#pragma once

#include <VisualizerWindowHeaders.h>


namespace SpectrogramViewer
{

class SpectrogramNode;

class SpectrogramCanvas : public Visualizer
{
public:
	SpectrogramCanvas(SpectrogramNode* processor);
	~SpectrogramCanvas();

    /** Called when the component's tab becomes visible again.*/
    void refreshState() override;

    /** Called when parameters of underlying data processor are changed.*/
    void update() override;

    /** Called instead of "repaint" to avoid redrawing underlying components if not necessary.*/
    void refresh() override;

    /** Called when data acquisition is active.*/
    void beginAnimation() override;

    /** Called when data acquisition ends.*/
    void endAnimation() override;

    /** Called by an editor to initiate a parameter change.*/
    void setParameter(int, float) override {}

    /** Called by an editor to initiate a parameter change.*/
    void setParameter(int, int, int, float) override {}

private:
	SpectrogramNode* processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramCanvas);
};

}