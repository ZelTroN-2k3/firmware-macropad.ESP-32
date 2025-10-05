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

#ifndef LV_ATTRIBUTE_IMG_ICON_MENU_WIFI
#define LV_ATTRIBUTE_IMG_ICON_MENU_WIFI
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ICON_MENU_WIFI uint8_t icon_menu_wifi_map[] = {
  0xfe, 0xfe, 0xfe, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 0x07, 0xe0, 0x08, 0x10, 0x13, 0xc8, 0x24, 0x24, 0x09, 0x90, 0x02, 0x40, 0x00, 0x00, 
  0x01, 0x80, 0x00, 0x00, 0x3f, 0xfc, 0x2a, 0xbc, 0x3f, 0xfc, 0x3f, 0xfc, 0x18, 0x18, 0x00, 0x00, 
};

const lv_img_dsc_t icon_menu_wifi = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 40,
  .data = icon_menu_wifi_map,
};
