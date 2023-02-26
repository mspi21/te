# Issue tracker

## QA
[x] quitting when changed file
[x] down arrow on last line + up arrow on first line
[x] command line args
[x] ctrl + arrows doesn't skip whole word
[x] clicking in the editor doesn't move cursor
[x] automatic scrolling to cursor when out of view
[x] paste from clipboard
[x] move glEnable BLEND to renderer init
[x] aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa (very long lines, automatic scrolling)
[x] refactor main -- move input handling into separate file / set of functions
[x] changed file should be indicated in the window title
[x] window resizing
[x] rewrite Makefile
[x] selecting column (click) fix rounding
[x] segfault when selecting text and then typing
[x] selection - backspace, delete, enter, typing
[x] copy, cut
[x] fix saving files (segfault)
[x] selection - no mod arrows, skipping words
[x] adjust view after click
[x] saving + closing untitled files (no dialog when quitting with unsaved changes and no loaded file)

## IN PROGRESS
[] refactor "editor" class - split into smaller modules
[] line.{h,c}
[] cursor.{h,c}
[] selection - shift+arrows
[] fix pasting strings with newlines

## TODO
[] shift click
[] automatic indentation on new lines
[] refactor selection?
[] reset selection on file change (potential segfault)
[] clamp scrolling	
[] swapping lines w/ selection
[] line numbers
[] syntax highlighting (for now just C)
[] function collapsing
[] actual tabs
[] Ctrl+F (search in file)
[] horizontal cursor position persistence

## BACKLOG
[] fix click not handled when window gets focus
[] fix baseline
[] multiple cursors
[] blinking cursor
[] glyphs outside ASCII range
[] scroll indicator
[] smooth window resizing
[] consider a change in capitalization to be a word boundary
[] undo
[] config file
[] editor tabs / windows / layout
