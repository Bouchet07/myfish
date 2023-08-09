#ifndef UCI_H
#define UCI_H

int parse_move(const char *move_string);
void parse_position(const char *command);
void parse_go(const char *command);
void uci_loop();

#endif