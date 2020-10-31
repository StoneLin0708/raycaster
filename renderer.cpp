#include "renderer.h"
#include <math.h>
#include "raycaster_data.h"

void Renderer::TraceFrame(Game *g, uint32_t *fb)
{
    _rc->Start(static_cast<uint16_t>(g->playerX * 256.0f),
               static_cast<uint16_t>(g->playerY * 256.0f),
               static_cast<int16_t>(g->playerA / (2.0f * M_PI) * 1024.0f));

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        uint32_t *lb = fb + x;

        const auto trace = _rc->Trace(x);
        auto screenY = trace.screenY;

        int16_t ws = HORIZON_HEIGHT - trace.screenY;
        if (ws < 0) {
            ws = 0;
            screenY = HORIZON_HEIGHT;
        }
        uint16_t to = trace.textureY;

        for (int y = 0; y < ws; y++) {
            *lb = GetARGB(96 + (HORIZON_HEIGHT - y));
            lb += SCREEN_WIDTH;
        }

        const auto tx = static_cast<int>(trace.textureX >> 2);
        for (int y = 0; y < screenY * 2; y++) {
            // paint texture pixel
            auto ty = static_cast<int>(to >> 10);
            auto tv = g_texture8[(ty << 6) + tx];

            to += trace.textureStep;

            if (trace.textureNo == 1 && tv > 0) {
                // dark wall
                tv >>= 1;
            }
            *lb = GetARGB(tv);
            lb += SCREEN_WIDTH;
        }

        for (int y = 0; y < ws; y++) {
            *lb = GetARGB(96 + (HORIZON_HEIGHT - (ws - y)));
            lb += SCREEN_WIDTH;
        }
    }
}
