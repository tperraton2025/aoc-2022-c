#ifndef ANSI_CMD
#define ANSI_CMD

#define HOME "\e[H" // moves cursor to home position (0, 0)
#define MCUR_FMT "\e[%ld;%ldH"
#define MCUR(line, col) "\e[" line ";" col "H"
#define MCUR1_FMT "\e[%d%c" //  moves cursor up # lines

#define MCURUP(_n) "\e["_n \
                   "A" //  moves cursor up # lines
#define MCURDO(_n) "\e["_n \
                   "B" //  moves cursor down # lines
#define MCURRI(_n) "\e["_n \
                   "C" //  moves cursor right # columns
#define MCURLE(_n) "\e["_n \
                   "D" //  moves cursor left # columns

#define MCURNB(_n) "\e["_n \
                   "D" //  moves cursor to beginning of next line, # lines down
#define MCURPB(_n) "\e["_n \
                   "F" //  moves cursor to beginning of previous line, # lines up
#define MCURCO(_n) "\e["_n \
                   "G"     //  moves cursor to column #
#define CUR_REPORT "\e[6n" // request cursor position (reports as "\e"[#;#R")
#define MCURUPSCRL "\e M"  //  moves cursor one line up, scrolling if needed
#define SAVECURPOS1 "\e 7" // save cursor position (DEC)
#define CURPOSREST1 "\e 8" // restores the cursor to the last saved position (DEC)
#define SAVECURPOS2 "\e[s" // save cursor position (SCO)
#define CURPOSREST2 "\e[u" // restores the cursor to the last saved position (SCO)

#define ERASE_DISPLAY "\e[2J"         //	erase in display (same as ESC[0J)
#define ERASE_ALL_FROM_CURSOR "\e[0J" //	erase from cursor until end of screen
#define ERASE_BACK "\e[1J"            //	erase from cursor to beginning of screen
#define ERASE_FULL_SCREEN "\e[2J"     //	erase entire screen
#define ERASE_SAVED_LINES "\e[3J"     //	erase saved lines
#define ERASE_IN_LINE "\e[K"          //	erase in line (same as ESC[0K)
#define ERASE_LINE_FROM_CR "\e[0K"    //	erase from cursor to end of line
#define ERASE_LINE_TO_CURS "\e[1K"    //	erase start of line to the cursor
#define ERASE_FULL_LINE "\e[2K"       //	erase the entire line

#define MODE_SEL(value) "\e[=" value "h" // 	Changes the screen width or type to the mode specified by value.
#define MODE_40x25_BW "\e[=0h"           // 40 x 25 monochrome (text)
#define MODE_40x25_COL "\e[=1h"          // 40 x 25 color (text)
#define MODE_80x25_BW "\e[=2h"           // 80 x 25 monochrome (text)
#define MODE_80x25_COL "\e[=3h"          // 80 x 25 color (text)
#define MODE_320x200_4BCOL "\e[=4h"      // 320 x 200 4-color (graphics)
#define MODE_320x200_BW "\e[=5h"         // 320 x 200 monochrome (graphics)
#define MODE_640x200_BW "\e[=6h"         // 640 x 200 monochrome (graphics)
#define MODE_EN_WRAP "\e[=7h"            // Enables line wrapping
#define MODE_320x200_COL "\e[=13h"       // 320 x 200 color (graphics)
#define MODE_640x200_COL "\e[=14h"       // 640 x 200 color (16-color graphics)
#define MODE_640x350_BW "\e[=15h"        // 640 x 350 monochrome (2-color graphics)
#define MODE_640x350_16COL "\e[=16h"     // 640 x 350 color (16-color graphics)
#define MODE_640x480_BW "\e[=17h"        // 640 x 480 monochrome (2-color graphics)
#define MODE_640x480_COL "\e[=18h"       // 640 x 480 color (16-color graphics)
#define MODE_320x200_256COL "\e[=19h"    // 320 x 200 color (256-color graphics)
#define MODE "\e[={value}l"              // 	Resets the mode by using the same values that Set Mode uses, except for 7, which disables line wrapping. The last character in this escape sequence is a lowercase L.

#endif