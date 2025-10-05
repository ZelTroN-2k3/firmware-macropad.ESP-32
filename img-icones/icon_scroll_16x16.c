#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_ICON_SCROLL_16X16
#define LV_ATTRIBUTE_IMG_ICON_SCROLL_16X16
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ICON_SCROLL_16X16 uint8_t icon_scroll_16x16_map[] = {
  0xfe, 0xfe, 0xfe, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x05, 0xa0, 0x04, 0x20, 0x05, 0x20, 0x05, 0x20, 
  0x05, 0x00, 0x01, 0xe0, 0x05, 0xe0, 0x07, 0xe0, 0x03, 0xe0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t icon_scroll_16x16 = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 40,
  .data = icon_scroll_16x16_map,
};
