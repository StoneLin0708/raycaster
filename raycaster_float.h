#pragma once
#include "raycaster.h"
#include "raycaster_data.h"

class RayCasterFloat : public RayCaster
{
public:
    void Start(uint16_t playerX, uint16_t playerY, int16_t playerA) override;
    TraceResult Trace(uint16_t screenX) override;

    RayCasterFloat();
    ~RayCasterFloat();

private:
    float _playerX;
    float _playerY;
    float _playerA;

    float Distance(float playerX,
                   float playerY,
                   float rayA,
                   float *hitOffset,
                   int *hitDirection);
    bool IsWall(float rayX, float rayY);
};
