#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses/ncurses.h>
#include <ncurses/form.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main(void)
{
    FIELD *fields[5];
    FORM *my_form;
    int ch;

    /* Initialize curses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    /* Create fields */
    fields[0] = new_field(1, 10, 4, 18, 0, 0);
    fields[1] = new_field(1, 40, 6, 18, 0, 0);
    fields[2] = new_field(1, 40, 8, 18, 0, 0);
    fields[3] = new_field(1, 40, 10, 18, 0, 0);
    fields[4] = NULL;

    /* Set field options */
    set_field_back(fields[0], A_UNDERLINE);
    field_opts_off(fields[0], O_AUTOSKIP);
    set_field_back(fields[1], A_UNDERLINE);
    field_opts_off(fields[1], O_AUTOSKIP);
    set_field_back(fields[2], A_UNDERLINE);
    field_opts_off(fields[2], O_AUTOSKIP);
    set_field_back(fields[3], A_UNDERLINE);
    field_opts_off(fields[3], O_AUTOSKIP);

    /* Create the form and post it */
    my_form = new_form(fields);
    post_form(my_form);
    refresh();

    /* Field labels */
    mvprintw(4, 10, "ID:");
    mvprintw(6, 10, "Name:");
    mvprintw(8, 10, "Email:");
    mvprintw(10, 10, "Phone:");
    mvprintw(14, 10, "Press F1 to exit");
    refresh();

    /* Loop through to get user requests */
    while ((ch = getch()) != KEY_F(1))
    {
        switch (ch)
        {
            case KEY_DOWN:
                /* Go to next field */
                form_driver(my_form, REQ_NEXT_FIELD);
                /* Go to the end of the present buffer */
                form_driver(my_form, REQ_END_LINE);
                break;
            case KEY_UP:
                /* Go to previous field */
                form_driver(my_form, REQ_PREV_FIELD);
                form_driver(my_form, REQ_END_LINE);
                break;
            case KEY_BACKSPACE:
            case 127:
                /* Delete the char before cursor */
                form_driver(my_form, REQ_DEL_PREV);
                break;
            default:
                /* If this is a normal character, it gets printed */
                form_driver(my_form, ch);
                break;
        }
    }

    /* Un post form and free the memory */
    unpost_form(my_form);
    free_form(my_form);
    for (int i = 0; fields[i] != NULL; i++)
        free_field(fields[i]);

    /* End curses mode */
    endwin();

    return 0;
}