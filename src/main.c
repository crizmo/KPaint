#include <gb/gb.h>
#include <gbdk/font.h>
#include <gbdk/console.h>
#include <stdio.h>
#include <string.h>
#include <gb/drawing.h>

// Constants
#define CANVAS_WIDTH 18
#define CANVAS_HEIGHT 15
#define MAX_CANVASES 16

// Canvas positions on screen (centered on the Game Boy screen)
#define CANVAS_X 1
#define CANVAS_Y 2

// Game states
#define STATE_DRAWING 0
#define STATE_MENU 1

// Gameboy colors (inverted for Kindle)
#define COLOR_BLACK 0
#define COLOR_DARK_GRAY 1
#define COLOR_LIGHT_GRAY 2
#define COLOR_WHITE 3

// Tile definitions
#define EMPTY_TILE 0
#define CURSOR_TILE 1
#define COLOR_TILES_START 2  // Colors start from tile 2
#define GRID_TILE 6
#define PEN_ICON 7
#define ERASER_ICON 8
#define COLOR_INDICATOR_UP 9     // Black indicator (UP)
#define COLOR_INDICATOR_RIGHT 10 // Dark gray indicator (RIGHT)
#define COLOR_INDICATOR_DOWN 11  // White indicator (DOWN)
#define COLOR_INDICATOR_LEFT 12  // Light gray indicator (LEFT)

// Title text
#define TITLE_TEXT "KPAINT 1.0"

// Tile data
const unsigned char empty_tile[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Improved pen cursor that looks more like a pen/brush
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

// Grid tile (used for the canvas border)
const unsigned char grid_tile[] = {
    0xFF, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0xFF
};

// Tool icons for the right sidebar
const unsigned char pen_icon[] = {
    0x00, 0x00, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10,
    0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01
};

const unsigned char eraser_icon[] = {
    0x7E, 0x7E, 0x42, 0x42, 0x5A, 0x5A, 0x5A, 0x5A,
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x7E
};

// Color selection indicators with preview circles showing the color patterns
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

// Title and UI definitions
#define TITLE_TEXT "KPAINT 1.0"
#define MENU_TEXT "CLEAR"

// Color indicator preview tiles - small circles showing the color pattern
#define COLOR_PREVIEW_BLACK 13   // Preview for black (empty circle)
#define COLOR_PREVIEW_DARK 14    // Preview for dark gray
#define COLOR_PREVIEW_LIGHT 15   // Preview for light gray
#define COLOR_PREVIEW_WHITE 16   // Preview for white (filled circle)

// Preview tile patterns
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

// Global variables
UBYTE current_state = STATE_DRAWING;
UBYTE canvas[MAX_CANVASES][CANVAS_WIDTH][CANVAS_HEIGHT];
UBYTE current_canvas = 0;
UBYTE cursor_x = 0;
UBYTE cursor_y = 0;
UBYTE current_color = COLOR_BLACK; // Start with black as the default color for Kindle (appears as white)
UBYTE joypad_state = 0;
UBYTE prev_joypad_state = 0;
UBYTE color_indicator_active = 0; // Tracks whether color indicators should be shown

// Function prototypes
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
    // Initialize everything
    init();
    
    // Set up initial screen
    if (current_state == STATE_DRAWING) {
        draw_canvas_initial();
    }
    
    // Make sure cursor_x and cursor_y are initialized to valid values
    cursor_x = 0;
    cursor_y = 0;
    
    // Flag to track if menu was already drawn
    UBYTE menu_drawn = 0;
    
    // Main game loop
    while(1) {
        // Wait for VBlank at the beginning of each frame
        wait_vbl_done();
        
        // Get input
        prev_joypad_state = joypad_state;
        joypad_state = joypad();
        
        // Keep track of the state before handling input
        UBYTE previous_game_state = current_state;
        
        // Process input
        handle_input();
        
        // Update screen based on current state
        if (current_state == STATE_DRAWING) {
            // Only update what changed - no full redraw
            update_cursor_position();
            update_color_indicators(); // Update color selection indicators
            menu_drawn = 0; // Reset menu drawn flag
        } else if (current_state == STATE_MENU && !menu_drawn) {
            // Only draw menu once when entering the menu state
            draw_menu();
            menu_drawn = 1;
        }
    }
}

void init() {
    // Initialize display
    DISPLAY_ON;
    SHOW_BKG;
    
    // Initialize the font system for text display
    font_init();
    font_set(font_load(font_ibm));
    
    // Load custom tiles
    load_tiles();
    
    // Clear all canvases
    for (UBYTE i = 0; i < MAX_CANVASES; i++) {
        clear_current_canvas();
    }
    
    // Set up color palette
    // Inverted palette for Kindle: Black (darkest), Dark Gray, Light Gray, White (lightest)
    BGP_REG = 0xE4;  // Keep the same palette order but we've redefined the colors
    
    // Start with a blank screen
    cls();
}

void load_tiles() {
    // Load custom tile data
    set_bkg_data(EMPTY_TILE, 1, empty_tile);
    set_bkg_data(CURSOR_TILE, 1, cursor_tile);
    set_bkg_data(COLOR_TILES_START + COLOR_BLACK, 1, black_pixel);
    set_bkg_data(COLOR_TILES_START + COLOR_DARK_GRAY, 1, dark_gray_pixel);
    set_bkg_data(COLOR_TILES_START + COLOR_LIGHT_GRAY, 1, light_gray_pixel);
    set_bkg_data(COLOR_TILES_START + COLOR_WHITE, 1, white_pixel);
    
    // Load UI elements
    set_bkg_data(GRID_TILE, 1, grid_tile);
    set_bkg_data(PEN_ICON, 1, pen_icon);
    set_bkg_data(ERASER_ICON, 1, eraser_icon);
    set_bkg_data(COLOR_INDICATOR_UP, 1, color_indicator_up);
    set_bkg_data(COLOR_INDICATOR_RIGHT, 1, color_indicator_right);
    set_bkg_data(COLOR_INDICATOR_DOWN, 1, color_indicator_down);
    set_bkg_data(COLOR_INDICATOR_LEFT, 1, color_indicator_left);
    
    // Load color preview tiles
    set_bkg_data(COLOR_PREVIEW_BLACK, 1, preview_black);
    set_bkg_data(COLOR_PREVIEW_DARK, 1, preview_dark_gray);
    set_bkg_data(COLOR_PREVIEW_LIGHT, 1, preview_light_gray);
    set_bkg_data(COLOR_PREVIEW_WHITE, 1, preview_white);
    
    // Load color preview tiles
    set_bkg_data(COLOR_PREVIEW_BLACK, 1, preview_black);
    set_bkg_data(COLOR_PREVIEW_DARK, 1, preview_dark_gray);
    set_bkg_data(COLOR_PREVIEW_LIGHT, 1, preview_light_gray);
    set_bkg_data(COLOR_PREVIEW_WHITE, 1, preview_white);
}

void draw_canvas() {
    static UBYTE initialized = 0;
    
    // Only clear and initialize once to avoid flickering
    if (!initialized) {
        cls();
        
        // Draw the title at the top - centered
        gotoxy(5, 0);
        printf(TITLE_TEXT);
        
        // We'll create the grid and UI elements manually with tiles
        initialized = 1;
    }
    
    // Draw grid around the canvas area (with borders for the larger grid)
    // Top and bottom rows
    for (UBYTE i = 0; i <= CANVAS_WIDTH+1; i++) {
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y - 1, GRID_TILE);          // Top row
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y + CANVAS_HEIGHT, GRID_TILE); // Bottom row
    }
    
    // Left and right columns
    for (UBYTE i = 0; i < CANVAS_HEIGHT; i++) {
        set_bkg_tile_xy(CANVAS_X - 1, CANVAS_Y + i, GRID_TILE);           // Left column
        set_bkg_tile_xy(CANVAS_X + CANVAS_WIDTH, CANVAS_Y + i, GRID_TILE); // Right column
    }
    
    // Draw canvas pixels
    for (UBYTE x = 0; x < CANVAS_WIDTH; x++) {
        for (UBYTE y = 0; y < CANVAS_HEIGHT; y++) {
            UBYTE color = canvas[current_canvas][x][y];
            set_bkg_tile_xy(CANVAS_X + x, CANVAS_Y + y, COLOR_TILES_START + color);
        }
    }
    
    // Removed canvas number display as requested
    
    // Draw cursor on the canvas - static (no flashing)
    set_bkg_tile_xy(CANVAS_X + cursor_x, CANVAS_Y + cursor_y, CURSOR_TILE);
}

