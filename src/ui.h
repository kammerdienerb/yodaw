#ifndef __UI_H__
#define __UI_H__

#define UI_STATE_NORMAL (1)
#define UI_STATE_QUIT   (2)

#define UI_WIN                (1)
#define UI_BUTTON             (2)
#define UI_MENU_BAR           (3)
#define UI_ROW_LAYOUT_STATIC  (4)
#define UI_ROW_LAYOUT_DYNAMIC (5)
#define UI_MENU_LABEL         (6)
#define UI_LABEL              (7)
#define UI_SIMPLE_COMBOBOX    (8)

#define WIN_STATE_NORMAL    (1)
#define WIN_STATE_COLLAPSED (2)
#define WIN_STATE_CLOSED    (3)

typedef int ui_item_id;

/* These are going to come from bJou in ui.bjou. */
void         init_ui_state(void);
void         ui_inform_win_size(int h, int w);
float        ui_get_scale_factor(void);
ui_item_id   ui_get_main_win(void);
int          ui_get_num_windows(void);
ui_item_id * ui_get_windows(void);
void         ui_item_get_dims(ui_item_id item, int *t, int *l, int *h, int *w);
void         ui_item_set_dims(ui_item_id item, int t, int l, int h, int w);
void         ui_item_get_options(ui_item_id item, int *n_options, char ***options);
int          ui_item_get_selected(ui_item_id item);
void         ui_item_set_selected(ui_item_id item, int selected);
int          ui_item_get_kind(ui_item_id item);
int          ui_item_get_win_state(ui_item_id item);
void         ui_item_set_win_state(ui_item_id item, int state);
unsigned int ui_item_get_flags(ui_item_id item);
char *       ui_item_get_text(ui_item_id item);
int          ui_item_get_num_children(ui_item_id item);
ui_item_id * ui_item_get_children(ui_item_id item);
void         act_on_ui_item(ui_item_id item);
int          ui_update(void);
void         ui_handle_reload(void);

#endif
