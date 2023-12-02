#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
static void delay(int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
static uint32_t millis() { return xTaskGetTickCount(); }

#include <SmartLeds.h>

const int LED_COUNT = 1;
const int DATA_PIN = 8;
const int CHANNEL = 0;

// SmartLed -> RMT driver (WS2812/WS2812B/SK6812/WS2813)
SmartLed leds(LED_WS2812B, LED_COUNT, DATA_PIN, CHANNEL, DoubleBuffer);

// const int CLK_PIN = 23;
// APA102 -> SPI driver
// Apa102 leds(LED_COUNT, CLK_PIN, DATA_PIN, DoubleBuffer);

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
