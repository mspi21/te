#include "selection.h"

void selection_update_selection(Selection *selection, size_t row, size_t col) {
    selection->row_end = row;
    selection->col_end = col;
}

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

void selection_start_selecting(Selection *selection, size_t row, size_t col) {
    selection->row_start = row;
    selection->row_end   = row;
    selection->col_start = col;
    selection->col_end   = col;
}

void selection_reset(Selection *selection) {
    selection->row_start = 0;
    selection->row_end   = 0;
    selection->col_start = 0;
    selection->col_end   = 0;
}