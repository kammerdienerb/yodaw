#include "internal.h"

yodaw_state *ys;

static void handle_hup(int sig) {
    ys->status = YODAW_RELOAD;
}

static void register_hup_handler(void) {
    struct sigaction sa;

     // Setup the sighub handler
    sa.sa_handler = &handle_hup;

    // Restart the system call, if at all possible
    sa.sa_flags = SA_RESTART;

    // Block every signal during the handler
    sigfillset(&sa.sa_mask);

    sigaction(SIGHUP, &sa, NULL);
}

yodaw_state* yodaw_init(yodaw_lib_t *yodaw_lib, int argc, char **argv) {
    ys = malloc(sizeof(*ys));
    memset(ys, 0, sizeof(*ys));

    ys->yodaw_lib = yodaw_lib;

    lib_initialize();
    ys->lib_state = lib_get_state();

    register_hup_handler();

    init_ui_state();
    ui_update();
    ys->scale = ui_get_scale_factor();

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    ys->win = SDL_CreateWindow("yodaw",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH * ys->scale, WINDOW_HEIGHT * ys->scale,
        SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_RESIZABLE);
    ys->glContext = SDL_GL_CreateContext(ys->win);
    SDL_GetWindowSize(ys->win, &ys->win_width, &ys->win_height);
    ys->bg.r = 0.10f, ys->bg.g = 0.18f, ys->bg.b = 0.24f, ys->bg.a = 1.0f;

    ys->ctx = nk_sdl_init(ys->win);

    nk_sdl_font_stash_begin(&ys->atlas);
    ys->font = nk_font_atlas_add_from_file(ys->atlas, "ProggyClean.ttf", 14 * ys->scale, 0);
    nk_sdl_font_stash_end();
    nk_style_set_font(ys->ctx, &ys->font->handle);

    return ys;
}

void yodaw_fini(yodaw_state *state) {
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(ys->glContext);
    SDL_DestroyWindow(ys->win);
    SDL_Quit();
    free(state);
}

void yodaw_set_state(yodaw_state *state)    { ys = state; }
yodaw_state * yodaw_get_state(void)         { return ys;  }

void do_ui_item(ui_item_id);

void do_window(ui_item_id win) {
    int             t, l, h, w;
    struct nk_rect  bounds;
    unsigned int    flags;
    char           *text;
    int             c, n_children;
    ui_item_id     *children;
    int             win_state, shown_before;

    ui_item_get_dims(win, &t, &l, &h, &w);

    t *= ys->scale;
    l *= ys->scale;
    h *= ys->scale;
    w *= ys->scale;

    flags        = ui_item_get_flags(win);
    text         = ui_item_get_text(win);
    win_state    = ui_item_get_win_state(win);
    shown_before = !nk_window_is_hidden(ys->ctx, text);

    /* Set the bounds to be whatever the UI wants them to be. */
    nk_window_set_bounds(ys->ctx, text, nk_rect(l, t, w, h));

    if (nk_begin(ys->ctx, text, nk_rect(l, t, w, h), flags)) {
        n_children = ui_item_get_num_children(win);
        children   = ui_item_get_children(win);

        for (c = 0; c < n_children; c += 1) {
            do_ui_item(children[c]);
        }

        bounds = nk_window_get_bounds(ys->ctx);
        /* Update the bounds in the UI state. */
        ui_item_set_dims(win,
                         bounds.y / ys->scale,
                         bounds.x / ys->scale,
                         bounds.h / ys->scale,
                         bounds.w / ys->scale);
    }
    nk_end(ys->ctx);

    /*
     * Update the win_state if we were shown before,
     * but now have been hidden.
     */
    if (shown_before
    &&  nk_window_is_hidden(ys->ctx, text)) {

        win_state = WIN_STATE_CLOSED;
    }

    ui_item_set_win_state(win, win_state);

    nk_window_show(ys->ctx,
                   text,
                   (win_state != WIN_STATE_CLOSED ? NK_SHOWN : NK_HIDDEN));
}

void do_row_layout_static(ui_item_id row) {
    int             t, l, h, w;
    int             c, n_children;
    ui_item_id     *children;

    ui_item_get_dims(row, &t, &l, &h, &w);

    h *= ys->scale;
    w *= ys->scale;

    n_children = ui_item_get_num_children(row);
    children   = ui_item_get_children(row);

    nk_layout_row_static(ys->ctx, h, w, n_children);
    for (c = 0; c < n_children; c += 1) {
        do_ui_item(children[c]);
    }
}

void do_row_layout_dynamic(ui_item_id row) {
    int             t, l, h, w;
    int             c, n_children;
    ui_item_id     *children;

    ui_item_get_dims(row, &t, &l, &h, &w);

    h *= ys->scale;

    n_children = ui_item_get_num_children(row);
    children   = ui_item_get_children(row);

    nk_layout_row_dynamic(ys->ctx, h, n_children);
    for (c = 0; c < n_children; c += 1) {
        do_ui_item(children[c]);
    }
}

