#include "shared.h"

#if 0	// FIXME-ALLEGRO5: Disabled, I assume Allegro support it now

/* loadpng, Allegro wrapper routines for libpng
 * by Peter Wang (tjaden@users.sf.net).
 *
 * This file is hereby placed in the public domain.
 */


#include <png.h>
#include "system.h" // allegro.h, because of STATICLINK stuff...
#include "loadpng.h"



/* write_data:
 *  Custom write function to use Allegro packfile routines,
 *  rather than C streams.
 */
static void write_data(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
    PACKFILE *f = png_get_io_ptr(png_ptr);
    if ((png_uint_32)pack_fwrite(data, length, f) != length)
	png_error(png_ptr, "write error (loadpng calling pack_fwrite)");
}

/* Don't think Allegro has any problem with buffering
 * (rather, Allegro provides no way to flush packfiles).
 */
static void flush_data(png_structp png_ptr) { (void)png_ptr; }



/* save_hicolour:
 *  Core save routine for 15/16 bpp images, by Martijn Versteegh.
 */
static int save_hicolour(png_structp png_ptr, ALLEGRO_BITMAP *bmp, int depth)
{
    unsigned char *row, *p;
    int i, j, c;

    row = malloc(bmp->w * 3);
    if (!row)
	return 0;

    for (i=0; i<bmp->h; i++) {
	p = row;
        for (j = 0; j < bmp->w; j++) {
            c = getpixel(bmp, j, i);
            if (depth == 15) {
		*p++ = getr15(c);
		*p++ = getg15(c);
		*p++ = getb15(c);
            }
	    else {
		*p++ = getr16(c);
		*p++ = getg16(c);
		*p++ = getb16(c);
            }
        }

        png_write_row(png_ptr, row);
    }

    free(row);

    return 1;
}

/* save_truecolour:
 *  Core save routine for 32 bpp images.
 */
static int save_truecolour(png_structp png_ptr, ALLEGRO_BITMAP *bmp)
{
    unsigned char *row, *p;
    int i, j, c;

    // [OMAR]
    row = malloc(bmp->w * 3);
    //row = malloc(bmp->w * 4);
    if (!row)
	return 0;

    for (i=0; i<bmp->h; i++) {
	p = row;
        for (j = 0; j < bmp->w; j++) {
            c = getpixel(bmp, j, i);
	    *p++ = getr32(c);
	    *p++ = getg32(c);
	    *p++ = getb32(c);
        // [OMAR]
	    //*p++ = geta32(c);
        }

        png_write_row(png_ptr, row);
    }

    free(row);

    return 1;
}



/* save_png:
 *  Writes a non-interlaced, no-frills PNG, taking the usual save_xyz
 *  parameters.  Returns non-zero on error.
 */
int save_png(AL_CONST char *filename, ALLEGRO_BITMAP *bmp, AL_CONST RGB *pal)
{
    PACKFILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int depth, colour_type;

    depth = bitmap_color_depth(bmp);
    if ((!bmp) || ((depth == 8) && (!pal)))
	return -1;

    fp = pack_fopen(filename, "w");
    if (!fp)
	return -1;

    /* Create and initialize the png_struct with the
     * desired error handler functions.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				      (void *)NULL, NULL, NULL);
    if (!png_ptr) {
	pack_fclose(fp);
	return -1;
    }

    /* Allocate/initialize the image information data. */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
	pack_fclose(fp);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	return -1;
    }

    /* Set error handling. */
    if (setjmp(png_ptr->jmpbuf)) {
	/* If we get here, we had a problem reading the file. */
	pack_fclose(fp);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	return -1;
    }

    /* Use packfile routines. */
    png_set_write_fn(png_ptr, fp, (png_rw_ptr)write_data, flush_data);

    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE.
     */
    if (depth == 8)
	colour_type = PNG_COLOR_TYPE_PALETTE;
    // [OMAR] No alpha
    //else if (depth == 32)
	//colour_type = PNG_COLOR_TYPE_RGB_ALPHA;
    else
	colour_type = PNG_COLOR_TYPE_RGB;

    png_set_IHDR(png_ptr, info_ptr, bmp->w, bmp->h, 8, colour_type,
		 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
		 PNG_FILTER_TYPE_BASE);

    /* Set the palette if there is one.  Required for indexed-color images. */
    if (colour_type == PNG_COLOR_TYPE_PALETTE) {
	png_color palette[256];
	int i;

	for (i = 0; i < 256; i++) {
	    palette[i].red = _rgb_scale_6[pal[i].r];   /* 64 -> 256 */
	    palette[i].green = _rgb_scale_6[pal[i].g];
	    palette[i].blue = _rgb_scale_6[pal[i].b];
	}

	/* Set palette colors. */
	png_set_PLTE(png_ptr, info_ptr, palette, 256);
    }

    /* Optionally write comments into the image ... Nah. */

    /* Write the file header information. */
    png_write_info(png_ptr, info_ptr);

    /* Once we write out the header, the compression type on the text
     * chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
     * PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
     * at the end.
     */

    /* Flip BGR pixels to RGB. */
    /*if (depth >= 24) {
	if (makecol24(255, 0, 0) > makecol24(0, 0, 255))
	    png_set_bgr(png_ptr);
    }*/

    /* Set compression level. */
    png_set_compression_level(png_ptr, _png_compression_level);

    /* Save the data. */
    if ((depth == 15) || (depth == 16)) {
	if (!save_hicolour(png_ptr, bmp, depth)) {
	    pack_fclose(fp);
	    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	    return -1;
	}
    }
    else if (depth == 32) {
	if (!save_truecolour(png_ptr, bmp)) {
	    pack_fclose(fp);
	    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	    return -1;
	}
    }
    else
	png_write_image(png_ptr, bmp->line);   /* Dirt cheap! */

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    pack_fclose(fp);

    return 0;
}
#endif