void draw_ui() {
    // UI is now integrated directly into draw_canvas
    // This is empty because we're handling all UI elements 
    // in the draw_canvas function for simplicity
}

void draw_menu() {
    cls();
    
    // Draw title
    gotoxy(4, 0);
    printf(TITLE_TEXT);
    
    // Draw menu box with grid tiles
    for (UBYTE y = 4; y <= 10; y++) {
        for (UBYTE x = 3; x <= 15; x++) {
            // Border
            if (y == 4 || y == 10 || x == 3 || x == 15) {
                set_bkg_tile_xy(x, y, GRID_TILE);
            } 
            // Interior
            else {
                set_bkg_tile_xy(x, y, COLOR_TILES_START + COLOR_BLACK); // Black appears as white on Kindle
            }
        }
    }
    
    // Draw menu text - properly centered and not overlapping the border
    gotoxy(5, 6);
    printf("CLEAR");
    
    gotoxy(7, 8);
    printf("A: YES");
    
    gotoxy(7, 9);
    printf("B: NO");
}

// Define BUTTON_PRESSED macro at the global level so it can be used in all functions
#define BUTTON_PRESSED(button) (((joypad_state & button) != 0) && ((prev_joypad_state & button) == 0))

void handle_input() {
    static UBYTE redraw_required = 1;
    static UBYTE prev_state = STATE_DRAWING;
    
    // Check if state has changed
    if (prev_state != current_state) {
        redraw_required = 1;
        prev_state = current_state;
    }
    
    if (current_state == STATE_DRAWING) {
        // Move cursor with D-pad
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
        
        // Draw with A button is now handled in update_cursor_position()
        
        // Change color with B + D-pad for quick color selection as requested
        if (joypad_state & J_B) {
            // B + Up: Black (for Kindle) - now BLACK on top as requested
            if (BUTTON_PRESSED(J_UP)) {
                current_color = COLOR_BLACK; // Black
                redraw_required = 1;
            }
            // B + Right: Dark Grey
            else if (BUTTON_PRESSED(J_RIGHT)) {
                current_color = COLOR_DARK_GRAY; // Dark Grey
                redraw_required = 1;
            }
            // B + Down: Complete white - now WHITE on bottom as requested
            else if (BUTTON_PRESSED(J_DOWN)) {
                current_color = COLOR_WHITE; // White
                redraw_required = 1;
            }
            // B + Left: Light Grey
            else if (BUTTON_PRESSED(J_LEFT)) {
                current_color = COLOR_LIGHT_GRAY; // Light Grey
                redraw_required = 1;
            }
        }
        
        // Change canvas with Select + D-pad
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
        
        // Open menu with Start
        if (BUTTON_PRESSED(J_START)) {
            current_state = STATE_MENU;
            redraw_required = 1;
        }
    } else if (current_state == STATE_MENU) {
        // Select menu item with A
        if (BUTTON_PRESSED(J_A)) {
            clear_current_canvas();
            current_state = STATE_DRAWING;
            // Redraw the canvas when returning from menu
            draw_canvas_initial();
            redraw_required = 1;
        }
        
        // Exit menu with B or Start
        if (BUTTON_PRESSED(J_B) || BUTTON_PRESSED(J_START)) {
            current_state = STATE_DRAWING;
            // Redraw the canvas when returning from menu
            draw_canvas_initial();
            redraw_required = 1;
        }
    }
}

