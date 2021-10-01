//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESP32Encoder.h>

#include <Wire.h>
#include "WiFi.h"
#include "free_font.h"

//------------------------------------------------------------------------------
#define PWM_FREQ        100000  // 100Khz
#define PWM_RES         8   // 8bits : 0 - 255
#define PWM_VALUE_MAX  256

#define PWM_LCD_PIN     17
#define PWM_LCD_CH      0

#define PWM_LED1_PIN    2
#define PWM_LED1_CH     1

#define PWM_LED2_PIN    13
#define PWM_LED2_CH     2

#define GPIO_BT1_PIN    36
#define GPIO_BT2_PIN    39
#define GPIO_BT3_PIN    34

#define GPIO_ENC_R_PIN      32
#define GPIO_ENC_L_PIN      33
#define GPIO_ENC_BT_PIN     35

#define EVENT_BT1_DN       0x01
#define EVENT_BT1_UP       0x02
#define EVENT_BT2_DN       0x04
#define EVENT_BT2_UP       0x08
#define EVENT_BT3_DN       0x10
#define EVENT_BT3_UP       0x20
#define EVENT_ENC_BT_DN    0x40
#define EVENT_ENC_BT_UP    0x80
#define EVENT_ENC_L         0x100
#define EVENT_ENC_R         0x200

#define GPIO_CH1_EN_PIN     27
#define GPIO_CH1_IRQ_PIN    25
#define GPIO_CH2_EN_PIN     14
#define GPIO_CH2_IRQ_PIN    26

#define FG_COLOR    TFT_DARKGREY
#define BG_COLOR    TFT_BLACK

//------------------------------------------------------------------------------
#define TEST_TABLE_NUM  4

uint16_t TEST_COLOR_TABLE[4] = { TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE };
uint16_t TEST_PWM_TABLE[4]   = { 10, 100, 150, 250 };

uint16_t test_func = 0;
uint16_t button_event = 0;

#define TEST_FUNC_NUM   3

//------------------------------------------------------------------------------
TFT_eSPI tft = TFT_eSPI();

ESP32Encoder    encoder;

//------------------------------------------------------------------------------
// GPIO Port & PWM Init
//------------------------------------------------------------------------------
void port_init(void)
{
    // STPD01 CH1 disable
    pinMode(GPIO_CH1_EN_PIN, OUTPUT);
    digitalWrite(GPIO_CH1_EN_PIN, LOW);

    // STPD01 CH2 disable
    pinMode(GPIO_CH2_EN_PIN, OUTPUT);
    digitalWrite(GPIO_CH2_EN_PIN, LOW);

    // BT1, BT2, BT3
    pinMode(GPIO_BT1_PIN, INPUT);
    pinMode(GPIO_BT2_PIN, INPUT);
    pinMode(GPIO_BT3_PIN, INPUT);

    // Encoder Port
    encoder.attachHalfQuad(GPIO_ENC_L_PIN, GPIO_ENC_R_PIN);
    encoder.setCount(0);
    encoder.setFilter(500);
    pinMode(GPIO_ENC_BT_PIN, INPUT);

    //PWM Backlight, LED
    ledcSetup       (PWM_LCD_CH, PWM_FREQ, PWM_RES);
    ledcAttachPin   (PWM_LCD_PIN, PWM_LCD_CH);
    ledcWrite       (PWM_LCD_CH, PWM_VALUE_MAX);

    ledcSetup       (PWM_LED1_CH, PWM_FREQ, PWM_RES);
    ledcAttachPin   (PWM_LED1_PIN, PWM_LED1_CH);
    ledcWrite       (PWM_LED1_CH, PWM_VALUE_MAX);

    ledcSetup       (PWM_LED2_CH, PWM_FREQ, PWM_RES);
    ledcAttachPin   (PWM_LED2_PIN, PWM_LED2_CH);
    ledcWrite       (PWM_LED2_CH, PWM_VALUE_MAX);
}

//------------------------------------------------------------------------------
void setup(void) {
    Serial.begin(115200);

    while (! Serial); // Wait until Serial is ready - Leonardo

    Serial.println("setup");

    port_init();

    tft.init();
    tft.setRotation(3);
    tft.setSwapBytes(true);
    tft.fillScreen(BG_COLOR);
}

