#include "channel_icon_widget.h"

ChannelIconWidget::ChannelIconWidget(TFT_eSPI *tft, uint8_t channel) : BaseIconWidget(tft)
{
	this->channel = channel;
}

ChannelIconWidget::~ChannelIconWidget(void)
{
}
