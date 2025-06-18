#ifndef CUSTOM_CHART_H
#define CUSTOM_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"


// 自定义颜色枚举
typedef enum{
    CHARGE_COLOR,     
    CHARGE_DONE_COLOR,   
    ANALYSE_COLOR,  
    ACTIVATE_COLOR,     
    STORAGE_COLOR,   
    CYCLE_COLOR, 
    DESTROY_COLOR,   
    DISCHARGE_ORANGE,  
} custom_chart_color_t;

// 函数原型声明
lv_color_t chart_area_color(custom_chart_color_t color);
lv_color_t chart_line_color(custom_chart_color_t color);

lv_obj_t* custom_dynamic_chart_create1(void); //compression chart
lv_obj_t* custom_dynamic_chart_create2(lv_obj_t* parent);

void add_solid_area(lv_event_t * e, custom_chart_color_t color);
void add_faded_area(lv_event_t * e, custom_chart_color_t color);
void draw_event_cb(lv_event_t * e);
void change_chart_color(lv_obj_t* chart, custom_chart_color_t new_color);
void custom_static_chart_create(void);
void custom_dynamic_chart_create(void);
void free_chart_resources(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif