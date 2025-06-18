
/**
 * @file custom_chart
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h> // 添加标准IO头文件,用于调用 LV_LOG_USER、 printf
#include "custom_chart.h"

/*********************
 *      DEFINES
 *********************/

#define MY_CHART_DISPLAY_POINT  20 //The maximum number of data points displayed in the chart


/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/


/**********************
 *  STATIC VARIABLES
 **********************/

// 定义静态数据
static float data[] = {30, 70, 30, 50, 20, 60, 40, 50, 30, 40, 50, 70,
 70, 30, 50, 20, 60, 40, 50, 30, 40, 50, 70, 30, 50, 20, 60, 40, 50, 30, 40, 30};
static int data_value_count = sizeof(data)/sizeof(data[0]);
static int current_data_index = 0;  // 当前数据索引
static lv_timer_t* static_chart_timer = NULL;  // 定时器指针
static lv_chart_series_t* static_chart_ser = NULL;  // 系列指针

/**********************************/

// 定义动态数据缓冲区
static float *dynamic_data = NULL;
static int dynamic_data_capacity = 10;  // 初始容量
static int dynamic_data_count = 0;

static lv_timer_t* dynamaic_chart_timer = NULL;  // 定时器指针
static lv_chart_series_t* dynamaic_chart_ser = NULL;  // 系列指针

/**********************************/

static lv_timer_t* my_chart_timer = NULL; 
static lv_chart_series_t* my_chart_ser = NULL;  //Series pointer
static float * my_chart_dynamic_data = NULL; //Chart dynamic data (chart display can only use integers)
static int my_chart_data_total_cnt = 0; //Total amount of dynamic data

static float debug_increasing_series = 0; //Incremental series
static bool debug_is_compressed = false; //Used to print debug information
static float debug_series_max = 100;
static int my_chart_line_w = 4;

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

lv_color_t chart_area_color(custom_chart_color_t color) {
    switch (color) {
        case CHARGE_COLOR: return lv_color_hex(0xff6600);  
        case CHARGE_DONE_COLOR: return lv_color_hex(0x99ff99);  
        case ANALYSE_COLOR: return lv_color_hex(0x9955ff);  
        case ACTIVATE_COLOR: return lv_color_hex(0x37c8ab); 
        case STORAGE_COLOR: return lv_color_hex(0x5555ff);  
        case CYCLE_COLOR: return lv_color_hex(0x00b7e5);  
        case DESTROY_COLOR: return lv_color_hex(0xff5555);  
        case DISCHARGE_ORANGE: return lv_color_hex(0xff5599); 
        default: return lv_color_black();  // 默认返回黑色
    }
}

lv_color_t chart_line_color(custom_chart_color_t color) {
    switch (color) {
        case CHARGE_COLOR: return lv_color_hex(0xffc299);  
        case CHARGE_DONE_COLOR: return lv_color_hex(0x00ff00);  
        case ANALYSE_COLOR: return lv_color_hex(0xc299ff);  
        case ACTIVATE_COLOR: return lv_color_hex(0x87decc); 
        case STORAGE_COLOR: return lv_color_hex(0x9999ff);  
        case CYCLE_COLOR: return lv_color_hex(0x80e5ff);  
        case DESTROY_COLOR: return lv_color_hex(0xff9999);  
        case DISCHARGE_ORANGE: return lv_color_hex(0xff99c2); 
        default: return lv_color_black();  // 默认返回黑色
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void add_solid_area(lv_event_t * e, custom_chart_color_t color)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);

    /*Add a line mask that keeps the area below the line*/
    lv_draw_mask_line_param_t line_mask_param;
    lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

    /*Draw a rectangle that will be affected by the mask*/
    lv_draw_rect_dsc_t draw_rect_dsc;
    lv_draw_rect_dsc_init(&draw_rect_dsc);
    draw_rect_dsc.bg_opa = LV_OPA_COVER;
    draw_rect_dsc.bg_color = chart_area_color(color);//dsc->line_dsc->color;

    lv_area_t a;
    a.x1 = dsc->p1->x;
    a.x2 = dsc->p2->x;
    a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
    a.y2 = obj->coords.y2 ;//- 13; /* -13 cuts off where the rectangle draws over the chart margin. Without this an area of 0 doesn't look like 0 */
    lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

    /*Remove the mask*/
    lv_draw_mask_free_param(&line_mask_param);
    lv_draw_mask_remove_id(line_mask_id); 

     /*Recolor the chart line*/
    dsc->line_dsc->color = chart_line_color(color); 
    dsc->line_dsc->width = my_chart_line_w;
}

