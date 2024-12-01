#include "opus/base/base_inc.c"
#include "opus/gfx/gfx_inc.c"
#include "opus/os/os_inc.c"
#include "opus/text/text_inc.c"
#include "opus/draw/draw_inc.c"
#include "opus/ui/ui_inc.c"
#include "opus/input/input_inc.c"

#include "percolation.h"

internal int32
get_cell_index(int32 x, int32 y, int32 col_count)
{
    return y * col_count + x;
}

internal int32
get_neighbour_index(int32 x, int32 y, EdgeDirection direction, int32 row_count, int32 col_count)
{
    Vec2  offset = edge_offsets[direction];
    int32 new_x  = x + (int32)offset.x;
    int32 new_y  = y + (int32)offset.y;

    // out of bounds
    if (new_x < 0 || new_x >= col_count || new_y < 0 || new_y >= row_count)
        return -1;

    return get_cell_index(new_x, new_y, col_count);
}

internal void
set_edge_index(Edge* edges, CellEdgeMap* cell_edge_map, int32 edge_index, int32 from_x, int32 from_y, int32 to_x, int32 to_y, int32 column_count)
{
    int32 from_index                          = get_cell_index(from_x, from_y, column_count);
    edges[edge_index].from                    = from_index;
    edges[edge_index].from_x                  = from_x;
    edges[edge_index].from_y                  = from_y;
    CellEdgeMap* cell_edge                    = &cell_edge_map[from_index];
    cell_edge->edges[cell_edge->edge_count++] = edge_index;

    int32 to_index                            = get_cell_index(to_x, to_y, column_count);
    edges[edge_index].to                      = to_index;
    edges[edge_index].to_x                    = to_x;
    edges[edge_index].to_y                    = to_y;
    cell_edge                                 = &cell_edge_map[to_index];
    cell_edge->edges[cell_edge->edge_count++] = edge_index;
}