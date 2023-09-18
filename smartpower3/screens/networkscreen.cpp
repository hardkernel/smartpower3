#include "networkscreen.h"

#define X_IPADDRESS 200
#define DNS_SERVERS_ARRAY_LENGTH 33


NetworkScreen::NetworkScreen (TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager,
								uint8_t *onoff) : Screen(tft, header, settings, wifi_manager, onoff)
{
	com_dhcp = new Component(tft, 250, 22, 2);
	com_static_ip = new Component(tft, 250, 22, 2);
	com_subnet_mask = new Component(tft, 250, 22, 2);
	com_gateway_ip = new Component(tft, 250, 22, 2);
	com_dns_ip = new Component(tft, 310, 22, 2);

	this->x = SETTING_SCREEN_INIT_POSITION_X;
	this->y = SETTING_SCREEN_INIT_POSITION_Y;
}

bool NetworkScreen::draw (void)
{
	show_next_screen = false;

	if (!this->isInitialized()) {
		this->init();
	}

	switch (mode) {
		case SETTING_NETWORK:
			drawNetwork();
			break;
		case SETTING_DHCP:
			drawDhcpSelection();
			break;
		default:
			break;
	}

	if (wifi_manager->update_dhcp_info) {
		wifi_manager->update_dhcp_info = false;
		this->drawDhcp(settings->isWifiIpv4DhcpEnabled());
	}
	if (wifi_manager->update_static_ip_info) {
		wifi_manager->update_static_ip_info = false;
		this->drawStaticIpaddr(settings->getWifiIpv4StaticIp().toString().c_str());
	}
	if (wifi_manager->update_subnet_info) {
		wifi_manager->update_subnet_info = false;
		this->drawSubnetMask(settings->getWifiIpv4SubnetMask().toString().c_str());
	}
	if (wifi_manager->update_gateway_address_info) {
		wifi_manager->update_gateway_address_info = false;
		this->drawGatewayIpaddr(settings->getWifiIpv4GatewayAddress().toString().c_str());
	}
	if (wifi_manager->update_dns_info) {
		wifi_manager->update_dns_info = false;
		this->drawDnsServers(
				settings->getWifiIpv4AddressDns1().toString().c_str(),
				settings->getWifiIpv4AddressDns2().toString().c_str());
	}

	header->draw();
	return show_next_screen;
}

void NetworkScreen::init (void)
{
	for (int i = 0; i < 2; i++) {
		tft->drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
	}

	tft->fillRect(0, 52, 480, 285, BG_COLOR);
	tft->loadFont(getFont(NOTOSANSBOLD20));
	tft->drawString(F("Build date : "), x + 128, y + 215, 2);
	tft->drawString(F(__DATE__), x + 248, y + 215, 2);
	tft->drawString(F(__TIME__), x + 378, y + 215, 2);

	tft->drawString(F("DHCP"), x, y, 4);
	tft->drawString(F("Static IP Address"), x, y + LINE_SPACING, 4);
	tft->drawString(F("Subnet Mask"), x, y + 2*LINE_SPACING, 4);
	tft->drawString(F("Gateway IP Address"), x, y + 3*LINE_SPACING, 4);
	tft->drawString(F("DNS Servers"), x, y + 4*LINE_SPACING, 4);
	tft->unloadFont();

	com_dhcp->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_dhcp->setCoordinate(x + X_IPADDRESS, y);
	com_static_ip->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_static_ip->setCoordinate(x + X_IPADDRESS, y + LINE_SPACING);

	com_subnet_mask->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_subnet_mask->setCoordinate(x + X_IPADDRESS, y + 2*LINE_SPACING);

	com_gateway_ip->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_gateway_ip->setCoordinate(x + X_IPADDRESS, y + 3*LINE_SPACING);

	com_dns_ip->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_dns_ip->setCoordinate(x + X_IPADDRESS - 60, y + 4*LINE_SPACING);

	drawDhcp(settings->isWifiIpv4DhcpEnabled(true));
	drawStaticIpaddr(settings->getWifiIpv4StaticIp(true).toString().c_str());
	drawGatewayIpaddr(settings->getWifiIpv4GatewayAddress(true).toString().c_str());
	drawSubnetMask(settings->getWifiIpv4SubnetMask(true).toString().c_str());
	drawDnsServers(
			settings->getWifiIpv4AddressDns1(true).toString().c_str(),
			settings->getWifiIpv4AddressDns2(true).toString().c_str());

	selected = dial_cnt = dial_cnt_old = STATE_NONE;

	is_initialized = true;
}

