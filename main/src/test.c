/**
 * @file test
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h> // 添加标准IO头文件,用于调用 LV_LOG_USER、 printf

#include "test.h"
#include "custom_chart.h"

/*********************
 *      DEFINES
 *********************/

// #define CLEAR lv_color_hex(0x00000000)
// #define CONTENT_BG lv_color_hex(0x000000)
// #define BIND_OFF lv_color_hex(0x666666)
// #define BIND_ON lv_color_hex(0x55ddff)
// #define BIND_REJECT lv_color_hex(0xff8080)
// #define LINE_BG lv_color_hex(0x333333)
// #define CHARGE_SIGN lv_color_hex(0Xb8f09a)
// #define INPUT_SIGN lv_color_hex(0x00ff66)
// #define ANI_ON lv_color_hex(0x999999)
// #define BAR_BG lv_color_hex(0x595959)
// #define BAR_GREEN lv_color_hex(0x2aff80)
// #define BAR_YELLOW lv_color_hex(0xffcb2a)
// #define BAR_RED lv_color_hex(0xff5555)
// #define FUNCTION_OFF lv_color_hex(0x464d53)
// #define LIST_BG lv_color_hex(0xcccccc)

// LV_IMG_DECLARE(signnum_2);

// LV_FONT_DECLARE(font_04B_20_8);
// LV_FONT_DECLARE(font_bahnschrift_18);
// LV_FONT_DECLARE(font_isdtyahei_18);

/**********************
 *  STATIC VARIABLES
 **********************/

static InfoRow rows[MAX_ROWS];  // 静态存储区（程序整个生命周期有效）
static const lv_img_dsc_t * info_title_images[] = { 
    NULL, &signnum_2 , &signnum_2, &signnum_2, &signnum_2,
		&signnum_2, &signnum_2,
};
static char * labs[10] = {"镍氢", "00:00:00", "100", "9999", "4.20", "1.01", "56", "45", "100", "9999"};
static char * lab_units[10] = {"", "", "%", "mAh", "V", "A", "mΩ", "°C,", "°F", "Wh"};
	

static lv_style_t common_screen_style;

static const lv_img_dsc_t * get_info_title_image(int index);


static void init_style1(){
    lv_style_init(&common_screen_style);
    lv_style_set_bg_opa(&common_screen_style, LV_OPA_TRANSP); 
    lv_style_set_border_opa(&common_screen_style, LV_OPA_COVER); //边框透明
    lv_style_set_border_width(&common_screen_style,0);
    lv_style_set_radius(&common_screen_style, 0);//圆角为0
    lv_style_set_pad_all(&common_screen_style, 0);//内边距为0

}

const lv_img_dsc_t * get_info_title_image(int index) {
    if (index >= 0 && index < sizeof(info_title_images) / sizeof(info_title_images[0])) { //不超出数组索引
        return info_title_images[index];
    }
    return NULL; // 索引无效时返回 NULL
}

