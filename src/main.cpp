#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
static void delay(int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
static uint32_t millis() { return xTaskGetTickCount(); }

#include <SmartLeds.h>

#define LGFX_AUTODETECT
#include <LGFX_AUTODETECT.hpp>
#include <lv_demo_conf.h>
#include <lvgl.h>

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * 10];

LGFX gfx;

const int LED_COUNT = 1;
const int DATA_PIN = 8;
const int CHANNEL = 0;

// SmartLed -> RMT driver (WS2812/WS2812B/SK6812/WS2813)
SmartLed leds(LED_WS2812B, LED_COUNT, DATA_PIN, CHANNEL, DoubleBuffer);

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    if (gfx.getStartCount() == 0)
    { // Processing if not yet started
        gfx.startWrite();
    }
    gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void setup()
{
    gfx.begin();

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    lv_demo_benchmark();
}

uint8_t hue;
void showGradient()
{
    hue++;
    // Use HSV to create nice gradient
    leds[0] = Hsv{static_cast<uint8_t>(hue + 30), 255, 255};
    leds.show();
    // Show is asynchronous; if we need to wait for the end of transmission,
    // we can use leds.wait(); however we use double buffered mode, so we
    // can start drawing right after showing.
}

void showRgb()
{
    leds[0] = Rgb{255, 0, 0};
    leds.show();
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */

    if (millis() % 10000 < 5000)
        showGradient();
    else
        showRgb();
    delay(50);
}

extern "C" void app_main()
{
    while (true)
    {
        loop();
        vTaskDelay(0);
    }
}
