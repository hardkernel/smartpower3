#include "wifi_header_icon_widget.h"

WifiHeaderIconWidget::WifiHeaderIconWidget(TFT_eSPI *tft) : HeaderIconWidget(tft)
{
}

WifiHeaderIconWidget::~WifiHeaderIconWidget(void)
{
}

const uint8_t* WifiHeaderIconWidget::iconCharArray(void)
{
	return &IMG_WIFI_DATA[0];
}
