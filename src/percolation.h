#pragma once
#include "opus/base/base_inc.h"
#include "opus/gfx/gfx_inc.h"
#include "opus/os/os_inc.h"
#include "opus/text/text_inc.h"
#include "opus/draw/draw_inc.h"
#include "opus/ui/ui_inc.h"
#include "opus/input/input_inc.h"

typedef enum
{
    EdgeDirection_Right  = 0,
    EdgeDirection_Left   = 1,
    EdgeDirection_Top    = 2,
    EdgeDirection_Bottom = 3,
    EdgeDirection_COUNT
} EdgeDirection;

typedef struct
{
    Color color;
    int32 edges[4];
} Cell;

Vec2 edge_offsets[4] = {
    {.x = 1,  .y = 0 },
    {.x = -1, .y = 0 },
    {.x = 0,  .y = 1 },
    {.x = 0,  .y = -1},
};