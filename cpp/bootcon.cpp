#include <stdint.h>
#include <bootcon.h>

G2BootConsole::G2BootConsole(uint8_t *ifb, psf2_t *ifont, uint32_t iwidth, uint32_t iheight, uint32_t iscan) {
	fb = ifb;
	font = ifont;
	width = iwidth;
	height = iheight;
	scanline = iscan;
	currentRow = currentColumn = 0;
	// dead simple initialization
}

bool G2BootConsole::putglyph(const char s, int kx, int ky) {
	// Here be dragons!
	// Adapted from the BOOTBOOT sample kernel's puts() method, which was
	// some of the worst code I've ever seen in my entire life. After reformatting it,
	// I reduced it down to this single helper method,
	// which paints a character directly to an x,y coordinate.
	// You should not have to call this directly except for ASCII art
    uint32_t x, y, line, mask, offs;
    int bpl = (font->width + 7) / 8;
   	
    unsigned char *glyph = (unsigned char *)font + font->headersize +
        (s > 0 && (uint32_t)(s) < font->numglyph ? s : 0) *
        font->bytesperglyph;
    offs = (kx * (font->width + 1) * 4) + (ky * (font->height + 1) * width * 4);
    
    for(y=0; y < font->height; y++) {
        line = offs;
        mask = 1 << (font->width - 1);
        
        for(x = 0; x < font->width; x++) {
            *((uint32_t*)((uint64_t)fb + line)) = ((int)*glyph) & (mask) ? 0xFFFFFF : 0;
            mask >>= 1;
            line += 4;
        }
        
        *((uint32_t*)((uint64_t)fb+line)) = 0;
        glyph += bpl;
        offs += scanline;
    }
    return true;
}

// What a load of crap that was

void G2BootConsole::putchar(const char s) {
	if (currentColumn >= width / (font->width + 1) || s == '\n') {
		// wrap to next line
		currentRow++;
		currentColumn = 0; 
	}
	if (currentRow >= height / (font->height + 1)) {
		// wrap back to start
		// TODO: implement scrolling with memcpy()
		currentRow = currentColumn = 0;
	}
	
	if (s != '\n' && s != '\t' && s != '\v' && s != '\f' && s != '\r') {
		putglyph(s, currentColumn, currentRow);
		currentColumn++;
	}
}

int G2BootConsole::puts(const char *s) {
	// just call putchar over and over to print a null-terminated string
	int count = 0;
	const char *current = s;
	while (*current != '\0') {
		putchar(*current);
		current++;
		count++;
	}
	return count;
}