void add_faded_area(lv_event_t * e, custom_chart_color_t color)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);

    /*Add a line mask that keeps the area below the line*/
    lv_draw_mask_line_param_t line_mask_param;
    lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

    /*Add a fade effect: transparent bottom covering top*/
    lv_coord_t h = lv_obj_get_height(obj);
    lv_draw_mask_fade_param_t fade_mask_param;
    lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,obj->coords.y2);
    int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

    /*Draw a rectangle that will be affected by the mask*/
    lv_draw_rect_dsc_t draw_rect_dsc;
    lv_draw_rect_dsc_init(&draw_rect_dsc);
    draw_rect_dsc.bg_opa = LV_OPA_20;
    draw_rect_dsc.bg_color = chart_area_color(color);//dsc->line_dsc->color;

    lv_area_t a;
    a.x1 = dsc->p1->x;
    a.x2 = dsc->p2->x - 1;
    a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
    a.y2 = obj->coords.y2;//- 13; /* -13 cuts off where the rectangle draws over the chart margin. Without this an area of 0 doesn't look like 0 */
    lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

    /*Remove the masks*/
    lv_draw_mask_free_param(&line_mask_param);
    lv_draw_mask_free_param(&fade_mask_param);
    lv_draw_mask_remove_id(line_mask_id);
    lv_draw_mask_remove_id(fade_mask_id);

     /*Recolor the chart line*/
    dsc->line_dsc->color = chart_line_color(color); 
    dsc->line_dsc->width = my_chart_line_w;
}

// 自定义绘制回调函数
void draw_event_cb(lv_event_t* e) {

    /*Add the faded area before the lines are drawn*/
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);

    if(dsc->part == LV_PART_ITEMS) {
        
        if(!dsc->p1 || !dsc->p2) return; //防止空指针导致的崩溃
        // Get color from user data
        custom_chart_color_t* color = (custom_chart_color_t*)lv_event_get_user_data(e);
        if(color) {
            add_solid_area(e, *color);
        } else {
            add_solid_area(e, CHARGE_COLOR); 
        }
    }

    // if(dsc->part == LV_PART_ITEMS) {
    // /*
    // dsc->p1 和 dsc->p2 是线段的两端点坐标指针，当它们为 NULL 时,不加判断的直接访问会导致 EXC_BAD_ACCESS 崩溃
    // 1.当图表首次创建时，数据点可能尚未生成，此时 p1/p2 可能为 NULL
    // 2.当绘制第一个或最后一个数据点时，可能缺少相邻点构成线段
    // 3.在动态更新图表数据时，LVGL 可能在数据缓冲切换期间短暂传入无效指针
    // */
    //     if(!dsc->p1 || !dsc->p2) return; //防止空指针导致的崩溃
    //     // add_solid_area(e, CHARGE_DONE_COLOR);
    //     add_faded_area(e, CHARGE_DONE_COLOR);
    // }
}

// Dynamically modify chart colors
void change_chart_color(lv_obj_t* chart, custom_chart_color_t new_color) {
    // First remove the old event callback
    lv_obj_remove_event_cb(chart, draw_event_cb);
    
    // Create a new color variable
    static custom_chart_color_t current_color;
    current_color = new_color;
    
    // Re-add the event callback
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, &current_color);
    
    // Refresh the chart
    lv_chart_refresh(chart);
}

/****************************************************************************************/

