#include "channel_icon_widget.h"

ChannelIconWidget::ChannelIconWidget(TFT_eSPI *tft, uint8_t channel) : BaseIconWidget(tft)
{
	this->channel = channel;
}

ChannelIconWidget::~ChannelIconWidget(void)
{
}

void ChannelIconWidget::clearInt(void)
{
	value = false;
}

void ChannelIconWidget::setInt(bool val)
{
	if (val) {
		setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}

	value = val;
}

bool ChannelIconWidget::getInt(void)
{
	return value;
}
