#if !defined(_BOOTCON_H_)
#define _BOOTCON_H_

#include <stdint.h>
#include <stddef.h>

/* font */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
    uint8_t glyphs;
} __attribute__((packed)) psf2_t;
extern volatile unsigned char _binary_font_psf_start;

class G2BootConsole {
public:
	G2BootConsole(uint8_t *fb, psf2_t *font, uint32_t width, uint32_t height, uint32_t scanline);
	bool putglyph(const char s, int kx, int ky);
	void putchar(const char s);
	int puts(const char *s);

private:
	uint8_t *fb;
	psf2_t *font;
	uint32_t width, height, scanline;
	uint32_t currentRow, currentColumn;
};	

#endif
