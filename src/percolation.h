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
    int32  from_x;
    int32  from_y;
    int32  from;
    int32  to_x;
    int32  to_y;
    int32  to;
    bool32 is_active;
    Color  color;
} Edge;

typedef struct
{
    Color  color;
    bool32 will_be_processed;
    bool32 is_processed;
} Cell;

typedef struct
{
    int32 edge_count;
    int32 edges[4];
} CellEdgeMap;

typedef struct CellNode CellNode;
struct CellNode
{
    Cell*     v;
    int32     cell_index;
    CellNode* next;
};

Vec2 edge_offsets[4] = {
    {.x = 1,  .y = 0 },
    {.x = -1, .y = 0 },
    {.x = 0,  .y = 1 },
    {.x = 0,  .y = -1},
};

typedef struct
{
    int32 x;
    int32 y;
} CellCoords;

internal int32 get_cell_index(int32 x, int32 y, int32 col_count);
internal int32 get_neighbour_index(int32 x, int32 y, EdgeDirection direction, int32 row_count, int32 col_count);
internal void  set_edge_index(Edge* edges, CellEdgeMap* cell_edge_map, int32 edge_index, int32 from_x, int32 from_y, int32 to_x, int32 to_y, int32 column_count);