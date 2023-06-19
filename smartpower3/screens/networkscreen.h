#ifndef SMARTPOWER3_SCREENS_NETWORKSCREEN_H_
#define SMARTPOWER3_SCREENS_NETWORKSCREEN_H_

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "component.h"
#include "settings.h"
#include "header.h"
#include "screens/screen.h"


#define LINE_SPACING 42
#define RIGHT_POSITION_X 460


enum network_screen_mode_t {
	SETTING_NETWORK = 0,
	SETTING_DHCP,
};


enum network_screen_state_setting {
	STATE_NETWORK_WIFI_ICON = 1,
	STATE_NETWORK_LOGGING_ICON,
	STATE_NETWORK_DHCP,
//	STATE_NETWORK_MODE,
//	STATE_LOG_INTERVAL,
//	STATE_BAUD_RATE,
//	STATE_BL,
};


class NetworkScreen : public Screen
{
public:
	NetworkScreen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t *onoff);

	virtual bool draw(void);
	virtual void init(void);
	virtual void select(void);
	virtual void deSelect(void);

	virtual void onShutdown(void);
	virtual void onWakeup(void);
	virtual screen_t getType(void);

	void drawDhcp(bool dhcp_state = true);
	void selectDhcp(uint16_t text_color=COLOR_TEXT_SELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_SELECTED);
	void deSelectDhcp(uint16_t text_color=COLOR_TEXT_DESELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_DESELECTED);
	void drawStaticIpaddr(const char * ipaddr);
	void drawGatewayIpaddr(const char* ipaddr);
	void drawSubnetMask(const char* ipaddr);
	void drawDnsServers(const char* ipaddr_dns1, const char* ipaddr_dns2);
	//void drawMode(device_operation_mode default_mode = OPERATION_MODE_DEFAULT);

	void onEnter(void);
	void debug(void);
private:
	network_screen_mode_t mode = SETTING_NETWORK;
	uint16_t x;
	uint16_t y;
	Component *com_dhcp;
	Component *com_static_ip;
	Component *com_gateway_ip;
	Component *com_subnet_mask;
	Component *com_dns_ip;
	bool dhcp_turned_on = true;
	bool dhcp_turned_on_edit = true;
	void drawDhcpSelection(void);
	void drawNetwork(void);
	void changeDhcp(bool is_turned_on);
	void restoreDhcp(void);
};


#endif /* SMARTPOWER3_SCREENS_NETWORKSCREEN_H_ */
