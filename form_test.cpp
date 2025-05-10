#include <form.h>
#include <ncurses.h>

int main() {
    FIELD *fields[3];
    FORM  *form;

    // Initialize curses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Create fields
    fields[0] = new_field(1, 20, 4, 10, 0, 0);  // Row 4, Col 10
    fields[1] = new_field(1, 20, 6, 10, 0, 0);  // Row 6, Col 10
    fields[2] = NULL; // End of field list

    // Set field options
    for (int i = 0; i < 2; ++i) {
        set_field_back(fields[i], A_UNDERLINE);  // Display underline
        field_opts_off(fields[i], O_AUTOSKIP);   // Don't autoskip to next
    }

    // Create the form and post it
    form = new_form(fields);
    post_form(form);
    refresh();

    mvprintw(4, 2, "Name:");
    mvprintw(6, 2, "Age:");
    refresh();

    int ch;
    while ((ch = getch()) != KEY_F(1)) { // Press F1 to exit
        switch (ch) {
            case KEY_DOWN:
                form_driver(form, REQ_NEXT_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_UP:
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_BACKSPACE:
            case 127:
                form_driver(form, REQ_DEL_PREV);
                break;
            default:
                form_driver(form, ch);
                break;
        }
    }

    // Cleanup
    unpost_form(form);
    free_form(form);
    free_field(fields[0]);
    free_field(fields[1]);
    endwin();
    return 0;
}
