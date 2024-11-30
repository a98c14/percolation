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

    const int32 row_count    = 40;
    const int32 column_count = 40;
    const int32 cell_count   = row_count * column_count;
    const Vec2  cell_offset  = vec2(-circle_size * ((float32)row_count / 2), -circle_size * ((float32)column_count / 2));
    Cell*       cells        = arena_push_array_zero(persistent_arena, Cell, cell_count);

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

        // Input_MouseInfo mouse = input_mouse_info();
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
            for (int32 y = 0; y < row_count; y++)
            {
                for (int32 x = 0; x < column_count; x++)
                {
                    int32 cell_index        = (y * row_count) + x;
                    cells[cell_index].color = color_from_vec4(vec4((float32)x / column_count, (float32)y / row_count, 1, 1));
                    for (int32 i = 0; i < EdgeDirection_COUNT; i++)
                    {
                        if (random_f32(1) > slider_value)
                        {
                            cells[cell_index].edges[i] = 1;
                        }
                        else
                        {
                            cells[cell_index].edges[i] = 0;
                        }
                    }
                    // d_arrow(vec2(x * circle_size, y * circle_size), vec2(x * circle_size, y * circle_size + 10), 1, ColorRed50);
                }
            }
        }

        // draw edges
        for (int32 y = 0; y < row_count; y++)
        {
            for (int32 x = 0; x < column_count; x++)
            {
                Cell c = cells[(y * row_count) + x];
                Vec2 p = add_vec2(vec2(x * circle_size, y * circle_size), cell_offset);
                for (int32 i = 0; i < EdgeDirection_COUNT; i++)
                {
                    if (c.edges[i])
                    {
                        Vec2 offset = mul_vec2_f32(edge_offsets[i], circle_size);
                        d_line(p, add_vec2(p, offset), 2, ColorWhite100);
                    }
                }
                // d_arrow(vec2(x * circle_size, y * circle_size), vec2(x * circle_size, y * circle_size + 10), 1, ColorRed50);
            }
        }

        // draw circles
        for (int32 y = 0; y < row_count; y++)
        {
            for (int32 x = 0; x < column_count; x++)
            {
                Cell c = cells[(y * row_count) + x];
                Vec2 p = add_vec2(vec2(x * circle_size, y * circle_size), cell_offset);
                d_circle(p, circle_radius, 1, c.color);
            }
        }

        ui_update(dt);
        gfx_render(dt);
        os_window_update(window);
    }

    return 0;
}