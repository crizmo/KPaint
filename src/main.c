#include <gb/gb.h>
#include <gbdk/font.h>
#include <gbdk/console.h>
#include <stdio.h>
#include <string.h>
#include <gb/drawing.h>

#define CANVAS_WIDTH 18
#define CANVAS_HEIGHT 15
#define MAX_CANVASES 16

#define CANVAS_X 1
#define CANVAS_Y 2

#define STATE_DRAWING 0
#define STATE_MENU 1

#define COLOR_BLACK 0
#define COLOR_DARK_GRAY 1
#define COLOR_LIGHT_GRAY 2
#define COLOR_WHITE 3

#define EMPTY_TILE 0
#define CURSOR_TILE 1
#define COLOR_TILES_START 2
#define GRID_TILE 6
#define PEN_ICON 7
#define ERASER_ICON 8
#define COLOR_INDICATOR_UP 9
#define COLOR_INDICATOR_RIGHT 10
#define COLOR_INDICATOR_DOWN 11
#define COLOR_INDICATOR_LEFT 12

#define TITLE_TEXT "KPAINT 1.0"

const unsigned char empty_tile[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char cursor_tile[] = {
    0x00, 0x00, 0x60, 0x60, 0x70, 0x70, 0x38, 0x38,
    0x1C, 0x1C, 0x0E, 0x0E, 0x07, 0x07, 0x03, 0x03
};

const unsigned char black_pixel[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char dark_gray_pixel[] = {
    0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA,
    0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA
};

const unsigned char light_gray_pixel[] = {
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};

const unsigned char white_pixel[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const unsigned char grid_tile[] = {
    0xFF, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0xFF
};

const unsigned char pen_icon[] = {
    0x00, 0x00, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10,
    0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01
};

const unsigned char eraser_icon[] = {
    0x7E, 0x7E, 0x42, 0x42, 0x5A, 0x5A, 0x5A, 0x5A,
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x7E
};
const unsigned char color_indicator_up[] = {
    0x18, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0xFF, 0xFF,
    0x18, 0x18, 0x18, 0x18, 0x3C, 0x3C, 0x3C, 0x3C  // Up = Blank (BLACK/white) circle at bottom
};

const unsigned char color_indicator_right[] = {
    0x00, 0x00, 0x0C, 0x0C, 0x06, 0x06, 0xFF, 0xFF,
    0xFF, 0xFF, 0x06, 0x06, 0x0C, 0x0C, 0x1C, 0x1C  // Right = Dark gray pattern circle at bottom right
};

const unsigned char color_indicator_down[] = {
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0xFF, 0xFF, 0x7E, 0x7E, 0x7E, 0x7E, 0xFF, 0xFF  // Down = Filled (WHITE/black) circle at bottom
};

const unsigned char color_indicator_left[] = {
    0x00, 0x00, 0x30, 0x30, 0x60, 0x60, 0xFF, 0xFF,
    0xFF, 0xFF, 0x60, 0x60, 0x38, 0x38, 0x38, 0x38  // Left = Light gray pattern circle at bottom left
};

#define TITLE_TEXT "KPAINT 1.0"
#define MENU_TEXT "CLEAR"

#define COLOR_PREVIEW_BLACK 13
#define COLOR_PREVIEW_DARK 14
#define COLOR_PREVIEW_LIGHT 15
#define COLOR_PREVIEW_WHITE 16

const unsigned char preview_black[] = {
    0x3C, 0x3C, 0x42, 0x42, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x42, 0x42, 0x3C, 0x3C
};

const unsigned char preview_dark_gray[] = {
    0x3C, 0x3C, 0x42, 0x42, 0x95, 0x95, 0xAA, 0xAA,
    0x95, 0x95, 0xAA, 0xAA, 0x42, 0x42, 0x3C, 0x3C
};

const unsigned char preview_light_gray[] = {
    0x3C, 0x3C, 0x42, 0x42, 0xAA, 0xAA, 0x55, 0x55,
    0xAA, 0xAA, 0x55, 0x55, 0x42, 0x42, 0x3C, 0x3C
};

const unsigned char preview_white[] = {
    0x3C, 0x3C, 0x7E, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x7E, 0x3C, 0x3C
};

UBYTE current_state = STATE_DRAWING;
UBYTE canvas[MAX_CANVASES][CANVAS_WIDTH][CANVAS_HEIGHT];
UBYTE current_canvas = 0;
UBYTE cursor_x = 0;
UBYTE cursor_y = 0;
UBYTE current_color = COLOR_BLACK;
UBYTE joypad_state = 0;
UBYTE prev_joypad_state = 0;
UBYTE color_indicator_active = 0;
void init();
void load_tiles();
void draw_canvas();
void draw_ui();
void draw_menu();
void handle_input();
void clear_current_canvas();
void draw_canvas_initial();
void update_cursor_position();
void update_color_indicators();

UBYTE last_cursor_x = 0;
UBYTE last_cursor_y = 0;

void main() {
    init();
    
    if (current_state == STATE_DRAWING) {
        draw_canvas_initial();
    }
    
    cursor_x = 0;
    cursor_y = 0;
    
    UBYTE menu_drawn = 0;
    while(1) {
        wait_vbl_done();
        
        prev_joypad_state = joypad_state;
        joypad_state = joypad();
        
        UBYTE previous_game_state = current_state;
        
        handle_input();
        if (current_state == STATE_DRAWING) {
            update_cursor_position();
            update_color_indicators();
            menu_drawn = 0;
        } else if (current_state == STATE_MENU && !menu_drawn) {
            draw_menu();
            menu_drawn = 1;
        }
    }
}

void init() {
    DISPLAY_ON;
    SHOW_BKG;
    
    font_init();
    font_set(font_load(font_ibm));
    
    load_tiles();
    
    for (UBYTE i = 0; i < MAX_CANVASES; i++) {
        clear_current_canvas();
    }
    
    BGP_REG = 0xE4;
    
    cls();
}

void load_tiles() {
    set_bkg_data(EMPTY_TILE, 1, empty_tile);
    set_bkg_data(CURSOR_TILE, 1, cursor_tile);
    set_bkg_data(COLOR_TILES_START + COLOR_BLACK, 1, black_pixel);
    set_bkg_data(COLOR_TILES_START + COLOR_DARK_GRAY, 1, dark_gray_pixel);
    set_bkg_data(COLOR_TILES_START + COLOR_LIGHT_GRAY, 1, light_gray_pixel);
    set_bkg_data(COLOR_TILES_START + COLOR_WHITE, 1, white_pixel);
    
    set_bkg_data(GRID_TILE, 1, grid_tile);
    set_bkg_data(PEN_ICON, 1, pen_icon);
    set_bkg_data(ERASER_ICON, 1, eraser_icon);
    set_bkg_data(COLOR_INDICATOR_UP, 1, color_indicator_up);
    set_bkg_data(COLOR_INDICATOR_RIGHT, 1, color_indicator_right);
    set_bkg_data(COLOR_INDICATOR_DOWN, 1, color_indicator_down);
    set_bkg_data(COLOR_INDICATOR_LEFT, 1, color_indicator_left);
    
    set_bkg_data(COLOR_PREVIEW_BLACK, 1, preview_black);
    set_bkg_data(COLOR_PREVIEW_DARK, 1, preview_dark_gray);
    set_bkg_data(COLOR_PREVIEW_LIGHT, 1, preview_light_gray);
    set_bkg_data(COLOR_PREVIEW_WHITE, 1, preview_white);
    
    set_bkg_data(COLOR_PREVIEW_BLACK, 1, preview_black);
    set_bkg_data(COLOR_PREVIEW_DARK, 1, preview_dark_gray);
    set_bkg_data(COLOR_PREVIEW_LIGHT, 1, preview_light_gray);
    set_bkg_data(COLOR_PREVIEW_WHITE, 1, preview_white);
}

void draw_canvas() {
    static UBYTE initialized = 0;
    
    if (!initialized) {
        cls();
        
        gotoxy(5, 0);
        printf(TITLE_TEXT);
        
        initialized = 1;
    }
    
    for (UBYTE i = 0; i <= CANVAS_WIDTH+1; i++) {
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y - 1, GRID_TILE);          
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y + CANVAS_HEIGHT, GRID_TILE); 
    }
    for (UBYTE i = 0; i < CANVAS_HEIGHT; i++) {
        set_bkg_tile_xy(CANVAS_X - 1, CANVAS_Y + i, GRID_TILE);           
        set_bkg_tile_xy(CANVAS_X + CANVAS_WIDTH, CANVAS_Y + i, GRID_TILE); 
    }
    
    for (UBYTE x = 0; x < CANVAS_WIDTH; x++) {
        for (UBYTE y = 0; y < CANVAS_HEIGHT; y++) {
            UBYTE color = canvas[current_canvas][x][y];
            set_bkg_tile_xy(CANVAS_X + x, CANVAS_Y + y, COLOR_TILES_START + color);
        }
    }
    
    set_bkg_tile_xy(CANVAS_X + cursor_x, CANVAS_Y + cursor_y, CURSOR_TILE);
}

void draw_ui() {
}

void draw_menu() {
    cls();
    
    gotoxy(4, 0);
    printf(TITLE_TEXT);
    
    for (UBYTE y = 4; y <= 10; y++) {
        for (UBYTE x = 3; x <= 15; x++) {
            if (y == 4 || y == 10 || x == 3 || x == 15) {
                set_bkg_tile_xy(x, y, GRID_TILE);
            } 
            else {
                set_bkg_tile_xy(x, y, COLOR_TILES_START + COLOR_BLACK);
            }
        }
    }
    
    gotoxy(5, 6);
    printf("CLEAR");
    
    gotoxy(7, 8);
    printf("A: YES");
    
    gotoxy(7, 9);
    printf("B: NO");
}


#define BUTTON_PRESSED(button) (((joypad_state & button) != 0) && ((prev_joypad_state & button) == 0))

void handle_input() {
    static UBYTE redraw_required = 1;
    static UBYTE prev_state = STATE_DRAWING;
    
    if (prev_state != current_state) {
        redraw_required = 1;
        prev_state = current_state;
    }
    
    if (current_state == STATE_DRAWING) {
        if (!(joypad_state & J_B) && !(joypad_state & J_SELECT)) {
            if (BUTTON_PRESSED(J_LEFT) && cursor_x > 0) {
                cursor_x--;
            }
            if (BUTTON_PRESSED(J_RIGHT) && cursor_x < CANVAS_WIDTH - 1) {
                cursor_x++;
            }
            if (BUTTON_PRESSED(J_UP) && cursor_y > 0) {
                cursor_y--;
            }
            if (BUTTON_PRESSED(J_DOWN) && cursor_y < CANVAS_HEIGHT - 1) {
                cursor_y++;
            }
        }
        
        if (joypad_state & J_B) {
            if (BUTTON_PRESSED(J_UP)) {
                current_color = COLOR_BLACK;
                redraw_required = 1;
            }
            else if (BUTTON_PRESSED(J_RIGHT)) {
                current_color = COLOR_DARK_GRAY;
                redraw_required = 1;
            }
            else if (BUTTON_PRESSED(J_DOWN)) {
                current_color = COLOR_WHITE;
                redraw_required = 1;
            }
            else if (BUTTON_PRESSED(J_LEFT)) {
                current_color = COLOR_LIGHT_GRAY;
                redraw_required = 1;
            }
        }
        if (joypad_state & J_SELECT) {
            if (BUTTON_PRESSED(J_LEFT) && current_canvas > 0) {
                current_canvas--;
                redraw_required = 1;
            }
            if (BUTTON_PRESSED(J_RIGHT) && current_canvas < MAX_CANVASES - 1) {
                current_canvas++;
                redraw_required = 1;
            }
        }
        
        if (BUTTON_PRESSED(J_START)) {
            current_state = STATE_MENU;
            redraw_required = 1;
        }
    } else if (current_state == STATE_MENU) {
        if (BUTTON_PRESSED(J_A)) {
            clear_current_canvas();
            current_state = STATE_DRAWING;
            draw_canvas_initial();
            redraw_required = 1;
        }
        
        if (BUTTON_PRESSED(J_B) || BUTTON_PRESSED(J_START)) {
            current_state = STATE_DRAWING;
            draw_canvas_initial();
            redraw_required = 1;
        }
    }
}

void draw_canvas_initial() {
    cls();
    
    gotoxy(4, 0);
    printf(TITLE_TEXT);
    
    for (UBYTE i = 0; i <= CANVAS_WIDTH+1; i++) {
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y - 1, GRID_TILE);          
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y + CANVAS_HEIGHT, GRID_TILE); 
    }
    for (UBYTE i = 0; i < CANVAS_HEIGHT; i++) {
        set_bkg_tile_xy(CANVAS_X - 1, CANVAS_Y + i, GRID_TILE);           
        set_bkg_tile_xy(CANVAS_X + CANVAS_WIDTH, CANVAS_Y + i, GRID_TILE); 
    }
    
    for (UBYTE x = 0; x < CANVAS_WIDTH; x++) {
        for (UBYTE y = 0; y < CANVAS_HEIGHT; y++) {
            UBYTE color = canvas[current_canvas][x][y];
            set_bkg_tile_xy(CANVAS_X + x, CANVAS_Y + y, COLOR_TILES_START + color);
        }
    }
    
    last_cursor_x = 255; 
    last_cursor_y = 255;
}

