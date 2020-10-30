#pragma once
#include "raycaster.h"

class RayCasterFixed : public RayCaster
{
public:
    void Start(uint16_t playerX, uint16_t playerY, int16_t playerA);
    void Trace(uint16_t screenX,
               uint8_t *screenY,
               uint8_t *textureNo,
               uint8_t *textureX,
               uint16_t *textureY,
               uint16_t *textureStep);

    RayCasterFixed();
    ~RayCasterFixed();

private:
    uint16_t _playerX;
    uint16_t _playerY;
    int16_t _playerA;
    uint8_t _viewQuarter;
    uint8_t _viewAngle;
};
