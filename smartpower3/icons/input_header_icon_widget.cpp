#include "input_header_icon_widget.h"

InputHeaderIconWidget::InputHeaderIconWidget(TFT_eSPI *tft) : HeaderIconWidget(tft)
{
}

InputHeaderIconWidget::~InputHeaderIconWidget(void)
{
}

const uint8_t* InputHeaderIconWidget::iconCharArray(void)
{
	return &IMG_INPUT_DATA2[0];
}