//------------------------------------------------------------------------------
void button_check(void)
{
    static uint8_t bt_status_old = 0;
    uint8_t bt_xor = 0, bt_status = 0;

    bt_status  = digitalRead(GPIO_BT1_PIN) ? 0 : 0x01;
    bt_status |= digitalRead(GPIO_BT2_PIN) ? 0 : 0x02;
    bt_status |= digitalRead(GPIO_BT3_PIN) ? 0 : 0x04;
    bt_status |= digitalRead(GPIO_ENC_BT_PIN) ? 0 : 0x08;

    if (bt_status_old != bt_status) {
        bt_xor = bt_status ^ bt_status_old;
        if (bt_xor & 0x01)
            button_event = (bt_status & 0x01) ? EVENT_BT1_DN : EVENT_BT1_UP;
        if (bt_xor & 0x02)
            button_event = (bt_status & 0x02) ? EVENT_BT2_DN : EVENT_BT2_UP;
        if (bt_xor & 0x04)
            button_event = (bt_status & 0x04) ? EVENT_BT3_DN : EVENT_BT3_UP;
        if (bt_xor & 0x08)
            button_event = (bt_status & 0x08) ? EVENT_ENC_BT_DN : EVENT_ENC_BT_UP;
        bt_status_old = bt_status;
    }

    if (encoder.getCount() > 1)
        button_event |= EVENT_ENC_R;
    else if (encoder.getCount() < -1)
        button_event |= EVENT_ENC_L;

    encoder.setCount(0);
}

//------------------------------------------------------------------------------
void color_test_init(void)
{
    tft.fillScreen(BG_COLOR);
    tft.setFreeFont(FSSBO18);
    tft.setTextColor(TFT_GREEN, BG_COLOR);
    tft.drawString("[ LCD/LED Color Test ]", 0, 0);
    tft.setFreeFont(FSSBO12);
    tft.setTextColor(TFT_YELLOW, BG_COLOR);
    tft.drawString("  - Press any button key to Test Start...", 0, 40);
}

//------------------------------------------------------------------------------
bool color_test(uint16_t bt_event)
{
    static uint8_t pwm_pos = 0, led_pos = 0, color_pos = 0;

    switch(bt_event) {
        case EVENT_BT1_UP:
            led_pos++;
            led_pos %= TEST_TABLE_NUM;
        break;
        case EVENT_BT2_UP:
            color_pos = (color_pos > 0) ? color_pos -1 : TEST_TABLE_NUM -1;
        break;
        case EVENT_BT3_UP:
            color_pos++;
            color_pos %= TEST_TABLE_NUM;
        break;
        case EVENT_ENC_L:
            if (pwm_pos)    pwm_pos--;
        break;
        case EVENT_ENC_R:
            if (pwm_pos < TEST_TABLE_NUM -1)    pwm_pos++;
        break;
        case EVENT_ENC_BT_UP:
            ledcWrite(PWM_LCD_CH, PWM_VALUE_MAX);
            ledcWrite(PWM_LED1_CH, PWM_VALUE_MAX);
            ledcWrite(PWM_LED2_CH, PWM_VALUE_MAX);
            return  true;
        break;
        default:
            return false;
    }

    tft.fillScreen(TEST_COLOR_TABLE[color_pos]);
    ledcWrite(PWM_LCD_CH, TEST_PWM_TABLE[pwm_pos]);

    switch(led_pos) {
        default:
        case 0:
            ledcWrite(PWM_LED1_CH, TEST_PWM_TABLE[pwm_pos]);
            ledcWrite(PWM_LED2_CH, TEST_PWM_TABLE[pwm_pos]);
        break;
        case 1:
            ledcWrite(PWM_LED1_CH, TEST_PWM_TABLE[pwm_pos]);
            ledcWrite(PWM_LED2_CH, 0);
        break;
        case 2:
            ledcWrite(PWM_LED1_CH, 0);
            ledcWrite(PWM_LED2_CH, TEST_PWM_TABLE[pwm_pos]);
        break;
        case 3:
            ledcWrite(PWM_LED1_CH, 0);
            ledcWrite(PWM_LED2_CH, 0);
        break;
    }
    return false;
}

//------------------------------------------------------------------------------
void button_test_init(void)
{
    tft.fillScreen(BG_COLOR);
    tft.setFreeFont(FSSBO18);
    tft.setTextColor(TFT_GREEN, BG_COLOR);
    tft.drawString("[ Button Test ]", 0, 0);

    tft.setTextColor(TFT_DARKGREY, BG_COLOR);
    tft.drawString("   - Button1 Event", 0, 60);
    tft.drawString("   - Button2 Event", 0, 100);
    tft.drawString("   - Button3 Event", 0, 140);
    tft.drawString("   - Encoder Left Event", 0, 180);
    tft.drawString("   - Encoder Right Event", 0, 220);
    tft.drawString("   - Encoder Button Event", 0, 260);
}

