#include "polarity_channel_icon_widget.h"

PolarityChannelIconWidget::PolarityChannelIconWidget(TFT_eSPI *tft, uint8_t channel, icon_polarity polarity) : ChannelIconWidget(tft, channel)
{
	this->polarity = polarity;
}

PolarityChannelIconWidget::~PolarityChannelIconWidget(void)
{
}

const uint8_t* PolarityChannelIconWidget::iconCharArray(void)
{
	return (this->polarity) ? &IMG_POSITIVE_DATA[0] : &IMG_NEGATIVE_DATA[0];
}
