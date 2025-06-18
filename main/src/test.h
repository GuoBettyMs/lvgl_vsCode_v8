#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl/lvgl.h"


/*********************
 *      DEFINES
 *********************/
#define MAX_ROWS 5

#define CLEAR lv_color_hex(0x00000000)
#define CONTENT_BG lv_color_hex(0x000000)
#define BIND_OFF lv_color_hex(0x666666)
#define BIND_ON lv_color_hex(0x55ddff)
#define BIND_REJECT lv_color_hex(0xff8080)
#define LINE_BG lv_color_hex(0x333333)
#define CHARGE_SIGN lv_color_hex(0Xb8f09a)
#define INPUT_SIGN lv_color_hex(0x00ff66)
#define ANI_ON lv_color_hex(0x999999)
#define BAR_BG lv_color_hex(0x595959)
#define BAR_GREEN lv_color_hex(0x2aff80)
#define BAR_YELLOW lv_color_hex(0xffcb2a)
#define BAR_RED lv_color_hex(0xff5555)
#define FUNCTION_OFF lv_color_hex(0x464d53)
#define LIST_BG lv_color_hex(0xcccccc)

LV_IMG_DECLARE(signnum_2);

LV_FONT_DECLARE(font_04B_20_8);
LV_FONT_DECLARE(font_bahnschrift_18);
LV_FONT_DECLARE(font_isdtyahei_18);

 /**********************
  *      TYPEDEFS
  **********************/

typedef struct {
    lv_obj_t* text;   // 文本标签
    lv_obj_t* unit;   // 单位标签
} TextPair;

//UI 对象本身不能完全静态分配,逻辑结构体使用静态内存，UI 对象由 LVGL 动态管理
typedef struct {
    lv_obj_t* container;  // 行容器
    lv_obj_t* sign_obj;   // 标志（label 或 img）
    TextPair first_text;       // 第一组文本 + 单位
    TextPair second_text;      // 第二组文本 + 单位
} InfoRow;

/**********************
 *  STATIC PROTOTYPES 函数原型声明
 **********************/
//自定义文本
TextPair custom_text_unit(lv_obj_t * parent,
                          const char * text, int32_t x, 
                          const char * unit, const lv_font_t * font);

// 创建信息行
InfoRow* add_info_row(lv_obj_t* parent, bool is_top,
                      const lv_img_dsc_t* img_src,
                      const char* fir_text, const char* fir_unit,
                      const char* sec_text, const char* sec_unit);
// 更新行内容
void update_info_row(InfoRow* row, bool is_top, 
                     const char* sign_text, const lv_img_dsc_t* img_src,
                     const char* text1, const char* unit1,
                     const char* text2, const char* unit2);


void create_info_screen(lv_obj_t *parent);
void create_page(void);
void view_zoom(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif