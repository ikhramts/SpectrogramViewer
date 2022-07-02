#include <cstdio>

#include "SpectrogramNode.h"
#include "SpectrogramCanvas.h"

using namespace SpectrogramViewer;

SpectrogramCanvas::SpectrogramCanvas(SpectrogramNode* processor_)
	: processor(processor_)
{

}

SpectrogramCanvas::~SpectrogramCanvas()
{

}

void SpectrogramCanvas::resized()
{
}

void SpectrogramCanvas::refreshState()
{
}

void SpectrogramCanvas::update()
{
}

void SpectrogramCanvas::refresh()
{
    auto lastDataUpdateTime = processor->getLastDataUpdateTime();

    if (lastDataUpdateTime != drawnToTime)
    {
        drawnToTime = lastDataUpdateTime;
        //repaintChartOnly = true;
        repaint();
    }
}

void SpectrogramCanvas::beginAnimation()
{
    startTimer(20);
}

void SpectrogramCanvas::endAnimation()
{
    stopTimer();
}

void SpectrogramCanvas::paint(Graphics& g)
{
    // Repaint everything for now. There isn't that much extra that needs to be painted on every cycle.
	int numSpectrogramRows = processor->getNumFreqsPerSpectrigramColumn();
	int numSpectrogramColumns = processor->getNumSpectrogramColumns();

	int canvasWidth = getWidth();
	int canvasHeight = getHeight();

	int leftMargin = 60;
	int rightMargin = 90;
	int bottomMargin = 40;
	int topMargin = 40;

	int cellWidth = (canvasWidth - leftMargin - rightMargin) / numSpectrogramColumns;
	int cellHeight = (canvasHeight - topMargin - bottomMargin) / numSpectrogramRows;

	int chartLeft = leftMargin;
	int chartRight = chartLeft + cellWidth * numSpectrogramColumns;
	int chartBottom = canvasHeight - bottomMargin;
	int chartTop = chartBottom - cellHeight * numSpectrogramRows;

	int chartWidth = chartRight - chartLeft;
	int chartHeight = chartBottom - chartTop;

	// Clear the component.
    if (!repaintChartOnly)
    {
        g.setColour(Colours::black);
        g.fillRect(0, 0, getWidth(), getHeight());

        // Draw the axes
        g.setColour(Colours::lightgrey);
        g.drawLine(chartLeft - 1, chartTop, chartLeft - 1, chartBottom + 1);
        g.drawLine(chartLeft - 1, chartBottom + 1, chartRight, chartBottom + 1);

        // Draw X-axis ticks
        g.setFont(Font(Font::getDefaultSerifFontName(), 14, Font::plain));
        auto tickTextWidth = 40;
        auto tickTextHeight = 20;
        auto chartLengthSec = processor->getChartLengthSec();
        const int tickTextMaxLength = 20;
        char tickText[tickTextMaxLength];

        int numXTicks = 4;
        if (chartWidth > 800)
        {
            numXTicks = 8;
        }

        for (int i = 0; i <= numXTicks; i++)
        {
            int tickX = chartLeft - 1 + (chartRight - chartLeft + 1) * i / numXTicks;
            g.drawLine(tickX, chartBottom + 1, tickX, chartBottom + 6);

            auto tickValue = chartLengthSec * (numXTicks - i) / numXTicks;
            std::snprintf(tickText, tickTextMaxLength, "%.2f s", tickValue);
            auto tickTextTop = chartBottom + 11;
            auto tickTextLeft = tickX - tickTextWidth / 2;

            g.drawText(
                String(tickText), tickTextLeft, tickTextTop, 
                tickTextWidth, tickTextHeight, Justification::centredTop);
        }

        // Draw Y-axis ticks
        auto maxFreq = processor->getMaxShownFrequency();

        int numYTicks = 5;

        for (int i = 0; i <= numYTicks; i++)
        {
            int tickY = chartTop + (chartBottom + 1 - chartTop) * i / numYTicks;
            g.drawLine(chartLeft - 6, tickY, chartLeft - 1, tickY);

            auto tickValue = maxFreq * (numYTicks - i) / numYTicks;
            std::snprintf(tickText, tickTextMaxLength, "%.0f Hz", tickValue);
            auto tickTextLeft = chartLeft - 6 - tickTextWidth - 7;
            auto tickTextTop = tickY - tickTextHeight / 2;

            g.drawText(
                String(tickText), tickTextLeft, tickTextTop, 
                tickTextWidth, tickTextHeight, Justification::centredRight);
        }

        // Draw the scale.
        auto scaleCenterX = chartRight + 40;
        auto scaleWidth = 18;
        auto scaleLeft = scaleCenterX - scaleWidth / 2;
        auto scaleRight = scaleLeft + scaleWidth;
        auto scaleHeight = chartHeight;

        g.setColour(Colours::lightgrey);
        g.drawText(
            String("V/sqrt(Hz)"), 
            scaleCenterX - 30, chartTop - 30, 60, 20, Justification::centredBottom);

        for (int i = 0; i < scaleHeight; i++)
        {
            int y = chartTop + i;
            int colorIndex = (float)((scaleHeight - i - 1)) / scaleHeight * infernoColors.size();
            g.setColour(infernoColors[colorIndex]);
            g.drawLine(scaleLeft, y, scaleLeft + scaleWidth, y, 1);
        }

        g.setColour(Colours::lightgrey);
        g.drawLine(scaleLeft, chartTop - 1, scaleLeft, chartBottom + 1);
        g.drawLine(scaleRight, chartTop - 1, scaleRight, chartBottom + 1);
        g.drawLine(scaleLeft, chartTop - 1, scaleRight, chartTop - 1);
        g.drawLine(scaleLeft, chartBottom + 1, scaleRight, chartBottom + 1);

        // Draw the scale ticks.
        g.setColour(Colours::lightgrey);

        for (int i = 0; i < scaleTicks.size(); i++)
        {
            int tickY = chartTop + (float)i / (scaleTicks.size() - 1) * scaleHeight;
            g.drawLine(scaleRight, tickY, scaleRight + 5, tickY);

            auto tickTextLeft = scaleRight + 10;
            auto tickTextTop = tickY - tickTextHeight / 2;
            g.drawText(
                scaleTicks[i], tickTextLeft, tickTextTop, 
                tickTextWidth, tickTextHeight, Justification::centredLeft);
        }
    }
    else
    {
        repaintChartOnly = false;
    }


	// Draw the spectrogram body.
	auto& spectrogramValues = processor->getSpectrogram();
    Colour transparent(0.f, 0.f, 0.f, 0.f);
    g.setColour(transparent);

	for (int i = 0; i < spectrogramValues.size(); i++)
	{
		int col = i / numSpectrogramRows;
		int row = i % numSpectrogramRows;

		int x = chartLeft + col * cellWidth;
		int y = chartBottom - (row + 1) * cellHeight;
		auto value = spectrogramValues[i];
        
        FillType fill(colorMap(value));
        g.setFillType(fill);
        g.fillRect(x, y, cellWidth, cellHeight);
	}
}

