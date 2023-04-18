#include "unit_icon_widget.h"

UnitIconWidget::UnitIconWidget(icon_image_enum icon_image)
{
	this->icon_image = icon_image;
}

UnitIconWidget::~UnitIconWidget(void)
{
}

const uint8_t* UnitIconWidget::iconCharArray(void)
{
	switch (icon_image) {
	case LARGE_VOLT:
		return &IMG_VOLT_DATA2[0];
	case LARGE_AMPERE:
		return &IMG_AMPERE_DATA2[0];
	case LARGE_WATT:
		return &IMG_WATT_DATA2[0];
	case SMALL_VOLT:
		return &IMG_VOLT_DATA3[0];
	case SMALL_AMPERE:
		return &IMG_AMPERE_DATA3[0];
	default:
		return NULL;
	}
}
