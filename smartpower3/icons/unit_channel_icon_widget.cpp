#include "unit_channel_icon_widget.h"

UnitChannelIconWidget::UnitChannelIconWidget(TFT_eSPI *tft, uint8_t channel, icon_image_enum icon_image) : ChannelIconWidget(tft, channel), UnitIconWidget(icon_image)
{
}

UnitChannelIconWidget::~UnitChannelIconWidget(void)
{
}

const uint8_t* UnitChannelIconWidget::iconCharArray(void)
{
	return UnitIconWidget::iconCharArray();
}