// 静态曲线图的定时器回调函数
static void static_timer_cb(lv_timer_t* timer) {
    lv_obj_t* chart = (lv_obj_t*)timer->user_data;
    
    if(current_data_index >= data_value_count) {
        lv_timer_del(static_chart_timer);  // 数据全部添加完毕，删除定时器
        static_chart_timer = NULL;
        return;
    }

    // 添加下一个数据点
    lv_chart_set_next_value(chart, static_chart_ser, data[current_data_index]);
    current_data_index++;

    // 计算需要的缩放比例
    lv_coord_t chart_width = lv_obj_get_width(chart);
    lv_coord_t point_width = 8; // 每个点期望的像素宽度
    lv_coord_t total_width_needed = dynamic_data_count * point_width;
    
    if(total_width_needed > chart_width) {
                // 使用变换缩放X轴
        lv_coord_t zoom_x = (chart_width * 256) / total_width_needed;
        zoom_x = LV_MAX(zoom_x, 64);  // 最小25%缩放
        
        // 仅缩放X轴
        lv_obj_set_style_transform_width(chart, zoom_x, 0);
        lv_obj_set_style_transform_height(chart, 256, 0);  // Y轴不缩放
        lv_obj_refresh_ext_draw_size(chart);
        

    }
    // 滚动到最右侧
    lv_obj_scroll_to_x(chart, LV_COORD_MAX, LV_ANIM_OFF);

}

/**
 * gradient Color Area spline Chart
 */
void custom_static_chart_create(void) {
    // 创建图表对象
    lv_obj_t* chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, lv_pct(100), lv_pct(70));
    lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(chart, lv_palette_main(LV_PALETTE_BLUE), 0);


    // 强制刷新布局
    lv_refr_now(NULL);
    int screen_w = lv_disp_get_hor_res(NULL);  // 屏幕宽度（像素）
    int screen_h = lv_disp_get_ver_res(NULL);  // 屏幕高度（像素）

    int chart_w = lv_obj_get_width(chart);   // 图表实际宽度（像素）
    int chart_h = lv_obj_get_height(chart);  // 图表实际高度（像素）

    LV_LOG_USER("Screen resolution: %dx%d", screen_w, screen_h);
    LV_LOG_USER("Chart: %dx%d (%.1f%% x %.1f%%)", 
                chart_w, chart_h,
                (double)chart_w / screen_w * 100,
                (double)chart_h / screen_h * 100);


    // 设置图表类型为样条线
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    
    // 设置范围
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    
    // 设置足够多的点以容纳所有数据
    lv_chart_set_point_count(chart, data_value_count);
    
    // 彻底清除所有网格线和背景
    // lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(chart, lv_color_white(), 0);
    lv_obj_set_style_line_width(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);

    //remove padding 
    lv_obj_set_style_pad_all(chart, 0, 0);

    // 禁用数据点标记
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    //使线条更平滑
    lv_obj_set_style_line_rounded(chart, true, LV_PART_ITEMS);

    // 添加数据系列
    static_chart_ser = lv_chart_add_series(chart, lv_color_hex(0x3498db), LV_CHART_AXIS_PRIMARY_Y);
    
    // 初始化数据为0
    for(int i = 0; i < data_value_count; i++) {
        lv_chart_set_next_value(chart, static_chart_ser, 0);
    }
    
    // 自定义绘制回调
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    
    // 创建定时器，每秒添加一个数据点
    current_data_index = 0;  // 重置索引
    static_chart_timer = lv_timer_create(static_timer_cb, 1000, chart);  // 1000ms = 1秒
}


/****************************************************************************************/

// 动态曲线图的定时器释放
static void free_dynamic_chart_resources() {
    if(dynamaic_chart_timer) {
        lv_timer_del(dynamaic_chart_timer);// 数据全部添加完毕，删除定时器
        dynamaic_chart_timer = NULL;
    }
    if(dynamic_data) {
        lv_mem_free(dynamic_data);
        dynamic_data = NULL;
    }
    dynamic_data_capacity = 0;
    dynamic_data_count = 0;

}
static void free_chart_resources_cb(lv_event_t * e) {
    (void)e;  // 忽略事件参数
    free_dynamic_chart_resources();  // 调用原函数
}

