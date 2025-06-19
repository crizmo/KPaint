# KPaint

A Game Boy drawing application optimized for Kindle, inspired by GB Paint by Bownly. Created using C and GBDK. The application features a clean interface with a static cursor and visual color selection indicators.

## Features

- Draw on an 18x15 pixel canvas
- Use the Game Boy's 4-color palette (optimized for Kindle display)
- Simple menu system to clear the canvas
- Switch between multiple canvases (up to 16)
- Simple controls for drawing and color selection
- Visual color selection indicators when holding B button
- Static pen cursor with no flickering

## Controls

- **D-pad**: Move the cursor
- **A**: Draw pixel
- **B + D-pad**: Switch colors
  - **B + Up**: Select Black (appears white on Kindle)
  - **B + Right**: Select Dark Gray
  - **B + Down**: Select White (appears black on Kindle)
  - **B + Left**: Select Light Gray
- **Start**: Open clear canvas menu

## Building

### Prerequisites

1. Install [GBDK](https://github.com/gbdk-2020/gbdk-2020) - the Game Boy Development Kit
2. Ensure you have a C compiler (gcc) installed on your system

### Setting up GBDK

There are two ways to configure GBDK for this project:

**Relative Path (Default)**: Place the gbdk folder one directory up from KPaint
   ```
   parent_folder/
   ├── gbdk/
   └── KPaint/
   ```


### Building the ROM

There are two ways to build the ROM:

#### Using the build script (recommended)

```bash
# Make the script executable (if not already)
chmod +x build.sh

# Run the build script
./build.sh
```

#### Using make

```bash
# Use the Makefile
make
```

The ROM will be created in `build/kpaint.gb`

### Troubleshooting

If you encounter include file errors:
1. Make sure GBDK is installed correctly
2. Verify the GBDKDIR environment variable is set or the gbdk folder is in the correct relative location
3. Check that include paths in Makefile and build.sh are correctly set
4. The GBDK header files are organized in subdirectories:
   - `<gb/gb.h>` - Basic Game Boy functionality
   - `<gbdk/font.h>` - Font handling
   - `<gbdk/console.h>` - Console functions like gotoxy

## Running

You can run the ROM in any Game Boy emulator like BGB, Emulicious, or on real hardware with a flashcart. The application is specifically optimized for Kindle display, with inverted color mapping for better visibility on e-ink screens.

## Credits

- Inspired by GB Paint by Bownly
- Created using [GBDK](https://github.com/gbdk-2020/gbdk-2020)
