#pragma once

#include <algorithm>
#include <array>
#include <type_traits>
#include "gcem.hpp"
#include "raycaster.h"

template <typename T, typename V>
constexpr T clamp_cast(V v)
{
    return static_cast<T>(std::clamp<V>(v, std::numeric_limits<T>::min(),
                                        std::numeric_limits<T>::max()));
}

constexpr auto g_tan = []() constexpr
{
    std::array<uint16_t, 256> g_tan{};
    for (int i = 0; i < 256; i++)
        g_tan[i] =
            static_cast<uint16_t>((256.0f * gcem::tan(i * M_PI_2 / 256.0f)));
    return g_tan;
}
();

constexpr auto g_cotan = []() constexpr
{
    std::array<uint16_t, 256> g_cotan{};
    for (int i = 0; i < 256; i++) {
        auto t = gcem::tan(i * M_PI_2 / 256.0f);
        g_cotan[i] =
            t != 0
                ? static_cast<uint16_t>(256.0f / t)
                : static_cast<uint16_t>(std::numeric_limits<uint16_t>::max());
    }
    return g_cotan;
}
();

constexpr auto g_sin = []() constexpr
{
    std::array<uint8_t, 256> g_sin{};
    for (int i = 0; i < 256; i++) {
        g_sin[i] =
            static_cast<uint8_t>(256.0f * gcem::sin(i / 1024.0f * 2 * M_PI));
    }
    return g_sin;
}
();

constexpr auto g_cos = []() constexpr
{
    std::array<uint8_t, 256> g_cos{};
    for (int i = 0; i < 256; i++) {
        g_cos[i] =
            clamp_cast<uint8_t>(256.0f * gcem::cos(i / 1024.0f * 2 * M_PI));
    }
    return g_cos;
}
();

constexpr auto g_deltaAngle = []() constexpr
{
    std::array<uint16_t, SCREEN_WIDTH> g_deltaAngle{};
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        float deltaAngle = atanf(((int16_t) i - SCREEN_WIDTH / 2.0f) /
                                 (SCREEN_WIDTH / 2.0f) * M_PI / 4);
        int16_t da = static_cast<int16_t>(deltaAngle / M_PI_2 * 256.0f);
        if (da < 0) {
            da += 1024;
        }
        g_deltaAngle[i] = static_cast<uint16_t>(da);
    }
    return g_deltaAngle;
}
();

constexpr auto g_nearHeight = []() constexpr
{
    std::array<uint8_t, 256> g_nearHeight{};
    for (int i = 0; i < 256; i++) {
        g_nearHeight[i] = static_cast<uint8_t>(
            (INV_FACTOR_INT / (((i << 2) + MIN_DIST) >> 2)) >> 2);
    }
    return g_nearHeight;
}
();

constexpr auto g_farHeight = []() constexpr
{
    std::array<uint8_t, 256> g_farHeight{};
    for (int i = 0; i < 256; i++) {
        g_farHeight[i] = static_cast<uint8_t>(
            (INV_FACTOR_INT / (((i << 5) + MIN_DIST) >> 5)) >> 5);
    }
    return g_farHeight;
}
();

constexpr auto g_nearStep = []() constexpr
{
    std::array<uint16_t, SCREEN_WIDTH> g_nearStep{};
    for (int i = 0; i < 256; i++) {
        auto txn =
            ((INV_FACTOR_INT / (((i * 4.0f) + MIN_DIST) / 4.0f)) / 4.0f) * 2.0f;
        if (txn != 0) {
            g_nearStep[i] = (256 / txn) * 256;
        }
    }
    return g_nearStep;
}
();

constexpr auto g_farStep = []() constexpr
{
    std::array<uint16_t, SCREEN_WIDTH> g_farStep{};
    for (int i = 0; i < 256; i++) {
        auto txf =
            ((INV_FACTOR_INT / (((i * 32.0f) + MIN_DIST) / 32.0f)) / 32.0f) *
            2.0f;
        if (txf != 0) {
            g_farStep[i] = (256 / txf) * 256;
        }
    }
    return g_farStep;
}
();

constexpr auto g_overflowStep = []() constexpr
{
    std::array<uint16_t, 256> g_overflowStep{};
    for (int i = 1; i < 256; i++) {
        auto txs = ((INV_FACTOR_INT / (float) (i / 2.0f)));
        auto ino = (txs - SCREEN_HEIGHT) / 2;
        g_overflowStep[i] = (256 / txs) * 256;
    }
    return g_overflowStep;
}
();

constexpr auto g_overflowOffset = []() constexpr
{
    std::array<uint16_t, 256> g_overflowOffset{};
    for (int i = 1; i < 256; i++) {
        auto txs = ((INV_FACTOR_INT / (float) (i / 2.0f)));
        auto ino = (txs - SCREEN_HEIGHT) / 2;
        g_overflowOffset[i] = clamp_cast<uint16_t>(ino * (256 / txs) * 256);
    }
    return g_overflowOffset;
}
();