//------------------------------------------------------------------------------
bool button_test(uint16_t bt_event)
{
    static uint16_t event_check =
        ( EVENT_BT1_DN | EVENT_BT1_UP | EVENT_BT2_DN | EVENT_BT2_UP |
          EVENT_BT3_DN | EVENT_BT3_UP | EVENT_ENC_BT_DN | EVENT_ENC_BT_UP |
          EVENT_ENC_R | EVENT_ENC_L );

    if (bt_event & EVENT_BT1_DN) {
        tft.setTextColor(TFT_YELLOW, BG_COLOR);
        tft.drawString("OK - Button1 Event", 0, 60);
    }
    if (bt_event & EVENT_BT1_UP) {
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        tft.drawString("OK - Button1 Event", 0, 60);
    }

    if (bt_event & EVENT_BT2_DN) {
        tft.setTextColor(TFT_YELLOW, BG_COLOR);
        tft.drawString("OK - Button2 Event", 0, 100);
    }
    if (bt_event & EVENT_BT2_UP) {
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        tft.drawString("OK - Button2 Event", 0, 100);
    }

    if (bt_event & EVENT_BT3_DN) {
        tft.setTextColor(TFT_YELLOW, BG_COLOR);
        tft.drawString("OK - Button3 Event", 0, 140);
    }
    if (bt_event & EVENT_BT3_UP) {
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        tft.drawString("OK - Button3 Event", 0, 140);
    }

    if (bt_event & EVENT_ENC_BT_DN) {
        tft.setTextColor(TFT_YELLOW, BG_COLOR);
        tft.drawString("OK - Encoder Button Event", 0, 260);
    }
    if (bt_event & EVENT_ENC_BT_UP) {
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        tft.drawString("OK - Encoder Button Event", 0, 260);
    }

    if (bt_event & EVENT_ENC_L) {
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        tft.drawString("OK - Encoder Left Event", 0, 180);
    }
    if (bt_event & EVENT_ENC_R) {
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        tft.drawString("OK - Encoder Right Event", 0, 220);
    }
    event_check &= ~bt_event;
    if (!event_check) {
        event_check =
            ( EVENT_BT1_DN | EVENT_BT1_UP | EVENT_BT2_DN | EVENT_BT2_UP |
              EVENT_BT3_DN | EVENT_BT3_UP | EVENT_ENC_BT_DN | EVENT_ENC_BT_UP |
              EVENT_ENC_R | EVENT_ENC_L );
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
void wifi_test_init(void)
{
    tft.fillScreen(BG_COLOR);
    tft.setFreeFont(FSSBO18);
    tft.setTextColor(TFT_GREEN, BG_COLOR);
    tft.drawString("[ WiFi STA Test ]", 0, 0);
    tft.setFreeFont(FSSBO12);
    tft.setTextColor(TFT_YELLOW, BG_COLOR);
    tft.drawString("  - Press any button key to WiFi scan...", 0, 40);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

//------------------------------------------------------------------------------
bool wifi_test(uint16_t bt_event)
{
    static uint16_t sta_cnt = 0;

    switch(bt_event) {
        case EVENT_ENC_BT_UP:
            if (sta_cnt)    {
                sta_cnt = 0;
                return  true;
            }
        case EVENT_BT1_UP:
        case EVENT_BT2_UP:
        case EVENT_BT3_UP:
            tft.fillRoundRect(0, 40, 480, 320, 0, BG_COLOR);
            tft.setFreeFont(FSSBO12);
            tft.setTextColor(TFT_YELLOW, BG_COLOR);
            tft.drawString("  - WiFi Scanning...", 0, 40);
            sta_cnt = WiFi.scanNetworks();
        break;
        default:
            return false;
    }

    tft.fillRoundRect(0, 40, 480, 320, 0, BG_COLOR);
    if (sta_cnt) {
        char buf[100], list_cnt;

        tft.setFreeFont(FSSBO12);
        tft.setTextColor(TFT_GREEN, BG_COLOR);
        memset(buf, 0x00, sizeof(buf));
        sprintf(buf, "   - Networks found(%d).. ", sta_cnt);
        tft.drawString(buf, 0, 40);

        tft.setFreeFont(FSSBO9);
        list_cnt = (sta_cnt > 10) ? 10 : sta_cnt;
        for (int i = 0, j = 0; i < list_cnt; i++, j+= 20) {
            tft.drawString(WiFi.SSID(i), 50, 70 + j);
        }
    } else {
        tft.setTextColor(TFT_RED, BG_COLOR);
        tft.drawString("   - no Networks found..", 0, 100);
    }

    return false;
}

//------------------------------------------------------------------------------
void loop()
{
    static uint8_t init_func_pos = -1, test_func_pos = 0;
    bool test_complete = false;

    button_check();

    if (init_func_pos != test_func_pos) {
        init_func_pos  = test_func_pos;
        switch(init_func_pos) {
            case 0: button_test_init(); break;
            case 1: color_test_init();  break;
            case 2: wifi_test_init();   break;
            default:
            break;
        }
    }

    switch(test_func_pos) {
        case 0: test_complete = button_test(button_event);  break;
        case 1: test_complete = color_test(button_event);   break;
        case 2: test_complete = wifi_test(button_event);    break;
        default:
        break;
    }

    if (test_complete) {
        test_func_pos++;
        test_func_pos %= TEST_FUNC_NUM;
        test_complete = false;
    }
    button_event = 0;

    delay(100);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
