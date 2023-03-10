# Issue tracker

## IN PROGRESS
- [ ] fix multiple line paste

## TODO
- [ ] refactor selection_is_nonempty into selection_is_empty
- [ ] fix word skip skipping first whitespace
- [ ] fix adjust_view_to_cursor on first invisible line
- [ ] zenity --confirm-overwrite
- [ ] primary selection
- [ ] ctrl +-
- [ ] automatic indentation on new lines
- [ ] refactor selection?
- [ ] reset selection on file change (potential segfault)
- [ ] clamp scrolling	
- [ ] swapping lines w/ selection
- [ ] line numbers
- [ ] syntax highlighting (for now just C)
- [ ] function collapsing
- [ ] actual tabs
- [ ] Ctrl+F (search in file)
- [ ] horizontal cursor position persistence
- [ ] document function headers
- [ ] file dialog should start in "current" directory
- [ ] test for memory leaks

## BACKLOG
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
