#include "protection_channel_icon_widget.h"

ProtectionChannelIconWidget::ProtectionChannelIconWidget(TFT_eSPI *tft, uint8_t channel, icon_image_enum icon_image) : ChannelIconWidget(tft, channel)
{
	this->protection = true;
	this->icon_image = icon_image;
}

ProtectionChannelIconWidget::~ProtectionChannelIconWidget(void)
{
}

// redefining parents dimensions
const uint16_t ProtectionChannelIconWidget::iconWidth(void)
{
	return (uint16_t) ICON_DIMENSION_LANDSCAPE_WIDTH;
}

const uint16_t ProtectionChannelIconWidget::iconHeight(void)
{
	return (uint16_t) ICON_DIMENSION_LANDSCAPE_HEIGHT;
}

const uint8_t* ProtectionChannelIconWidget::iconCharArray(void)
{
	return &IMG_ICON_TABLE[this->icon_image][0];
}

void ProtectionChannelIconWidget::clearProtection(void)
{
	this->protection = false;
}

void ProtectionChannelIconWidget::setProtection(bool val)
{
	if (val) {
		this->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		this->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}

	this->protection = val;
}

bool ProtectionChannelIconWidget::getProtection(void)
{
	return this->protection;
}

void ProtectionChannelIconWidget::update(bool forced)
{
	if (!ic->refresh && !forced) {
		return;
	}

	ic->refresh = false;

	this->draw();
}