// 动态曲线图的定时器回调函数
static void dynamic_timer_cb(lv_timer_t* timer) {
    lv_obj_t* chart = (lv_obj_t*)timer->user_data;

    // 检查并扩展缓冲区
    if(dynamic_data_count >= dynamic_data_capacity) {
        dynamic_data_capacity *= 2;
        float* new_data = lv_mem_realloc(dynamic_data, dynamic_data_capacity * sizeof(float));
        if(!new_data) {
            LV_LOG_ERROR("Failed to realloc memory");
            return;
        }
        dynamic_data = new_data;
    }
    
    // 添加新的随机数据点 (范围0-100)
    dynamic_data[dynamic_data_count] = (float)(lv_rand(47, 53));
    LV_LOG_USER("count: %d, %.1f", dynamic_data_count, (double)dynamic_data[dynamic_data_count]);
    dynamic_data_count++;
    if (dynamic_data_count >= 30) {
        free_dynamic_chart_resources();
        return;
    }

    // 更新图表点数
    lv_chart_set_point_count(chart, dynamic_data_count);

    // // 使用扩展数组方式更新数据
    for(int i = 0; i < dynamic_data_count; i++) {
        lv_chart_set_next_value(chart, dynamaic_chart_ser, dynamic_data[i]);
    }
    
    // 计算需要的缩放比例
    lv_coord_t chart_width = lv_obj_get_width(chart);
    lv_coord_t point_width = 8; // 每个点期望的像素宽度
    lv_coord_t total_width_needed = dynamic_data_count * point_width;
    
    if(total_width_needed > chart_width) {
                // 使用变换缩放X轴
        lv_coord_t zoom_x = (chart_width * 256) / total_width_needed;
        zoom_x = LV_MAX(zoom_x, 64);  // 最小25%缩放
        
        // 仅缩放X轴
        lv_obj_set_style_transform_width(chart, zoom_x, 0);
        lv_obj_set_style_transform_height(chart, 256, 0);  // Y轴不缩放
        lv_obj_refresh_ext_draw_size(chart);

    }
    // 滚动到最右侧
    lv_obj_scroll_to_x(chart, LV_COORD_MAX, LV_ANIM_OFF);

}

