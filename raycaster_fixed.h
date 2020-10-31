#pragma once
#include "raycaster.h"

class RayCasterFixed : public RayCaster
{
public:
    void Start(uint16_t playerX, uint16_t playerY, int16_t playerA) override;
    TraceResult Trace(uint16_t screenX) override;

    RayCasterFixed();
    ~RayCasterFixed();

private:
    uint16_t _playerX;
    uint16_t _playerY;
    int16_t _playerA;
    uint8_t _viewQuarter;
    uint8_t _viewAngle;
};