void update_cursor_position() {
    if (cursor_x != last_cursor_x || cursor_y != last_cursor_y) {
        if (last_cursor_x != 255 && last_cursor_y != 255) {
            UBYTE prev_color = canvas[current_canvas][last_cursor_x][last_cursor_y];
            set_bkg_tile_xy(CANVAS_X + last_cursor_x, CANVAS_Y + last_cursor_y, COLOR_TILES_START + prev_color);
        }
        
        wait_vbl_done();
        set_bkg_tile_xy(CANVAS_X + cursor_x, CANVAS_Y + cursor_y, CURSOR_TILE);
        
        last_cursor_x = cursor_x;
        last_cursor_y = cursor_y;
    }
    if (BUTTON_PRESSED(J_A)) {
        canvas[current_canvas][cursor_x][cursor_y] = current_color;
        
        wait_vbl_done();
        set_bkg_tile_xy(CANVAS_X + cursor_x, CANVAS_Y + cursor_y, CURSOR_TILE);
    }
}

void update_color_indicators() {
    const UBYTE indicator_pos_x = CANVAS_X + cursor_x;
    const UBYTE indicator_pos_y = CANVAS_Y + cursor_y;
    
    if (joypad_state & J_B) {
        if (!color_indicator_active) {
            if (cursor_y > 0) {
                set_bkg_tile_xy(indicator_pos_x, indicator_pos_y - 1, COLOR_INDICATOR_UP);
            }
            
            if (cursor_x < CANVAS_WIDTH - 1) {
                set_bkg_tile_xy(indicator_pos_x + 1, indicator_pos_y, COLOR_INDICATOR_RIGHT);
            }
            
            if (cursor_y < CANVAS_HEIGHT - 1) {
                set_bkg_tile_xy(indicator_pos_x, indicator_pos_y + 1, COLOR_INDICATOR_DOWN);
            }
            
            if (cursor_x > 0) {
                set_bkg_tile_xy(indicator_pos_x - 1, indicator_pos_y, COLOR_INDICATOR_LEFT);
            }
            
            color_indicator_active = 1;
        }
    }
    else if (color_indicator_active) {
        if (cursor_y > 0) { // Up
            UBYTE color = canvas[current_canvas][cursor_x][cursor_y - 1];
            set_bkg_tile_xy(indicator_pos_x, indicator_pos_y - 1, COLOR_TILES_START + color);
        }
        
        if (cursor_x < CANVAS_WIDTH - 1) { // Right
            UBYTE color = canvas[current_canvas][cursor_x + 1][cursor_y];
            set_bkg_tile_xy(indicator_pos_x + 1, indicator_pos_y, COLOR_TILES_START + color);
        }
        
        if (cursor_y < CANVAS_HEIGHT - 1) { // Down
            UBYTE color = canvas[current_canvas][cursor_x][cursor_y + 1];
            set_bkg_tile_xy(indicator_pos_x, indicator_pos_y + 1, COLOR_TILES_START + color);
        }
        
        if (cursor_x > 0) { // Left
            UBYTE color = canvas[current_canvas][cursor_x - 1][cursor_y];
            set_bkg_tile_xy(indicator_pos_x - 1, indicator_pos_y, COLOR_TILES_START + color);
        }
        
        color_indicator_active = 0;
    }
}

void clear_current_canvas() {
    for (UBYTE x = 0; x < CANVAS_WIDTH; x++) {
        for (UBYTE y = 0; y < CANVAS_HEIGHT; y++) {
            canvas[current_canvas][x][y] = COLOR_BLACK; 
        }
    }
}
