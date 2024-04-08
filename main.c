#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

// returns the distance a ray needs to travel to hit a wall on a 2D grid
// if it doesn't hit a wall, returns the maximum distance the ray is allowed to travel
float castRayDDA
(
    // the starting position of the ray (x/y coordinates)
    Vector2 start_pos,
    // the direction the ray is cast in (normalized unit vector)
    Vector2 direction,
    // a 2D grid of cells that can be marked as wall
    // NOTE: the pointer type may differ depending on how the map is structured
    int** map,
    // the number of rows in the grid
    int map_rows,
    // the number of columns in the grid
    int map_cols,
    // the side length of one grid cell
    float tile_size,
    // the maximum distance the ray is allowed to travel
    float max_distance
) {
    // calculate the direction to step in pixel space each iteration relative to one
    // grid cell
    // these are constant because the ray follows a linear line which means that the
    // directions to move in never change
    // multiplying the values with the tile_size gives the distance the ray needs to
    // travel to cross one grid cell
    const Vector2 step_dir = {
        .x = sqrtf(1.0f + (direction.y / direction.x) * (direction.y / direction.x)),
        .y = sqrtf(1.0f + (direction.x / direction.y) * (direction.x / direction.y)),
    };

    // calculate the initial grid space coordinates the ray starts from
    // they will be incremented and decremented during iteration to indicate the
    // coordinates of the cell the ray is currently in
    // these are used to index the map array to check if the current cell is a wall
    int cur_map_x = (int)(start_pos.x / tile_size);
    int cur_map_y = (int)(start_pos.y / tile_size);

    // calculate the direction to step in in grid space each iteration
    // these are constant because the ray follows a linear line which means that the
    // directions to move in never change
    // NOTE: to improve performance, this may be done together with calculating the
    //       initial ray_len to reduce the number of individual checks from 4 to 2
    //       step_x and step_y cannot be const in this case
    const int step_x = (direction.x < 0.0f) ? -1 : 1;
    const int step_y = (direction.y < 0.0f) ? -1 : 1;

    // calculate the initial lengths that the ray needs to travel in each direction to
    // hit the first grid line
    // NOTE: if step_x, step_y and ray_len are calculated in the same step, the if
    //       conditions need to look like this:
    //           if (direction.x < 0.0f) {...} else {...}
    //           if (direction.y < 0.0f) {...} else {...}
    //       initialize both step_x and ray_len.x as well as step_y and ray_len.y
    //       respectively if the corresponding conditions are met
    Vector2 ray_len;
    if (step_x == -1) {
        ray_len.x = (start_pos.x - (float)cur_map_x * tile_size) * step_dir.x;
    } else {
        ray_len.x = ((float)(cur_map_x + 1) * tile_size - start_pos.x) * step_dir.x;
    }
    if (step_y == -1) {
        ray_len.y = (start_pos.y - (float)cur_map_y * tile_size) * step_dir.y;
    } else {
        ray_len.y = ((float)(cur_map_y + 1) * tile_size - start_pos.y) * step_dir.y;
    }
    
    // denotes if the ray has hit a wall during travel through the grid
    // used to break out of the loop that calculates the total distance traveled
    bool has_hit_wall = false;

    // is set to the larger value of ray_len.x or ray_len.y each iteration to simplify
    // checks and calculating the end_pos as well as to not overstep if the ray hits a
    // wall
    float distance = 0.0f;

    while (!has_hit_wall && distance < max_distance) {
        // check if the next grid line that the ray hits is horizontal or vertical
        if (ray_len.x < ray_len.y) {
            // step in the horizontal direction in grid space
            cur_map_x += step_x;

            // cache the distance to not overstep if the ray hits a wall
            distance = ray_len.x;

            // accumulate the distance traveled on the x-axis in pixel space
            // the step_dir is multiplied by the tile_size to get the length in
            // pixel space
            ray_len.x += step_dir.x * tile_size;
        } else {
            // step in the vertical direction in grid space
            cur_map_y += step_y;

            // cache the distance to not overstep if the ray hits a wall
            distance = ray_len.y;

            // accumulate the distance traveled on the y-axis in pixel space
            // the step_dir is multiplied by the tile_size to get the length in
            // pixel space
            ray_len.y += step_dir.y * tile_size;
        }

        // check the array bounds to avoid accessing invalid memory
        if
        (
            cur_map_x >= 0 && cur_map_x < map_cols &&
            cur_map_y >= 0 && cur_map_y < map_rows
        ) {
            // check if the grid cell at the current ray location in grid space
            // is marked as a wall
            // NOTE: depending on the type of the values stored in the array,
            //       this check may access fields or use different logic to
            //       determine if a grid cell is marked as a wall
            if (map[cur_map_y][cur_map_x] == 1) {
                // set has_hit_wall to true to break out of the loop on the next
                // iteration and retain the correct value for distance
                has_hit_wall = true;
            }
        }
    }

    // return the distance the ray has traveled if it has hit a wall,
    // otherwise return the maximum distance the ray is allowed to travel
    return (has_hit_wall) ? distance : max_distance;
}