// Draw the initial canvas and static elements only once
void draw_canvas_initial() {
    // Clear the screen
    cls();
    
    // Draw the title at the top - centered with the new grid size
    gotoxy(4, 0);
    printf(TITLE_TEXT);
    
    // Draw grid around the canvas area with borders for the larger grid
    // Top and bottom rows
    for (UBYTE i = 0; i <= CANVAS_WIDTH+1; i++) {
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y - 1, GRID_TILE);          // Top row
        set_bkg_tile_xy(CANVAS_X - 1 + i, CANVAS_Y + CANVAS_HEIGHT, GRID_TILE); // Bottom row
    }
    
    // Left and right columns
    for (UBYTE i = 0; i < CANVAS_HEIGHT; i++) {
        set_bkg_tile_xy(CANVAS_X - 1, CANVAS_Y + i, GRID_TILE);           // Left column
        set_bkg_tile_xy(CANVAS_X + CANVAS_WIDTH, CANVAS_Y + i, GRID_TILE); // Right column
    }
    
    // Draw all canvas pixels without the cursor
    for (UBYTE x = 0; x < CANVAS_WIDTH; x++) {
        for (UBYTE y = 0; y < CANVAS_HEIGHT; y++) {
            UBYTE color = canvas[current_canvas][x][y];
            set_bkg_tile_xy(CANVAS_X + x, CANVAS_Y + y, COLOR_TILES_START + color);
        }
    }
    
    // Initialize last cursor position to force a draw in the first update_cursor_position call
    last_cursor_x = 255; // Invalid position to force cursor draw in first update
    last_cursor_y = 255;
    
    // Don't draw the initial cursor here - it will be drawn in the first update_cursor_position call
}

// Only update what changed - cursor movement and newly drawn pixels
void update_cursor_position() {
    // If cursor moved or this is the first update (last_cursor_x == 255)
    if (cursor_x != last_cursor_x || cursor_y != last_cursor_y) {
        // Check if last position was valid and needs to be restored
        if (last_cursor_x != 255 && last_cursor_y != 255) {
            // Restore the color at the previous cursor position
            UBYTE prev_color = canvas[current_canvas][last_cursor_x][last_cursor_y];
            set_bkg_tile_xy(CANVAS_X + last_cursor_x, CANVAS_Y + last_cursor_y, COLOR_TILES_START + prev_color);
        }
        
        // Draw cursor at the new position - wait for VBlank before updating
        wait_vbl_done();
        set_bkg_tile_xy(CANVAS_X + cursor_x, CANVAS_Y + cursor_y, CURSOR_TILE);
        
        // Update last cursor position
        last_cursor_x = cursor_x;
        last_cursor_y = cursor_y;
    }
    
    // After drawing a pixel, update the canvas and redraw the cursor
    if (BUTTON_PRESSED(J_A)) {
        // Update the canvas with the selected color
        canvas[current_canvas][cursor_x][cursor_y] = current_color;
        
        // Always ensure cursor is visible on top
        wait_vbl_done();
        set_bkg_tile_xy(CANVAS_X + cursor_x, CANVAS_Y + cursor_y, CURSOR_TILE);
    }
}

// Show color selection indicators when B is held
void update_color_indicators() {
    // Position for the indicators - just above/below/left/right of the cursor
    const UBYTE indicator_pos_x = CANVAS_X + cursor_x;
    const UBYTE indicator_pos_y = CANVAS_Y + cursor_y;
    
    // When B is pressed, show the color indicators with color previews
    if (joypad_state & J_B) {
        if (!color_indicator_active) {
            // Show indicators around the cursor with their appropriate color previews
            
            // Up - Black (empty circle)
            if (cursor_y > 0) {
                set_bkg_tile_xy(indicator_pos_x, indicator_pos_y - 1, COLOR_INDICATOR_UP);
            }
            
            // Right - Dark gray
            if (cursor_x < CANVAS_WIDTH - 1) {
                set_bkg_tile_xy(indicator_pos_x + 1, indicator_pos_y, COLOR_INDICATOR_RIGHT);
            }
            
            // Down - White (filled circle)
            if (cursor_y < CANVAS_HEIGHT - 1) {
                set_bkg_tile_xy(indicator_pos_x, indicator_pos_y + 1, COLOR_INDICATOR_DOWN);
            }
            
            // Left - Light gray
            if (cursor_x > 0) {
                set_bkg_tile_xy(indicator_pos_x - 1, indicator_pos_y, COLOR_INDICATOR_LEFT);
            }
            
            color_indicator_active = 1;
        }
    }
    // When B is released, remove the indicators and restore the canvas
    else if (color_indicator_active) {
        // Restore the tiles around the cursor to their original colors
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
    // Clear current canvas (set all pixels to COLOR_BLACK which appears as white on Kindle)
    for (UBYTE x = 0; x < CANVAS_WIDTH; x++) {
        for (UBYTE y = 0; y < CANVAS_HEIGHT; y++) {
            canvas[current_canvas][x][y] = COLOR_BLACK; 
        }
    }
}