void do_button(ui_item_id button) {
    char *text;

    text = ui_item_get_text(button);

    if (nk_button_label(ys->ctx, text)) {
        act_on_ui_item(button);
    }
}

void do_menu_label(ui_item_id label) {
    int         t, l, h, w;
    char       *text;
    int         c, n_children;
    ui_item_id *children;

    ui_item_get_dims(label, &t, &l, &h, &w);

    h *= ys->scale;
    w *= ys->scale;

    text = ui_item_get_text(label);

    if (nk_menu_begin_label(ys->ctx, text, NK_TEXT_LEFT, nk_vec2(w, h))) {
        n_children = ui_item_get_num_children(label);
        children   = ui_item_get_children(label);

        for (c = 0; c < n_children; c += 1) {
            do_ui_item(children[c]);
        }

        nk_menu_end(ys->ctx);
    }
}

void do_menu_bar(ui_item_id menu_bar) {
    int             c, n_children;
    ui_item_id     *children;

    n_children = ui_item_get_num_children(menu_bar);
    children   = ui_item_get_children(menu_bar);

    nk_menubar_begin(ys->ctx);
    for (c = 0; c < n_children; c += 1) {
        do_ui_item(children[c]);
    }
    nk_menubar_end(ys->ctx);
}

void do_label(ui_item_id label) {
    char *text;

    text = ui_item_get_text(label);

    nk_label(ys->ctx, text, NK_TEXT_CENTERED);
}

void do_simple_combobox(ui_item_id combobox) {
    int         t, l, h, w;
    int         n_options;
    char      **options;
    int         selected;

    ui_item_get_options(combobox, &n_options, &options);
    selected = ui_item_get_selected(combobox);
    ui_item_get_dims(combobox, &t, &l, &h, &w);

    h *= ys->scale;
    w *= ys->scale;

    selected = nk_combo(ys->ctx, (const char **)options, n_options, selected, 14 * ys->scale, nk_vec2(w, h));

    ui_item_set_selected(combobox, selected);
}

void do_ui_item(ui_item_id item) {
    int kind;

    kind = ui_item_get_kind(item);

    switch (kind) {
        case UI_WIN:                do_window(item);             break;
        case UI_ROW_LAYOUT_STATIC:  do_row_layout_static(item);  break;
        case UI_ROW_LAYOUT_DYNAMIC: do_row_layout_dynamic(item); break;
        case UI_BUTTON:             do_button(item);             break;
        case UI_MENU_BAR:           do_menu_bar(item);           break;
        case UI_MENU_LABEL:         do_menu_label(item);         break;
        case UI_LABEL:              do_label(item);              break;
        case UI_SIMPLE_COMBOBOX:    do_simple_combobox(item);    break;
    }
}

int yodaw_pump(void) {
    ui_item_id *windows;
    int         w, n_wins;
    float       new_scale;

    if (ys->status == YODAW_QUIT) {
        return YODAW_QUIT;
    } else if (ys->status == YODAW_RELOAD) {
        lib_set_state(ys->lib_state);
        register_hup_handler();
        ui_handle_reload();
    }

    ys->status = YODAW_NORMAL;

    /* Input */
    SDL_Event evt;
    nk_input_begin(ys->ctx);
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_QUIT) {
            ys->status = YODAW_QUIT;
        }
        nk_sdl_handle_event(&evt);
    }
    nk_input_end(ys->ctx);

    /* GUI */
    n_wins    = ui_get_num_windows();
    windows   = ui_get_windows();
    for (w = 0; w < n_wins; w += 1) {
        do_window(windows[w]);
    }

    if (ui_update() == UI_STATE_QUIT) {
        ys->status = YODAW_QUIT;
        return ys->status;
    }

    /* Draw */
    SDL_GetWindowSize(ys->win, &ys->win_width, &ys->win_height);
    glViewport(0, 0, ys->win_width, ys->win_height);
    glClear(GL_COLOR_BUFFER_BIT);

    /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
    * with blending, scissor, face culling, depth test and viewport and
    * defaults everything back into a default state.
    * Make sure to either a.) save and restore or b.) reset your own state after
    * rendering the UI. */

    nk_sdl_render(NK_ANTI_ALIASING_ON);
    SDL_GL_SwapWindow(ys->win);

    ui_inform_win_size(ys->win_height, ys->win_width);

    new_scale = ui_get_scale_factor();
    if (ys->scale != new_scale) {
        ys->scale = new_scale;
        nk_font_atlas_clear(ys->atlas);
        nk_sdl_font_stash_begin(&ys->atlas);
        ys->font = nk_font_atlas_add_from_file(ys->atlas, "ProggyClean.ttf", 14 * ys->scale, 0);
        nk_sdl_font_stash_end();
        nk_style_set_font(ys->ctx, &ys->font->handle);
    }

    if (ys->status == YODAW_RELOAD) {
        /* Do anything to handle reload */
    }

    return ys->status;
}
