#ifndef SELECTION_H_
#define SELECTION_H_

#include <stddef.h>

typedef struct {
    size_t row_start;
    size_t col_start;
    size_t row_end;
    size_t col_end;
} Selection;

void selection_update_selection(Selection *selection, size_t row, size_t col);
void selection_get_ordered_range(Selection *selection, size_t *sr, size_t *sc, size_t *er, size_t *ec);
void selection_start_selecting(Selection *selection, size_t row, size_t col);
void selection_reset(Selection *selection);

#endif // SELECTION_H_
