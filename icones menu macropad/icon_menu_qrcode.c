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

#ifndef LV_ATTRIBUTE_IMG_ICON_MENU_QRCODE
#define LV_ATTRIBUTE_IMG_ICON_MENU_QRCODE
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ICON_MENU_QRCODE uint8_t icon_menu_qrcode_map[] = {
  0xfe, 0xfe, 0xfe, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 
  0x7d, 0x3e, 
  0x45, 0xa2, 
  0x55, 0x2a, 
  0x44, 0xa2, 
  0x7d, 0x3e, 
  0x00, 0x00, 
  0x5b, 0x5a, 
  0x2c, 0x28, 
  0x01, 0x0a, 
  0x7c, 0xb6, 
  0x45, 0x60, 
  0x55, 0x4e, 
  0x45, 0xaa, 
  0x7c, 0x6e, 
  0x00, 0x00, 
};

const lv_img_dsc_t icon_menu_qrcode = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 40,
  .data = icon_menu_qrcode_map,
};
