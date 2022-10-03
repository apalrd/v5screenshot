/* Screenshot routine for PROS V5
 * Copyright (c) 2022 Andrew Palardy
 * This code is subject to the BSD 2-clause 'Simplified' license
 * See the LICENSE file for complete terms
 */
#include "pal/screenshot.h"
#include "pros/apix.h"
#include <stdint.h>
#include <stdio.h>

/* Static local data */
static bool has_init = false;
static FILE* tga_file;


/* Draw function */
static void screenshot_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* color) 
{
/* Write 8 byte targa header - no ID, no color map, uncompressed true color */
    const char tga_head[8] = {0x0,0x0,0x2,0x0,0x0,0x0,0x0};
    fwrite(tga_head,1,8,tga_file);

    /* Calculate image size */
    uint16_t width = (x2-x1+1);
    uint16_t height = (y2-y1+1);

    /* Write image header - x/y size, 32 bits per pixel, 8 bit alpha */
    uint16_t tga_img_head[5] = {0,0,width,height,0x2820};
    fwrite(tga_img_head,1,10,tga_file);

    /* Write image data - bytes are already in the right order (BGRA) */
    fwrite(color,1,4*height*width,tga_file);

    /* callback when done flushing */
	lv_flush_ready();
}

/* Screenshot function */
void screenshot(const char * fname)
{
    /* Important data to be saved */
    static lv_disp_t * disp_old;
    static lv_disp_t * disp_new;

    /* Initialization of library */
    if(!has_init)
    {
        /* Get the current lv display driver for safe keeping */
        disp_old = lv_disp_get_active();

        /* Create a new display driver with our flush function */
        lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.disp_flush = screenshot_disp_flush;
        disp_new = lv_disp_drv_register(&disp_drv);

        if(!disp_new)
        {
            printf("ERROR: Failed to register screenshot display driver\n");
        }
        else
        {
            has_init = true;
        }
    }

    /* Actual taking of the screenshot */
    if(has_init)
    {
        /* Check if usd is even inserted */
        if(!usd_is_installed())
        {
            printf("ERROR: Failed to take screenshot since uSD card is not installed!\n");
            return;
        }

        /* Add usd path to fname and file extension */
        static char scr_fname[256];
        sprintf(scr_fname,"/usd/%s.tga",fname);

        /* Open the file */
        tga_file = fopen(scr_fname,"w");

        if(!tga_file)
        {
            printf("ERROR: Failed to open file %s (got %x)\n",scr_fname,tga_file);
            return;
        }

        /* Set display driver to our modified one */
        lv_disp_set_active(disp_new);

        /* Invalidate the entire screen so it's drawn, then refresh it */
        lv_obj_invalidate(lv_scr_act());
        lv_refr_now();

        /* Close the file handle */
        fclose(tga_file);

        /* Set display driver back and mark it for redraw later */
        lv_disp_set_active(disp_old);
        lv_obj_invalidate(lv_scr_act());

        printf("SUCCESS: Screenshot %s completed\n",scr_fname);
    }
}