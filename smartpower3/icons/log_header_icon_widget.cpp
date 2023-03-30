#include "log_header_icon_widget.h"

LogHeaderIconWidget::LogHeaderIconWidget(TFT_eSPI *tft) : HeaderIconWidget(tft)
{
}

LogHeaderIconWidget::~LogHeaderIconWidget(void)
{
}

const uint8_t* LogHeaderIconWidget::iconCharArray(void)
{
	return &IMG_LOG_DATA2[0];
}
