#pragma once

#include <VisualizerWindowHeaders.h>


namespace SpectrogramViewer
{

class SpectrogramNode;

class SpectrogramCanvas 
    : public Visualizer
{
public:
	SpectrogramCanvas(SpectrogramNode* processor);
	~SpectrogramCanvas();

    void resized() override;

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

    /** Components can override this method to draw their content.

        The paint() method gets called when a region of a component needs redrawing,
        either because the component's repaint() method has been called, or because
        something has happened on the screen that means a section of a window needs
        to be redrawn.

        Any child components will draw themselves over whatever this method draws. If
        you need to paint over the top of your child components, you can also implement
        the paintOverChildren() method to do this.

        If you want to cause a component to redraw itself, this is done asynchronously -
        calling the repaint() method marks a region of the component as "dirty", and the
        paint() method will automatically be called sometime later, by the message thread,
        to paint any bits that need refreshing. In Juce (and almost all modern UI frameworks),
        you never redraw something synchronously.

        You should never need to call this method directly - to take a snapshot of the
        component you could use createComponentSnapshot() or paintEntireComponent().

        @param g    the graphics context that must be used to do the drawing operations.
        @see repaint, paintOverChildren, Graphics
    */
    void paint(Graphics& g) override;

    void timerCallback() override;


private:
	SpectrogramNode* processor;
    const Colour& colorMap(float value) const;

    static std::vector<Colour> infernoColors;
    static std::vector<String> scaleTicks;

    int64 drawnToTime = 0;
    bool repaintChartOnly = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramCanvas);
};

}