void drawDottedLine(Vector2 start_pos, Vector2 end_pos, Color color);

int main(void) {
    const int screen_width = 800;
    const int screen_height = 800;

    InitWindow(screen_width, screen_height, "raycasting");

    const int map_rows = 80;
    const int map_cols = 80;

    int** map = malloc(sizeof (int*) * map_rows);
    for (int i = 0; i < map_rows; i++) {
        map[i] = calloc(map_cols, sizeof (int));
    }

    const float tile_size = 20.0f;

    Vector2 origin_pos = { (float)screen_width / 2.0f, (float)screen_height / 2.0f };
    Vector2 target_pos = GetMousePosition();

    const float origin_spd = 8.0f;

    Vector2 ray_pos = { -100.0f, -100.0f };
    const float max_ray_len = 1000.0f;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_W)) origin_pos.y -= origin_spd;
        if (IsKeyDown(KEY_A)) origin_pos.x -= origin_spd;
        if (IsKeyDown(KEY_S)) origin_pos.y += origin_spd;
        if (IsKeyDown(KEY_D)) origin_pos.x += origin_spd;
        
        target_pos = GetMousePosition();

        const int tile_x = (int)(target_pos.x / tile_size);
        const int tile_y = (int)(target_pos.y / tile_size);

        if
        (
            tile_x >= 0 && tile_x < map_cols &&
            tile_y >= 0 && tile_y < map_rows
        ) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) map[tile_y][tile_x] = 1;
            else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) map[tile_y][tile_x] = 0;
        }

        if (IsKeyPressed(KEY_C)) {
            for (int i = 0; i < map_rows; i++) {
                for (int j = 0; j < map_cols; j++) {
                    map[i][j] = 0;
                }
            }
        }

        const Vector2 ray_dir = Vector2Normalize(Vector2Subtract(target_pos, origin_pos));

        const float intersection_distance = castRayDDA(
            origin_pos,
            ray_dir,
            map,
            map_rows,
            map_cols,
            tile_size,
            max_ray_len
        );

        ray_pos = Vector2Add(origin_pos, Vector2Scale(ray_dir, intersection_distance));

        BeginDrawing();

        ClearBackground(BLACK);

        // draw horizontal grid lines
        for (int i = 0; i < map_rows; i++) {
            DrawLine(0, i * (int)tile_size, screen_width, i * (int)tile_size, GRAY);
        }
        // draw vertical grid lines
        for (int i = 0; i < map_cols; i++) {
            DrawLine(i* (int)tile_size, 0, i * (int)tile_size, screen_height, GRAY);
        }
        // draw tiles
        for (int i = 0; i < map_rows; i++) {
            for (int j = 0; j < map_cols; j++) {
                if (map[i][j] == 1) {
                    DrawRectangle(
                        (int)((float)j * tile_size),
                        (int)((float)i * tile_size),
                        (int)tile_size,
                        (int)tile_size,
                        WHITE
                    );
                }
            }
        }
        // draw line from origin to target
        DrawLineV(origin_pos, target_pos, YELLOW);      
        // draw line that continues after target
        drawDottedLine(
            target_pos,
            Vector2Add(target_pos, Vector2Scale(ray_dir, 100.0f * max_ray_len)),
            YELLOW
        );
        // draw origin
        DrawCircleV(origin_pos, 5.0f, RED);
        // draw target
        DrawCircleV(target_pos, 5.0f, GREEN);
        // draw raycast intersection point
        DrawCircleV(ray_pos, 2.0f, BLUE);
        DrawCircleLinesV(ray_pos, 6.0f, BLUE);

        // draw text
        const int font_size = 20;
        const int margin = 5;

        const int buf_size = 50;

        char ori_buf[buf_size];
        snprintf(ori_buf, buf_size, "ORI: %.1f / %.1f", origin_pos.x, origin_pos.y);

        char tar_buf[buf_size];
        snprintf(tar_buf, buf_size, "TAR: %.1f / %.1f", target_pos.x, target_pos.y);

        char ray_buf[buf_size];
        snprintf(ray_buf, buf_size, "RAY: %.1f / %.1f", ray_pos.x, ray_pos.y);

        char len_buf[buf_size];
        snprintf(len_buf, buf_size, "LEN: %.4f", intersection_distance);

        DrawRectangle(
            0,
            0,
            220,
            5 + 4 * font_size + 4 * margin,
            BLACK
        );
        DrawText(ori_buf, 5, 5, font_size, RED);
        DrawText(tar_buf, 5, 5 + font_size + margin, font_size, GREEN);
        DrawText(ray_buf, 5, 5 + 2 * font_size + 2 * margin, font_size, BLUE);
        DrawText(len_buf, 5, 5 + 3 * font_size + 3 * margin, font_size, YELLOW);

        const int tooltip_x = screen_width - 280;

        DrawRectangle(
            tooltip_x - 5,
            0,
            285,
            5 + 5 * font_size + 5 * margin,
            BLACK
        );
        DrawText(
            "[wasd] to move origin",
            tooltip_x,
            5,
            font_size,
            WHITE
        );
        DrawText(
            "[mouse] to move target",
            tooltip_x,
            5 + font_size + margin,
            font_size,
            WHITE
        );
        DrawText(
            "[left click] to paint tile",
            tooltip_x,
            5 + 2 * font_size + 2 * margin,
            font_size,
            WHITE
        );
        DrawText(
            "[right click] to remove tile",
            tooltip_x,
            5 + 3 * font_size + 3 * margin,
            font_size,
            WHITE
        );
        DrawText(
            "[c] to clear tiles",
            tooltip_x,
            5 + 4 * font_size + 4 * margin,
            font_size,
            WHITE
        );

        EndDrawing();
    }

    // uninitialize
    for (int i = 0; i < map_rows; i++) {
        free(map[i]);
    }
    free(map);

    CloseWindow();
    return EXIT_SUCCESS;
}

void drawDottedLine(Vector2 start_pos, Vector2 end_pos, Color color) {
    const float len = 4.0f;
    const Vector2 dir = Vector2Normalize(Vector2Subtract(end_pos, start_pos));
    const float full_distance = Vector2Distance(start_pos, end_pos);
    const int steps = (int)(full_distance / (len * 2.0f));

    for (int i = 0; i < steps; i++) {
        start_pos = Vector2Add(start_pos, Vector2Scale(dir, len));
        end_pos = Vector2Add(start_pos, Vector2Scale(dir, len));
        if (i % 2 == 0) {
            DrawLineV(start_pos, end_pos, color);
        }
    }
}