// //动态曲线图绘制
void custom_dynamic_chart_create(void) {

    int v_max = 100;
    
    // 初始化动态数据
    dynamic_data = lv_mem_alloc(dynamic_data_capacity * sizeof(float));
    if(!dynamic_data) {
        LV_LOG_ERROR("Failed to allocate memory");
        return;
    }

    // 创建图表对象
    lv_obj_t* chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, lv_pct(100), lv_pct(70));
    lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(chart, lv_palette_main(LV_PALETTE_BLUE), 0);


    // 强制刷新布局
    lv_refr_now(NULL);
    int screen_w = lv_disp_get_hor_res(NULL);  // 屏幕宽度（像素）
    int screen_h = lv_disp_get_ver_res(NULL);  // 屏幕高度（像素）

    int chart_w = lv_obj_get_width(chart);   // 图表实际宽度（像素）
    int chart_h = lv_obj_get_height(chart);  // 图表实际高度（像素）

    LV_LOG_USER("Screen resolution: %dx%d", screen_w, screen_h);
    LV_LOG_USER("Chart: %dx%d (%.1f%% x %.1f%%)", 
                chart_w, chart_h,
                (double)chart_w / screen_w * 100,
                (double)chart_h / screen_h * 100);
    
    // 图表设置
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, v_max);
    lv_chart_set_point_count(chart, dynamic_data_count);
    
    // 样式设置
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_line_width(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(chart, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_rounded(chart, true, LV_PART_ITEMS);
    
    // 添加系列
    dynamaic_chart_ser = lv_chart_add_series(chart, lv_color_hex(0x3498db), LV_CHART_AXIS_PRIMARY_Y);

    // 自定义绘制曲线区域回调
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    
    // 创建定时器
    dynamaic_chart_timer = lv_timer_create(dynamic_timer_cb, 500, chart);

    lv_obj_t *stop_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(stop_btn, lv_pct(50), 50);
    lv_obj_set_style_bg_color(stop_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_align(stop_btn, LV_ALIGN_BOTTOM_MID);
    lv_obj_align_to(stop_btn, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_add_event_cb(stop_btn, free_chart_resources_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * label = lv_label_create(stop_btn);
    lv_label_set_text(label, "stop");
    lv_obj_center(label);

}

/****************************************************************************************/


void free_chart_resources(void) {
    if(my_chart_timer) {
        lv_timer_del(my_chart_timer);// All data has been added, delete the timer
        my_chart_timer = NULL;
    }
    if(my_chart_dynamic_data) {
        lv_mem_free(my_chart_dynamic_data);
        my_chart_dynamic_data = NULL;
    }
    my_chart_data_total_cnt = 0;
}

static void free_chart_resources_wrapper1(lv_event_t * e) {
    (void)e; // Ignore event parameters
    free_chart_resources();
}

static void compress_data(void) 
{
    size_t new_count = my_chart_data_total_cnt / 2;
    for (size_t i = 0; i < new_count; i++) {
        my_chart_dynamic_data[i] = (my_chart_dynamic_data[2*i] + my_chart_dynamic_data[2*i + 1]) / 2.0f;
    }
    my_chart_data_total_cnt = new_count;
    
    debug_is_compressed = true;
    LV_LOG_USER("Compressed data: [9]=%.2f [0]=%.2f", 
           (double)my_chart_dynamic_data[9], 
           (double)my_chart_dynamic_data[0]);
}

// my_chart_data_total_cnt % MY_CHART_DISPLAY_POINT == 0 ,ui_display[20] = compressed_data[0...9] + new_data[10...19]
//After the first compression, compressed_data[0] is equal to (the average value of old_data in the first 2 seconds);
//After the second compression, compressed_data[0] is equal to (the average value of old_data in the first 4 seconds);
//After the third compression, compressed_data[0] is equal to (the average value of old_data in the first 8 seconds);

static void update_chart_display(lv_obj_t* chart) 
{
    // Update chart points
    lv_chart_set_point_count(chart, my_chart_data_total_cnt);

    // Update chart data in batches
    for(int i = 0; i < my_chart_data_total_cnt; i++) {
        lv_chart_set_next_value(chart, my_chart_ser, my_chart_dynamic_data[i]);
    }

    // Calculate required scale ratio
    lv_coord_t chart_width = lv_obj_get_width(chart);
    lv_coord_t point_width = 2; // Expected pixel width per point
    lv_coord_t total_width_needed = my_chart_data_total_cnt * point_width;
    
    if(total_width_needed > chart_width) {
        // Scale X axis using transform
        lv_coord_t zoom_x = (chart_width * 256) / total_width_needed;
        zoom_x = LV_MAX(zoom_x, 64);  // Minimum 25% scale
        
        lv_obj_set_style_transform_width(chart, zoom_x, 0); // Scale only the X axis
        lv_obj_set_style_transform_height(chart, 256, 0);  // Do not scale the Y axis
        lv_obj_refresh_ext_draw_size(chart);

    }
    // Scroll to the far right
    lv_obj_scroll_to_x(chart, LV_COORD_MAX, LV_ANIM_OFF);
}

static void dynamic_timer_cb1(lv_timer_t* timer) {
    
    lv_obj_t* chart = (lv_obj_t*)timer->user_data;
    if(my_chart_data_total_cnt % MY_CHART_DISPLAY_POINT == 0) {
        compress_data();
    }
    // Set the chart's y values ​​to an increasing series (optional)
    my_chart_dynamic_data[my_chart_data_total_cnt] = debug_increasing_series++;
    
    // //debug info
    // if (debug_is_compressed)
    // {
    //     LV_LOG_USER("new_data init index: %d, new_data[10]: %.2f", 
    //        (double)my_chart_dynamic_data[my_chart_data_total_cnt]);
    //     debug_is_compressed = false;
    // }

    // LV_LOG_USER("count: %d, %.1f", my_chart_data_total_cnt, (double)my_chart_dynamic_data[my_chart_data_total_cnt]);
    my_chart_data_total_cnt++;

    //Pause adding data when the y value reaches a maximum value (optional)
    if(debug_increasing_series >= debug_series_max/4){
        change_chart_color(chart, CYCLE_COLOR);
    }
    if (debug_increasing_series >= debug_series_max){
        free_chart_resources();
        return;
    }

    update_chart_display(chart);

}

lv_obj_t* custom_dynamic_chart_create1(void) {

    // Initialize dynamic data
    my_chart_dynamic_data = lv_mem_alloc(MY_CHART_DISPLAY_POINT * sizeof(float));
    if(!my_chart_dynamic_data) {
        LV_LOG_ERROR("Memory allocation failed");
        return NULL;
    }
    
    // Create a chart
    lv_obj_t* chart = lv_chart_create(lv_scr_act());
    if(!chart) {
        lv_mem_free(my_chart_dynamic_data);
        my_chart_dynamic_data = NULL;
        return NULL;
    }
    
    lv_coord_t chart_max_h = 70 * lv_disp_get_ver_res(NULL) / 100;
    lv_coord_t chart_max_w = 100 * lv_disp_get_hor_res(NULL) / 100;
    lv_obj_set_size(chart, chart_max_w, chart_max_h);
    lv_obj_center(chart);
    
    // Style configuration
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, debug_series_max); 
    lv_chart_set_point_count(chart, 0); 
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_rounded(chart, true, LV_PART_ITEMS);
    
    // Add series
    my_chart_ser = lv_chart_add_series(chart, lv_color_hex(0x3498db), LV_CHART_AXIS_PRIMARY_Y);
    
    // Add event callback
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    
    // Create a timer
    my_chart_timer = lv_timer_create(dynamic_timer_cb1, 500, chart);
    
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, lv_pct(50), 50);
    lv_obj_set_style_bg_color(btn, lv_palette_darken(LV_PALETTE_DEEP_ORANGE, 1), 0);
    lv_obj_set_align(btn, LV_ALIGN_TOP_MID);
    lv_obj_add_event_cb(btn, free_chart_resources_wrapper1, LV_EVENT_CLICKED, NULL);
    lv_obj_move_foreground(btn);  // Force sticky
	
	lv_obj_t * btn_title = lv_label_create(btn);
    lv_label_set_text(btn_title, "Stop");
    lv_obj_center(btn_title);

    return chart;
}



/****************************************************************************************/
//将 my_chart 和相关资源设为全局或静态变量，初始化一次后重复使用,实现对象复用而非反复创建/删除
static lv_obj_t* my_chart;

lv_obj_t* custom_dynamic_chart_create2(lv_obj_t* parent) {

    lv_obj_update_layout(parent); // 强制更新布局,从而可以获取父对象尺寸
    int chart_max_h = 80 * lv_obj_get_height(parent) / 100;
    int chart_max_w = 88 * lv_obj_get_width(parent) / 100;

    // Initialize dynamic data
    my_chart_dynamic_data = lv_mem_alloc(MY_CHART_DISPLAY_POINT * sizeof(float));
    if(!my_chart_dynamic_data) {
        LV_LOG_ERROR("Memory allocation failed");
        return NULL;
    }
    
    // Create a chart
    if (my_chart == NULL) {
        my_chart = lv_chart_create(parent);
        if(!my_chart) {
            lv_mem_free(my_chart_dynamic_data);
            my_chart_dynamic_data = NULL;
            return NULL;
        }
        lv_obj_set_size(my_chart, chart_max_w, chart_max_h);
        lv_obj_align(my_chart, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

        lv_obj_set_style_bg_opa(my_chart, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_line_width(my_chart, 0, LV_PART_MAIN);
        lv_obj_set_style_line_width(my_chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_border_width(my_chart, 0, LV_PART_MAIN);
        lv_obj_set_style_line_width(my_chart, 0, LV_PART_SCROLLBAR);
        lv_obj_set_style_pad_all(my_chart, 0, 0);
        lv_obj_set_style_size(my_chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_line_rounded(my_chart, true, LV_PART_ITEMS);
        
        // Style configuration
        lv_chart_set_type(my_chart, LV_CHART_TYPE_LINE);
        lv_chart_set_range(my_chart, LV_CHART_AXIS_PRIMARY_Y, 0, debug_series_max); 
        lv_chart_set_point_count(my_chart, 0); 

        // Add series
        my_chart_ser = lv_chart_add_series(my_chart, lv_color_hex(0x3498db), LV_CHART_AXIS_PRIMARY_Y);

        // Add event callback and Passing color parameters
        static custom_chart_color_t chart_color = CHARGE_DONE_COLOR;  
        lv_obj_add_event_cb(my_chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, &chart_color);

        // Create a timer
        my_chart_timer = lv_timer_create(dynamic_timer_cb1, 500, my_chart);

    }

    return my_chart;
}