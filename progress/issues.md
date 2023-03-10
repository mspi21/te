# Issue tracker

## IN PROGRESS
- [ ] automatic indentation on new lines
- [ ] line numbers

## TODO
- [ ] fix SDL_GetClipboardText returning garbage?
- [ ] refactor selection_is_nonempty into selection_is_empty
- [ ] assure that there's a newline at the end of the file before saving

- [ ] fix adjust_view_to_cursor on first invisible line
- [ ] primary selection
- [ ] clamp scrolling	

## BACKLOG
- [ ] syntax highlighting (for now just C)
- [ ] function collapsing
- [ ] Ctrl+F (search in file)
- [ ] file dialog should start in "current" directory
- [ ] document function headers
- [ ] test for memory leaks
- [ ] fix word skip skipping whitespace
- [ ] ctrl +- (font size), font selection
- [ ] actual tabs
- [ ] fix click not handled when window gets focus
- [ ] fix baseline
- [ ] multiple cursors
- [ ] blinking cursor
- [ ] glyphs outside ASCII range
- [ ] scroll indicator
- [ ] smooth window resizing
- [ ] consider a change in capitalization to be a word boundary
- [ ] undo

## IDEAS FOR FAR FUTURE
- [ ] config file
- [ ] editor tabs / windows / layout
- [ ] automatic generation of .c from .h files
- [ ] annotations + overview of annotations -- todo, fixme, tobetested...

## QA
- [x] zenity --confirm-overwrite
- [x] horizontal cursor position persistence
- [x] reset selection on file change (potential segfault)
- [x] shift click
- [x] selection - shift+arrows
- [x] fix saving existing files created by the editor
- [x] ctrl+A
- [x] write a readme
- [x] fix pasting strings with newlines
- [x] fix cursor weirdness after refactor
- [x] refactor "editor" class - split into smaller modules
- [x] cursor.{h,c}
- [x] line.{h,c}
- [x] saving + closing untitled files (no dialog when quitting with unsaved changes and no loaded file)
- [x] adjust view after click
- [x] selection - no mod arrows, skipping words
- [x] refactor selection
- [x] fix saving files (segfault)
- [x] copy, cut
- [x] selection - backspace, delete, enter, typing
- [x] segfault when selecting text and then typing
- [x] selecting column (click) fix rounding
- [x] rewrite Makefile
- [x] window resizing
- [x] changed file should be indicated in the window title
- [x] refactor main -- move input handling into separate file / set of functions
- [x] aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa (very long lines, automatic scrolling)
- [x] move glEnable BLEND to renderer init
- [x] paste from clipboard
- [x] automatic scrolling to cursor when out of view
- [x] clicking in the editor doesn't move cursor
- [x] ctrl + arrows doesn't skip whole word
- [x] command line args
- [x] down arrow on last line + up arrow on first line
- [x] quitting when changed file