void SpectrogramCanvas::timerCallback()
{
    refresh();
}

const Colour& SpectrogramCanvas::colorMap(float value) const
{
	float logValue;

    if (std::isnan(value))
    {
        return infernoColors[0];

    } else if (value < 1e-20)
	{
		logValue = -20;
	
	} else {
		logValue = std::log10(value);
	}

    auto numColors = infernoColors.size();
    float min = -7;
    float max = -1;
    int colorIndex = ((logValue - min) / (max - min) * (float)numColors);
    colorIndex = std::min(std::max(0, colorIndex), (int)numColors - 1);

    return infernoColors[colorIndex];
}

std::vector<Colour> SpectrogramCanvas::infernoColors = {
    Colour(0, 0, 4),
    Colour(1, 0, 5),
    Colour(1, 1, 6),
    Colour(1, 1, 8),
    Colour(2, 1, 10),
    Colour(2, 2, 12),
    Colour(2, 2, 14),
    Colour(3, 2, 16),
    Colour(4, 3, 18),
    Colour(4, 3, 20),
    Colour(5, 4, 23),
    Colour(6, 4, 25),
    Colour(7, 5, 27),
    Colour(8, 5, 29),
    Colour(9, 6, 31),
    Colour(10, 7, 34),
    Colour(11, 7, 36),
    Colour(12, 8, 38),
    Colour(13, 8, 41),
    Colour(14, 9, 43),
    Colour(16, 9, 45),
    Colour(17, 10, 48),
    Colour(18, 10, 50),
    Colour(20, 11, 52),
    Colour(21, 11, 55),
    Colour(22, 11, 57),
    Colour(24, 12, 60),
    Colour(25, 12, 62),
    Colour(27, 12, 65),
    Colour(28, 12, 67),
    Colour(30, 12, 69),
    Colour(31, 12, 72),
    Colour(33, 12, 74),
    Colour(35, 12, 76),
    Colour(36, 12, 79),
    Colour(38, 12, 81),
    Colour(40, 11, 83),
    Colour(41, 11, 85),
    Colour(43, 11, 87),
    Colour(45, 11, 89),
    Colour(47, 10, 91),
    Colour(49, 10, 92),
    Colour(50, 10, 94),
    Colour(52, 10, 95),
    Colour(54, 9, 97),
    Colour(56, 9, 98),
    Colour(57, 9, 99),
    Colour(59, 9, 100),
    Colour(61, 9, 101),
    Colour(62, 9, 102),
    Colour(64, 10, 103),
    Colour(66, 10, 104),
    Colour(68, 10, 104),
    Colour(69, 10, 105),
    Colour(71, 11, 106),
    Colour(73, 11, 106),
    Colour(74, 12, 107),
    Colour(76, 12, 107),
    Colour(77, 13, 108),
    Colour(79, 13, 108),
    Colour(81, 14, 108),
    Colour(82, 14, 109),
    Colour(84, 15, 109),
    Colour(85, 15, 109),
    Colour(87, 16, 110),
    Colour(89, 16, 110),
    Colour(90, 17, 110),
    Colour(92, 18, 110),
    Colour(93, 18, 110),
    Colour(95, 19, 110),
    Colour(97, 19, 110),
    Colour(98, 20, 110),
    Colour(100, 21, 110),
    Colour(101, 21, 110),
    Colour(103, 22, 110),
    Colour(105, 22, 110),
    Colour(106, 23, 110),
    Colour(108, 24, 110),
    Colour(109, 24, 110),
    Colour(111, 25, 110),
    Colour(113, 25, 110),
    Colour(114, 26, 110),
    Colour(116, 26, 110),
    Colour(117, 27, 110),
    Colour(119, 28, 109),
    Colour(120, 28, 109),
    Colour(122, 29, 109),
    Colour(124, 29, 109),
    Colour(125, 30, 109),
    Colour(127, 30, 108),
    Colour(128, 31, 108),
    Colour(130, 32, 108),
    Colour(132, 32, 107),
    Colour(133, 33, 107),
    Colour(135, 33, 107),
    Colour(136, 34, 106),
    Colour(138, 34, 106),
    Colour(140, 35, 105),
    Colour(141, 35, 105),
    Colour(143, 36, 105),
    Colour(144, 37, 104),
    Colour(146, 37, 104),
    Colour(147, 38, 103),
    Colour(149, 38, 103),
    Colour(151, 39, 102),
    Colour(152, 39, 102),
    Colour(154, 40, 101),
    Colour(155, 41, 100),
    Colour(157, 41, 100),
    Colour(159, 42, 99),
    Colour(160, 42, 99),
    Colour(162, 43, 98),
    Colour(163, 44, 97),
    Colour(165, 44, 96),
    Colour(166, 45, 96),
    Colour(168, 46, 95),
    Colour(169, 46, 94),
    Colour(171, 47, 94),
    Colour(173, 48, 93),
    Colour(174, 48, 92),
    Colour(176, 49, 91),
    Colour(177, 50, 90),
    Colour(179, 50, 90),
    Colour(180, 51, 89),
    Colour(182, 52, 88),
    Colour(183, 53, 87),
    Colour(185, 53, 86),
    Colour(186, 54, 85),
    Colour(188, 55, 84),
    Colour(189, 56, 83),
    Colour(191, 57, 82),
    Colour(192, 58, 81),
    Colour(193, 58, 80),
    Colour(195, 59, 79),
    Colour(196, 60, 78),
    Colour(198, 61, 77),
    Colour(199, 62, 76),
    Colour(200, 63, 75),
    Colour(202, 64, 74),
    Colour(203, 65, 73),
    Colour(204, 66, 72),
    Colour(206, 67, 71),
    Colour(207, 68, 70),
    Colour(208, 69, 69),
    Colour(210, 70, 68),
    Colour(211, 71, 67),
    Colour(212, 72, 66),
    Colour(213, 74, 65),
    Colour(215, 75, 63),
    Colour(216, 76, 62),
    Colour(217, 77, 61),
    Colour(218, 78, 60),
    Colour(219, 80, 59),
    Colour(221, 81, 58),
    Colour(222, 82, 56),
    Colour(223, 83, 55),
    Colour(224, 85, 54),
    Colour(225, 86, 53),
    Colour(226, 87, 52),
    Colour(227, 89, 51),
    Colour(228, 90, 49),
    Colour(229, 92, 48),
    Colour(230, 93, 47),
    Colour(231, 94, 46),
    Colour(232, 96, 45),
    Colour(233, 97, 43),
    Colour(234, 99, 42),
    Colour(235, 100, 41),
    Colour(235, 102, 40),
    Colour(236, 103, 38),
    Colour(237, 105, 37),
    Colour(238, 106, 36),
    Colour(239, 108, 35),
    Colour(239, 110, 33),
    Colour(240, 111, 32),
    Colour(241, 113, 31),
    Colour(241, 115, 29),
    Colour(242, 116, 28),
    Colour(243, 118, 27),
    Colour(243, 120, 25),
    Colour(244, 121, 24),
    Colour(245, 123, 23),
    Colour(245, 125, 21),
    Colour(246, 126, 20),
    Colour(246, 128, 19),
    Colour(247, 130, 18),
    Colour(247, 132, 16),
    Colour(248, 133, 15),
    Colour(248, 135, 14),
    Colour(248, 137, 12),
    Colour(249, 139, 11),
    Colour(249, 140, 10),
    Colour(249, 142, 9),
    Colour(250, 144, 8),
    Colour(250, 146, 7),
    Colour(250, 148, 7),
    Colour(251, 150, 6),
    Colour(251, 151, 6),
    Colour(251, 153, 6),
    Colour(251, 155, 6),
    Colour(251, 157, 7),
    Colour(252, 159, 7),
    Colour(252, 161, 8),
    Colour(252, 163, 9),
    Colour(252, 165, 10),
    Colour(252, 166, 12),
    Colour(252, 168, 13),
    Colour(252, 170, 15),
    Colour(252, 172, 17),
    Colour(252, 174, 18),
    Colour(252, 176, 20),
    Colour(252, 178, 22),
    Colour(252, 180, 24),
    Colour(251, 182, 26),
    Colour(251, 184, 29),
    Colour(251, 186, 31),
    Colour(251, 188, 33),
    Colour(251, 190, 35),
    Colour(250, 192, 38),
    Colour(250, 194, 40),
    Colour(250, 196, 42),
    Colour(250, 198, 45),
    Colour(249, 199, 47),
    Colour(249, 201, 50),
    Colour(249, 203, 53),
    Colour(248, 205, 55),
    Colour(248, 207, 58),
    Colour(247, 209, 61),
    Colour(247, 211, 64),
    Colour(246, 213, 67),
    Colour(246, 215, 70),
    Colour(245, 217, 73),
    Colour(245, 219, 76),
    Colour(244, 221, 79),
    Colour(244, 223, 83),
    Colour(244, 225, 86),
    Colour(243, 227, 90),
    Colour(243, 229, 93),
    Colour(242, 230, 97),
    Colour(242, 232, 101),
    Colour(242, 234, 105),
    Colour(241, 236, 109),
    Colour(241, 237, 113),
    Colour(241, 239, 117),
    Colour(241, 241, 121),
    Colour(242, 242, 125),
    Colour(242, 244, 130),
    Colour(243, 245, 134),
    Colour(243, 246, 138),
    Colour(244, 248, 142),
    Colour(245, 249, 146),
    Colour(246, 250, 150),
    Colour(248, 251, 154),
    Colour(249, 252, 157),
    Colour(250, 253, 161),
    Colour(252, 255, 164)
};

std::vector<String> SpectrogramCanvas::scaleTicks = {
    String("100m"),
    String("10m"),
    String("1m"),
    String("100u"),
    String("10u"),
    String("1u"),
    String("100n")
};