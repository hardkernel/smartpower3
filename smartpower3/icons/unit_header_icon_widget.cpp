#include "unit_header_icon_widget.h"

UnitHeaderIconWidget::UnitHeaderIconWidget(TFT_eSPI *tft, icon_image_enum icon_image) : HeaderIconWidget(tft), UnitIconWidget(icon_image)
{
}

UnitHeaderIconWidget::~UnitHeaderIconWidget(void)
{
}

// redefining parents dimensions
const uint16_t UnitHeaderIconWidget::iconWidth(void)
{
	return (uint16_t) ICON_DIMENSION_PORTRAIT_WIDTH;
}

const uint16_t UnitHeaderIconWidget::iconHeight(void)
{
	return (uint16_t) ICON_DIMENSION_PORTRAIT_HEIGHT;
}

const uint8_t* UnitHeaderIconWidget::iconCharArray(void)
{
	return UnitIconWidget::iconCharArray();
}
