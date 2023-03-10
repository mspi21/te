#include "selection.h"

void selection_get_ordered_range(Selection *selection, size_t *sr, size_t *sc, size_t *er, size_t *ec) {
    if(selection->row_start < selection->row_end || (
        selection->row_start == selection->row_end &&
        selection->col_start < selection->col_end)
    ) {
        *sr = selection->row_start;
        *sc = selection->col_start;
        *er = selection->row_end;
        *ec = selection->col_end;
    }
    else if(selection->row_start > selection->row_end || (
        selection->row_start == selection->row_end &&
        selection->col_start > selection->col_end)
    ) {
        *sr = selection->row_end;
        *sc = selection->col_end;
        *er = selection->row_start;
        *ec = selection->col_start;
    }
    else {
        *sr = 0;
        *sc = 0;
        *er = 0;
        *ec = 0;
    }
}

void selection_set(Selection *selection, size_t rs, size_t cs, size_t re, size_t ce) {
    selection->row_start = rs;
    selection->row_end   = re;
    selection->col_start = cs;
    selection->col_end   = ce;
}

void selection_start_selecting(Selection *selection, size_t row, size_t col) {
    selection->row_start = row;
    selection->row_end   = row;
    selection->col_start = col;
    selection->col_end   = col;
    selection->is_selecting = true;
}

void selection_update_selection(Selection *selection, size_t row, size_t col) {
    selection->row_end = row;
    selection->col_end = col;
}

void selection_stop_selecting(Selection *selection) {
    selection->is_selecting = false;
}

bool selection_is_selecting(Selection *selection) {
    return selection->is_selecting;
}

bool selection_is_nonempty(Selection *selection) {
    return selection->row_start != selection->row_end || selection->col_start != selection->col_end;
}

void selection_reset(Selection *selection) {
    selection->row_start = 0;
    selection->row_end   = 0;
    selection->col_start = 0;
    selection->col_end   = 0;
}
