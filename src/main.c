#include "percolation.h"
#include "percolation.c"

int32
main(void)
{
    ThreadContext tctx;
    tctx_init_and_equip(&tctx);
    Arena* persistent_arena = arena_new();
    Arena* frame_arena      = arena_new();

    GFX_Configuration config = {0};
    config.clear_color       = ColorSlate800;

    input_init(persistent_arena);
    OS_Handle window = os_window_create(1920, 1080, string("Memory Tag"), input_key_callback);
    gfx_init(config);
    font_cache_init(persistent_arena);
    ui_init(persistent_arena);
    d_context_init(persistent_arena, frame_arena, string("..\\assets"));

    /*********************/
    /** APPLICATION CODE */

    /** setup inputs */
    String action_quit = string("quit");
    input_register_key(ui_input_select, OS_KeyCode_MouseLeft);
    input_register_key(action_quit, OS_KeyCode_BracketRight);
    random_init(123818);

    /** ui state */
    float32 prev_slider_value = 0;
    float32 slider_value      = 0;
    bool32  should_quit       = false;

    const float32 circle_radius = 12;
    const float32 padding       = 12;
    const float32 circle_size   = circle_radius + padding;

    const int32  row_count     = 40;
    const int32  column_count  = 40;
    const int32  cell_count    = row_count * column_count;
    const int32  edge_count    = (cell_count - row_count) + (cell_count - column_count);
    const Vec2   cell_offset   = vec2(-circle_size * ((float32)row_count / 2), -circle_size * ((float32)column_count / 2));
    Cell*        cells         = arena_push_array_zero(persistent_arena, Cell, cell_count);
    Edge*        edges         = arena_push_array_zero(persistent_arena, Edge, edge_count);
    CellEdgeMap* cell_edge_map = arena_push_array_zero(persistent_arena, CellEdgeMap, cell_count);

    // setup edges
    {
        int32 edge_index = 0;
        for (int32 y = 0; y < row_count; y++)
        {
            for (int32 x = 0; x < column_count; x++)
            {
                if (x < column_count - 1)
                {
                    set_edge_index(edges, cell_edge_map, edge_index, x, y, x + 1, y, column_count);
                    edge_index++;
                }

                if (y < row_count - 1)
                {
                    set_edge_index(edges, cell_edge_map, edge_index, x, y, x, y + 1, column_count);
                    edge_index++;
                }
            }
        }
    }

    float32 dt = 0.05f; // TODO(selim): calculate this
    for (;;)
    {
        prev_slider_value = slider_value;
        input_update(dt);
        arena_reset(frame_arena);

        if (input_is_pressed(action_quit))
        {
            should_quit = true;
        }

        Input_MouseInfo mouse = input_mouse_info();
        // Input_Key key = _input_context->keys[OS_KeyCode_MouseLeft];

        if (should_quit || os_window_should_close(window))
        {
            log_info("Exiting...");
            break;
        }

        ui_create_fixed(screen_rect())
        {
            ui_create(CutSideTop, 64)
            {
                ui_create(CutSideLeft, 512)
                {
                    ui_fill(ColorSlate900);
                    ui_pad(8);
                    ui_slider(string("Cut Off:"), 0, 1.0, &slider_value);
                }
            }
        }

        if (prev_slider_value != slider_value)
        {
            memory_zero_typed(cells, cell_count);
            // set edges
            for (int32 i = 0; i < edge_count; i++)
            {
                edges[i].is_active = random_f32(1) > slider_value;
            }

            for (int32 y = 0; y < row_count; y++)
            {
                for (int32 x = 0; x < column_count; x++)
                {
                    int32 cell_index = (y * row_count) + x;
                    Cell* cell       = &cells[cell_index];
                    if (cell->is_processed)
                        continue;

                    Color     group_color = random_color();
                    CellNode* queue       = 0;

                    CellNode* node   = arena_push_struct_zero(frame_arena, CellNode);
                    node->v          = cell;
                    node->cell_index = cell_index;
                    while (node != NULL)
                    {
                        node->v->color        = group_color;
                        node->v->is_processed = true;

                        CellEdgeMap cell_edges = cell_edge_map[node->cell_index];
                        for (int32 i = 0; i < cell_edges.edge_count; i++)
                        {
                            Edge edge = edges[cell_edges.edges[i]];
                            if (edge.is_active)
                            {
                                int32 neighbour_index = edge.to == node->cell_index ? edge.from : edge.to;
                                Cell* neighbour       = &cells[neighbour_index];
                                if (neighbour->will_be_processed || neighbour->is_processed)
                                    continue;

                                neighbour->will_be_processed = true;
                                CellNode* neighbour_node     = arena_push_struct_zero(frame_arena, CellNode);
                                neighbour_node->v            = neighbour;
                                neighbour_node->cell_index   = neighbour_index;
                                stack_push(queue, neighbour_node);
                            }
                        }
                        node = queue;
                        stack_pop(queue);
                    }
                }
            }
            // color connected groups
        }

        // draw edges
        for (int32 i = 0; i < edge_count; i++)
        {
            Edge e = edges[i];
            if (!e.is_active)
                continue;
            Vec2 p_from = add_vec2(vec2(e.from_x * circle_size, e.from_y * circle_size), cell_offset);
            Vec2 p_to   = add_vec2(vec2(e.to_x * circle_size, e.to_y * circle_size), cell_offset);
            d_line(p_from, p_to, 2, ColorWhite100);
        }

        // draw circles
        for (int32 y = 0; y < row_count; y++)
        {
            for (int32 x = 0; x < column_count; x++)
            {
                int32  cell_index = get_cell_index(x, y, column_count);
                Cell   c          = cells[cell_index];
                Vec2   p          = add_vec2(vec2(x * circle_size, y * circle_size), cell_offset);
                Circle circle     = {.center = p, .radius = circle_radius};
                d_circle(p, circle_radius, 1, c.color);

                if (intersects_circle_point(circle, mouse.screen).intersects)
                {
                    CellEdgeMap cell_edges = cell_edge_map[cell_index];
                    for (int32 i = 0; i < cell_edges.edge_count; i++)
                    {
                        Edge e      = edges[cell_edges.edges[i]];
                        Vec2 p_from = add_vec2(vec2(e.from_x * circle_size, e.from_y * circle_size), cell_offset);
                        Vec2 p_to   = add_vec2(vec2(e.to_x * circle_size, e.to_y * circle_size), cell_offset);
                        d_line(p_from, p_to, 2, ColorWhite100);
                    }
                }
            }
        }

        ui_update(dt);
        gfx_render(dt);
        os_window_update(window);
    }

    return 0;
}