//自定义文本
TextPair custom_text_unit(lv_obj_t * parent,
                          const char * text, int32_t x, 
                          const char * unit, const lv_font_t * font)
{
    TextPair pair = {0};

    // 创建文本标签
    pair.text = lv_label_create(parent);
    lv_obj_set_style_text_color(pair.text, lv_color_white(), 0);
    lv_label_set_text(pair.text, text);
    lv_obj_set_style_text_font(pair.text, font ? font : &lv_font_montserrat_18, 0);
    lv_obj_align(pair.text, LV_ALIGN_LEFT_MID, x, 0);

    // 创建单位标签
    pair.unit = lv_label_create(parent);
    lv_obj_set_style_text_color(pair.unit, lv_color_white(), 0);
    lv_label_set_text(pair.unit, unit);
    lv_obj_set_style_text_font(pair.unit, font ? font : &lv_font_montserrat_18, 0);
    lv_obj_align_to(pair.unit, pair.text, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    return pair;//返回结构体实例（值类型）
}


// 创建信息行
InfoRow* add_info_row(lv_obj_t* parent, bool is_top, 
                     const lv_img_dsc_t* img_src, 
                     const char* fir_text, const char* fir_unit, 
                     const char* sec_text, const char* sec_unit) {

    //轮询分配行对象 ,避免动态内存分配                   
    static int index = 0;
    InfoRow* row = &rows[index]; // 获取结构体实例地址
    index = (index + 1) % MAX_ROWS;

    // 清除旧内容或创建新对象
    if (lv_obj_is_valid(row->container)) {
        lv_obj_clean(row->container);  // 清空已有子对象
    } else {
        row->container = lv_obj_create(parent);
        lv_obj_set_size(row->container, lv_pct(100), is_top ? 28 : 31);
        lv_obj_add_style(row->container, &common_screen_style, 0);
        lv_obj_set_style_bg_color(row->container, CLEAR, 0);
    }

    // 2. 创建分割线  
    lv_obj_t * title_line = lv_obj_create(row->container);
    lv_obj_add_style(title_line, &common_screen_style, 0);
    lv_obj_set_size(title_line, lv_pct(87), is_top ? 2 : 1);
    lv_obj_set_style_bg_opa(title_line, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(title_line, is_top ? lv_color_white() : LINE_BG, 0);
    lv_obj_set_align(title_line, LV_ALIGN_BOTTOM_RIGHT);

    // 3. 创建标志容器
    lv_obj_t* sign_bg = lv_obj_create(row->container);
    lv_obj_set_size(sign_bg, lv_pct(12), lv_pct(100));
    lv_obj_add_style(sign_bg, &common_screen_style, 0);
	lv_obj_set_style_bg_opa(sign_bg, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(sign_bg, is_top ? lv_color_white() : CLEAR, 0);
    lv_obj_set_align(sign_bg, LV_ALIGN_LEFT_MID);

    if (is_top) {
        // 顶部模式：标签
        row->sign_obj = lv_label_create(sign_bg);
        lv_label_set_text(row->sign_obj, "--"); // 默认值
        lv_obj_set_style_text_font(row->sign_obj, (lv_font_t *)&font_04B_20_8, 0);
        lv_obj_center(row->sign_obj);
    } else {
        // 非顶部模式：图片
        row->sign_obj = lv_img_create(sign_bg);
        lv_img_set_src(row->sign_obj, img_src);
        lv_obj_center(row->sign_obj);	
    }

    // 4. 创建文本标签
    // 创建第一组文本+单位
    if (fir_text && fir_unit) {
        row->first_text = custom_text_unit(row->container, fir_text, 
        lv_pct(16), fir_unit, is_top ? &font_isdtyahei_18 : &font_bahnschrift_18);
    } else {
        // 可以清空或设为 NULL
        row->first_text.text = NULL;
        row->first_text.unit = NULL;
    }

    // 创建第二组文本+单位
    if (sec_text && sec_unit) {
        row->second_text = custom_text_unit(row->container, sec_text,
         lv_pct(50), sec_unit, NULL);
        if(is_top){//重新设置对齐
            lv_obj_align(row->second_text.text, LV_ALIGN_RIGHT_MID, -5, 0); 
        }
    } else {
        // 可以清空或设为 NULL
        row->second_text.text = NULL;
        row->second_text.unit = NULL;
    }

    return row; //返回指向实例的指针
}

// 更新行内容
void update_info_row(InfoRow* row, bool is_top, 
                     const char* sign_text, const lv_img_dsc_t* img_src,
                     const char* text1, const char* unit1,
                     const char* text2, const char* unit2) {

    if (!row || !lv_obj_is_valid(row->container)) return;

    // 更新 sign_obj
    if (is_top && sign_text && row->sign_obj && lv_obj_check_type(row->sign_obj, &lv_label_class)) {
        lv_label_set_text(row->sign_obj, sign_text);
    } else if (!is_top && img_src && row->sign_obj && lv_obj_check_type(row->sign_obj, &lv_img_class)) {
        lv_img_set_src(row->sign_obj, img_src);
    }

    // 更新第一个文本+单位
    if (text1 && row->first_text.text && lv_obj_is_valid(row->first_text.text)) {
        lv_label_set_text(row->first_text.text, text1);
        lv_obj_align_to(row->first_text.unit, row->first_text.text, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }
    if (unit1 && row->first_text.unit && lv_obj_is_valid(row->first_text.unit)) {
        lv_label_set_text(row->first_text.unit, unit1);
    }

    // 更新第二个文本+单位
    if (text2 && row->second_text.text && lv_obj_is_valid(row->second_text.text)) {
        lv_label_set_text(row->second_text.text, text2);
        lv_obj_align_to(row->second_text.unit, row->second_text.text, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }
    if (unit2 && row->second_text.unit && lv_obj_is_valid(row->second_text.unit)) {
        lv_label_set_text(row->second_text.unit, unit2);
    }
}


static void click_event(lv_event_t * e)
{   //NULL 表示不修改
	update_info_row(&rows[0], true, "2", NULL, NULL, NULL, NULL, NULL);
    update_info_row(&rows[1], false, NULL, NULL, "4.25, 100", NULL,"1.01", NULL);
    free_chart_resources();

}

void create_info_screen(lv_obj_t *parent)
{

init_style1();
lv_obj_set_style_bg_color(lv_scr_act(), CONTENT_BG, LV_PART_MAIN);

    if (parent == NULL || !lv_obj_is_valid(parent)) return;

    rows[0] = *add_info_row(parent, true, NULL, labs[0], "", labs[1], ""); //需要解引用赋值给结构体
    lv_obj_align(rows[0].container, LV_ALIGN_TOP_MID, 0, 0);

    for (int i = 1; i < 5; i++) {
        if(i == 3){
            char temp[32];
            snprintf(temp, sizeof(temp), "%s%s%s", labs[7], lab_units[7], labs[8]);
            rows[i] = *add_info_row(parent, false, get_info_title_image(i), 
            labs[i*2], lab_units[i*2], temp, lab_units[8]);

        }else if (i == 4){
            rows[i] = *add_info_row(parent, false, get_info_title_image(i), 
            labs[9], lab_units[9], "", "");
        }else{
            rows[i] = *add_info_row(parent, false, get_info_title_image(i), 
            labs[i*2], lab_units[i*2], labs[i*2+1], lab_units[i*2+1]);
        }
        lv_obj_align(rows[i].container, LV_ALIGN_TOP_MID, 0, i == 1 ? 28 : 28+(i-1)*31);       
    }

    lv_obj_t * tage = lv_obj_create(parent);
    lv_obj_set_size(tage, lv_pct(100), lv_pct(35));
    lv_obj_add_style(tage, &common_screen_style, 0);
    lv_obj_set_style_bg_opa(tage, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage, LINE_BG, 0);
    lv_obj_align(tage, LV_ALIGN_TOP_MID, 0, lv_pct(48));

    lv_obj_t * tage_title = lv_obj_create(tage);
    lv_obj_set_size(tage_title, lv_pct(88), lv_pct(18));
    lv_obj_add_style(tage_title, &common_screen_style, 0);
    lv_obj_set_style_bg_opa(tage_title, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage_title, ANI_ON, 0);
    lv_obj_align(tage_title, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t * title_label = lv_label_create(tage_title);
    lv_label_set_text(title_label, "UOLTAGE" );
	lv_obj_set_style_text_font(title_label, (lv_font_t *)&font_04B_20_8, 0);
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t * tage_left = lv_obj_create(tage);
    lv_obj_set_size(tage_left, lv_pct(12), lv_pct(50));
    lv_obj_add_style(tage_left, &common_screen_style, 0);
    lv_obj_set_style_bg_opa(tage_left, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage_left, CLEAR, 0);
    lv_obj_align(tage_left, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * tage_left_bottom = lv_obj_create(tage);
    lv_obj_set_size(tage_left_bottom, lv_pct(12), lv_pct(50));
    lv_obj_add_style(tage_left_bottom, &common_screen_style, 0);
    lv_obj_set_style_bg_opa(tage_left_bottom, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage_left_bottom, CLEAR, 0);
    lv_obj_align(tage_left_bottom, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    for(int i=0; i<2;i++){
        lv_obj_t * tage_img_1 = lv_img_create(i==0 ? tage_left : tage_left_bottom);
        lv_obj_set_style_img_recolor_opa(tage_img_1, LV_OPA_COVER, 0);
        lv_obj_set_style_img_recolor(tage_img_1, ANI_ON, 0);
        lv_img_set_src(tage_img_1, get_info_title_image(i+5));
        // lv_obj_set_style_image_recolor_opa(tage_img_1, LV_OPA_COVER, 0);
        // lv_obj_set_style_image_recolor(tage_img_1, ANI_ON, 0);
        // lv_image_set_src(tage_img_1, get_info_title_image(i+5));
        lv_obj_align(tage_img_1, LV_ALIGN_CENTER, 0, 0);
    }

    custom_dynamic_chart_create2(tage);

    lv_obj_t* play_btn = lv_btn_create(parent);
    lv_obj_set_size(play_btn, lv_pct(100), lv_pct(13));
    lv_obj_add_style(play_btn, &common_screen_style, 0);
    lv_obj_set_style_bg_opa(play_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(play_btn, BAR_GREEN, 0);
    lv_obj_align(play_btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(play_btn, click_event, LV_EVENT_CLICKED, NULL);

	lv_obj_t * left_img = lv_img_create(play_btn);
//    lv_obj_set_style_image_recolor_opa(left_img, LV_OPA_COVER, 0);
//    lv_obj_set_style_image_recolor(right_img, lv_color_black(), 0);
    lv_img_set_src(left_img, &signnum_2);
    lv_obj_align(left_img, LV_ALIGN_LEFT_MID, 4, 0);

    lv_obj_t * center_label = lv_label_create(play_btn);
    lv_label_set_text(center_label, "完成");
    lv_obj_set_style_text_color(center_label, lv_color_black(), 0);
    lv_obj_set_style_text_font(center_label, (lv_font_t *)&font_isdtyahei_18, 0); //
    lv_obj_center(center_label);
    
    lv_obj_t * right_img = lv_img_create(play_btn);
        lv_obj_set_style_img_recolor_opa(right_img, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(right_img, lv_color_black(), 0);
    // lv_obj_set_style_image_recolor_opa(right_img, LV_OPA_COVER, 0);
    // lv_obj_set_style_image_recolor(right_img, lv_color_black(), 0);
    lv_img_set_src(right_img, &signnum_2);
    lv_obj_align(right_img, LV_ALIGN_RIGHT_MID, -4, 0);
	
}


/**********************************//**********************************//**********************************/

static lv_obj_t * parent_v;
static lv_obj_t * v_btn;
static lv_obj_t * label;
static bool is_small = false;


/**********************************/
static lv_style_t batt_style;
static lv_style_t list_item_style;

//系统设置
char * setting_title[9] = {"音量", "完成声音", "屏幕亮度", "容量限制", "扩容充电", "自动熄屏", "语言", "系统信息", "恢复出厂设定"};
char * setting_btn_value_init[7] = {"低", "单次", "高", "关闭", "关闭", "关闭", "简体中文"};
char * setting_value_lists_lab[4] = {"关", "低", "中", "高"};   
int setting_btn_i = -1 ;
bool is_task_on = true;
static lv_obj_t * setting_btns[9];
static lv_obj_t * setting_value_btns[4];
static lv_obj_t * setting_btn_value[7];

lv_obj_t * tile1;
lv_obj_t * tile2;

/**********************
 *   FUNCTIONS
 **********************/

static void init_style(){
    // lv_style_init(&bg_style);
    // lv_style_set_bg_opa(&bg_style, LV_OPA_TRANSP);
    // lv_style_set_border_opa(&bg_style, LV_OPA_TRANSP); //边框透明
    // lv_style_set_border_width(&bg_style,0);
    // lv_style_set_radius(&bg_style, 0);//圆角为0
    // lv_style_set_pad_all(&bg_style, 0);//内边距为0
    // lv_style_set_margin_all(&bg_style, 0);
    // lv_style_set_flex_flow(&bg_style, LV_FLEX_FLOW_ROW);
    // lv_style_set_flex_track_place(&bg_style, LV_FLEX_ALIGN_CENTER);
    // lv_style_set_layout(&bg_style, LV_LAYOUT_FLEX); //设置灵活布局
    
    lv_style_init(&batt_style);
    lv_style_set_bg_opa(&batt_style, LV_OPA_COVER); // 确保背景完全不透明
    lv_style_set_bg_color(&batt_style,BAR_BG); 
    lv_style_set_border_opa(&batt_style, LV_OPA_TRANSP); //边框透明
    lv_style_set_border_width(&batt_style,0);
    lv_style_set_radius(&batt_style, 0);//圆角为0
    lv_style_set_pad_all(&batt_style, 0);//内边距为0
    // lv_style_set_margin_all(&batt_style, 0);

    // lv_style_init(&bar_indic);
    // lv_style_set_bg_opa(&bar_indic, LV_OPA_COVER);
    // lv_style_set_bg_color(&bar_indic, BAR_RED);
    // lv_style_set_radius(&bar_indic, 0);

    lv_style_init(&list_item_style);
    lv_style_set_bg_opa(&list_item_style, LV_OPA_TRANSP); 
    lv_style_set_border_opa(&list_item_style, LV_OPA_COVER); //边框透明
    lv_style_set_border_width(&list_item_style,0);
    lv_style_set_radius(&list_item_style, 0);//圆角为0
    lv_style_set_pad_all(&list_item_style, 0);//内边距为0
    // lv_style_set_margin_all(&list_item_style, 0);  

}

/**********************************//**********************************//**********************************//**********************************/
lv_obj_t * add_img_title(lv_obj_t * parent, bool is_top, const lv_img_dsc_t * img_src,  char * fir_text,  char * sec_text){

    int w = lv_obj_get_width(parent);
    int h = 9 * lv_obj_get_height(parent) / 100;
    lv_obj_t * title_bg = lv_obj_create(parent);
    lv_obj_set_size(title_bg, lv_pct(100), h);
    lv_obj_add_style(title_bg, &batt_style, 0);
    lv_obj_set_style_bg_color(title_bg, CLEAR, 0);

    lv_obj_t * title_sign_bg = lv_obj_create(title_bg);
    lv_obj_set_size(title_sign_bg, lv_pct(12), lv_pct(100));
    lv_obj_add_style(title_sign_bg, &batt_style, 0);
    lv_obj_set_style_bg_color(title_sign_bg, is_top ? lv_color_white() : CLEAR, 0);
    lv_obj_set_align(title_sign_bg, LV_ALIGN_LEFT_MID);

    lv_obj_t * img = lv_img_create(title_sign_bg);
    lv_img_set_src(img, img_src);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(img, ANI_ON, 0);
    lv_obj_center(img);

    lv_obj_t * title = lv_label_create(title_bg);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, fir_text);
    // lv_obj_set_style_text_font(title, is_top ? &lv_font_sourcehan_sans_rhw_18 : &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, title_sign_bg, LV_ALIGN_OUT_RIGHT_MID, 10, 0);


    lv_obj_t * title_time = lv_label_create(title_bg);
    lv_obj_set_style_text_color(title_time, lv_color_white(), 0);
    lv_label_set_text(title_time, sec_text);
    // lv_obj_set_style_text_font(title_time, is_top ? &lv_font_sourcehan_sans_rhw_18 : &lv_font_montserrat_16, 0);
    if(is_top){
        lv_obj_align(title_time, LV_ALIGN_RIGHT_MID, -5, 0);
    }else{
        lv_obj_align(title_time, LV_ALIGN_LEFT_MID, w/2, 0);
    }

   return  title_bg;
}

static void add_tile3(lv_obj_t * parent){
    char * labs[9] = {"镍氢", "00:00:00", "100%", "9999mAh", "4.20V", "1.01A", "56mΩ", "45°C, 100°F", "9999Wh"};
    int h = 9 * lv_obj_get_height(parent) / 100;

    for(int i=0; i<5; i++){
        lv_obj_t * item;
        item = add_img_title(parent, i == 0 , &signnum_2, labs[i*2], i == 4 ? "" : labs[i*2+1]);
        lv_obj_align(item, LV_ALIGN_TOP_MID, 0, i*h);

        if(i!=4){
            lv_obj_t * title_line = lv_obj_create(item);
            lv_obj_add_style(title_line, &list_item_style, 0);
            lv_obj_set_size(title_line, lv_pct(87), i == 0 ? 3 : 1);
            lv_obj_set_style_bg_opa(title_line, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(title_line, i == 0 ? lv_color_white() : LINE_BG, 0);
            lv_obj_set_align(title_line, LV_ALIGN_BOTTOM_RIGHT);
        }

    }

    lv_obj_t * tage = lv_obj_create(parent);
    lv_obj_set_size(tage, lv_pct(100), lv_pct(35));
    lv_obj_add_style(tage, &list_item_style, 0);
    lv_obj_set_style_bg_opa(tage, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage, LINE_BG, 0);
    lv_obj_align(tage, LV_ALIGN_TOP_MID, 0, 5*h+5);

    lv_obj_t * tage_title = lv_obj_create(tage);
    lv_obj_set_size(tage_title, lv_pct(88), lv_pct(18));
    lv_obj_add_style(tage_title, &list_item_style, 0);
    lv_obj_set_style_bg_opa(tage_title, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage_title, ANI_ON, 0);
    lv_obj_align(tage_title, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t * title_label = lv_label_create(tage_title);
    lv_label_set_text(title_label, "UOLTAGE" );
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t * tage_left = lv_obj_create(tage);
    lv_obj_set_size(tage_left, lv_pct(12), lv_pct(50));
    lv_obj_add_style(tage_left, &list_item_style, 0);
    lv_obj_set_style_bg_opa(tage_left, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage_left, CLEAR, 0);
    lv_obj_align(tage_left, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * tage_left_bottom = lv_obj_create(tage);
    lv_obj_set_size(tage_left_bottom, lv_pct(12), lv_pct(50));
    lv_obj_add_style(tage_left_bottom, &list_item_style, 0);
    lv_obj_set_style_bg_opa(tage_left_bottom, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tage_left_bottom, CLEAR, 0);
    lv_obj_align(tage_left_bottom, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    for(int i=0; i<2;i++){
        lv_obj_t * tage_img_1 = lv_img_create(i==0 ? tage_left : tage_left_bottom);
        lv_obj_set_style_img_recolor_opa(tage_img_1, LV_OPA_COVER, 0);
        lv_obj_set_style_img_recolor(tage_img_1, ANI_ON, 0);
        lv_img_set_src(tage_img_1, &signnum_2);
        
        lv_obj_align(tage_img_1, LV_ALIGN_CENTER, 0, 0);
    }

    //custom_static_chart_create(); //静态曲线图

    lv_obj_t* play_btn = lv_btn_create(parent);
    lv_obj_set_size(play_btn, lv_pct(100), lv_pct(13));
    lv_obj_add_style(play_btn, &list_item_style, 0);
    lv_obj_set_style_bg_opa(play_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(play_btn, BAR_GREEN, 0);
    lv_obj_align(play_btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    //lv_obj_add_event_cb(play_btn, free_chart_resources, LV_EVENT_CLICKED, NULL);

    lv_obj_t * left_label = lv_label_create(play_btn);
    lv_label_set_text(left_label, LV_SYMBOL_UP);
    lv_obj_set_style_text_color(left_label, lv_color_black(), 0);
    lv_obj_align(left_label, LV_ALIGN_LEFT_MID, 4, 0);

    lv_obj_t * center_label = lv_label_create(play_btn);
    lv_label_set_text(center_label, "完成");
    lv_obj_set_style_text_color(center_label, lv_color_black(), 0);
     lv_obj_set_style_text_font(center_label, (lv_font_t *)&lv_font_montserrat_18, 0); 
    // lv_obj_set_style_text_font(center_label, (lv_font_t *)&lv_font_sourcehan_sans_rhw_18, 0);
    lv_obj_center(center_label);
    
    lv_obj_t * right_img = lv_img_create(play_btn);
    lv_obj_set_style_img_recolor_opa(right_img, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(right_img, lv_color_black(), 0);
    lv_img_set_src(right_img, LV_SYMBOL_OK "Accept");
    lv_obj_align(right_img, LV_ALIGN_RIGHT_MID, -4, 0);
}


/**
 * Create a 1x2 tile view and allow scrolling only in an "-" shape.
 * Demonstrate scroll chaining with a long list that
 * scrolls the tile view when it can't be scrolled further.
 */
static void task_page_create(){

    lv_obj_t * tv = lv_tileview_create(lv_scr_act());
    lv_obj_add_style(tv, &list_item_style, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    // tile1 = lv_tileview_add_tile(tv, 0, 0, (lv_dir_t)(LV_DIR_RIGHT | LV_DIR_BOTTOM)); // Tile1: 只能向右滑到 Tile2
    // tile2 = lv_tileview_add_tile(tv, 1, 0, (lv_dir_t)(LV_DIR_LEFT | LV_DIR_RIGHT));
    // add_tile(tile1, true);
    // add_tile(tile2, false);

    // lv_obj_set_user_data(tile1, (void *)1);  // tile1 的 ID=1
    // lv_obj_set_user_data(tile2, (void *)2);  // tile2 的 ID=2
    // lv_obj_add_event_cb(tv, tileview_scroll_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


    lv_obj_t * tile3 = lv_tileview_add_tile(tv, 1, 0, (lv_dir_t)(LV_DIR_RIGHT | LV_DIR_TOP));
    add_tile3(tile3);

    // lv_obj_t * tile4 = lv_tileview_add_tile(tv, 0, 0, (lv_dir_t)(LV_DIR_RIGHT | LV_DIR_TOP));


    // static int32_t col_dsc[] = {58, 58, 58, 58, LV_GRID_TEMPLATE_LAST};
    // static int32_t row_dsc[] = {48, 48, 48, 48, LV_GRID_TEMPLATE_LAST};

    // lv_obj_t * grid = lv_obj_create(tile4);
    // lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    // lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    // lv_obj_set_size(grid, lv_pct(100), 98);
    // lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    // lv_obj_align(grid, LV_ALIGN_TOP_MID, 0, 0);

    // lv_obj_t * label;
    // lv_obj_t * obj;
    // lv_obj_t * img;
    // lv_obj_t * bar;

    // uint8_t i;
    // for(i = 0; i < 8; i++) {
    //     uint8_t col = i % 4;
    //     uint8_t row = i / 4;

    //     obj = lv_button_create(grid);
    //     lv_obj_add_style(obj, &list_item_style, 0);
    //     /*Stretch the cell horizontally and vertically too
    //      *Set span to 1 to make the cell 1 column/row sized*/
    //     lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
    //                          LV_GRID_ALIGN_STRETCH, row, 1);

    //     label = lv_label_create(obj);
    //     lv_label_set_text_fmt(label, "c%d, r%d", col, row);
    //     lv_obj_center(label);

    //     img = lv_img_create(obj);
    //     lv_img_set_src(img, &signnum_2);
    //     lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);

    //     bar = lv_bar_create(obj);
    //     lv_obj_add_style(bar, &list_item_style, 0);
        
    //     lv_bar_set_value(bar, 50, 0);

    // }
    

}

void create_page(void)
{
    init_style();
    lv_obj_set_style_bg_color(lv_scr_act(), CONTENT_BG, LV_PART_MAIN);
    task_page_create();
}

/****************************************************************************************/


static void zoom_event(lv_event_t * e)
{
    
    if (is_small){
        is_small = false;
        lv_obj_set_size(parent_v, 100, 90);
        lv_obj_set_style_bg_color(parent_v, lv_color_hex(0xff6600), 0);
    }else{
        is_small = true;
        lv_obj_set_size(parent_v, 50, 45);
        lv_obj_set_style_bg_color(parent_v, lv_color_hex(0x5555ff), 0);
    }

// 打印实际尺寸
lv_coord_t real_width = lv_obj_get_content_width(parent_v);
lv_coord_t real_height = lv_obj_get_content_height(parent_v);

LV_LOG_USER("Width: %d, Height: %d", 
            real_width, 
            real_height);

} 

void view_zoom(void)
{
    parent_v = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_opa(parent_v, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(parent_v, lv_color_hex(0xff6600), 0);
    lv_obj_set_size(parent_v, 100, 90);
    lv_obj_set_align(parent_v, LV_ALIGN_TOP_MID);

    v_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(v_btn, lv_pct(50), 50);
    lv_obj_set_style_bg_color(v_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_align(v_btn, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_align(v_btn, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_event_cb(v_btn, zoom_event, LV_EVENT_CLICKED, NULL);
    
    label = lv_label_create(v_btn);
    lv_label_set_text(label, "stop");
    lv_obj_center(label);

}