void NetworkScreen::select (void)
{
	if (dial_cnt == dial_cnt_old) {
		return;
	}
	// 3 is 1 based count of screen selectable elements, lower limit is 1 because 0 is used
	// as a marker for non-selected item that should not be included in the "selection circle"
	clampVariableToCircularRange(1, 3, dial_direct, &dial_cnt);
	dial_cnt_old = dial_cnt;

	deSelect();
	selected = dial_cnt;
	switch (dial_cnt) {
		case STATE_NETWORK_WIFI_ICON:
			header->select(WIFI);
			break;
		case STATE_NETWORK_LOGGING_ICON:
			header->select(LOGGING);
			break;
		case STATE_NETWORK_DHCP:
			this->selectDhcp();
			break;
		default:
			break;
	}
}

void NetworkScreen::deSelect(void)
{
	header->deSelect(LOGGING);
	header->deSelect(WIFI);
	this->deSelectDhcp();
}

void NetworkScreen::onShutdown (void)
{
}

void NetworkScreen::onWakeup (void)
{
}

screen_t NetworkScreen::getType (void)
{
	return NETWORK_SCREEN;
}

void NetworkScreen::onEnter (void)
{
}

void NetworkScreen::debug (void)
{
}

void NetworkScreen::drawDhcpSelection(void)
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING_NETWORK;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		deSelectDhcp(COLOR_TEXT_DESELECTED);
		restoreDhcp();
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		mode = SETTING_NETWORK;
		deSelectDhcp(COLOR_TEXT_DESELECTED);
		selected = dial_cnt = dial_cnt_old = STATE_NETWORK_DHCP;
		restoreDhcp();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING_NETWORK;
		settings->setWifiIpv4DhcpEnabled(dhcp_turned_on_edit);
		dhcp_turned_on = dhcp_turned_on_edit;
		this->selectDhcp(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_SELECTED);
		selected = dial_cnt = dial_cnt_old = STATE_NETWORK_DHCP;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		clampVariableToRange(0, 1, &dial_cnt);
		this->changeDhcp(dial_cnt);
		dial_cnt_old = dial_cnt;
	}
}

void NetworkScreen::drawNetwork(void)
{
	select();
	if ((cur_time - dial_time) > 10000) {
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		if (selected == STATE_NONE) {
			show_next_screen = true;
		} else {
			deSelect();
			show_next_screen = false;
		}
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[1] == true) {
		btn_pressed[1] = false;
		this->debug();
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == STATE_NETWORK_DHCP) {
			mode = SETTING_DHCP;
			dial_cnt = settings->isWifiIpv4DhcpEnabled();
			this->selectDhcp(COLOR_TEXT_ACTIVATED, COLOR_RECTANGLE_ACTIVATED);
		} else if (selected == STATE_NETWORK_LOGGING_ICON) {
			//mode = SETTING_NETWORK;
			settings->switchLogging();
		} else if (selected == STATE_NETWORK_WIFI_ICON) {
			//mode = SETTING_NETWORK;
			settings->switchWifi();
		}
	}
	dial_cnt_old = dial_cnt;
}

void NetworkScreen::drawDhcp(bool dhcp_state)
{
	com_dhcp->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), (dhcp_state == 0) ? F("OFF") : F("ON"));
}

void NetworkScreen::selectDhcp(uint16_t text_color, uint16_t rectangle_color)
{
	com_dhcp->select(rectangle_color);
}

void NetworkScreen::deSelectDhcp(uint16_t text_color, uint16_t rectangle_color)
{
	com_dhcp->deSelect(rectangle_color);
}

void NetworkScreen::drawStaticIpaddr(const char* ipaddr)
{
	com_static_ip->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), ipaddr);
}

void NetworkScreen::drawGatewayIpaddr(const char* ipaddr)
{
	com_gateway_ip->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), ipaddr);
}

void NetworkScreen::drawSubnetMask(const char* ipaddr)
{
	com_subnet_mask->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), ipaddr);
}

void NetworkScreen::drawDnsServers(const char* ipaddr_dns1, const char* ipaddr_dns2)
{
	char draw_string[DNS_SERVERS_ARRAY_LENGTH];
	snprintf(draw_string, DNS_SERVERS_ARRAY_LENGTH, "%s, %s", ipaddr_dns1, ipaddr_dns2);
	com_dns_ip->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), draw_string);
}

void NetworkScreen::changeDhcp (bool is_turned_on)
{
	drawDhcp(is_turned_on);
	dhcp_turned_on_edit = is_turned_on;
}

void NetworkScreen::restoreDhcp (void)
{
	dhcp_turned_on_edit = dhcp_turned_on;
	drawDhcp(dhcp_turned_on);
}
