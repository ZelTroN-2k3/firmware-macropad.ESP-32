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

#ifndef LV_ATTRIBUTE_IMG_ICON_MENU_BLUETOOTH
#define LV_ATTRIBUTE_IMG_ICON_MENU_BLUETOOTH
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ICON_MENU_BLUETOOTH uint8_t icon_menu_bluetooth_map[] = {
  0xfe, 0xfe, 0xfe, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 0x04, 0x00, 0x06, 0x00, 0x07, 0x10, 0x25, 0x88, 0x37, 0x24, 0x1e, 0x14, 0x0c, 0x14, 
  0x1e, 0x14, 0x37, 0x24, 0x25, 0x88, 0x07, 0x10, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t icon_menu_bluetooth = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 40,
  .data = icon_menu_bluetooth_map,
};
