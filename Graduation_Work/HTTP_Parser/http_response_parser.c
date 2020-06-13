#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "http_response_parser.h"

enum STATUS_LINE_STATE init_status_line_state() {
  return STATUS_STATE_START;
}

enum STATUS_LINE_STATE update_status_line_state(enum STATUS_LINE_STATE state, char input) {
  enum STATUS_LINE_STATE new_state;
  switch (state) {
  case STATUS_STATE_START:
    if (input == 'H') {
      new_state = STATUS_STATE_H_1;
    }
    break;
  case STATUS_STATE_H_1:
    if (input == 'T') {
      new_state = STATUS_STATE_T_2;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_T_2:
    if (input == 'T') {
      new_state = STATUS_STATE_T_3;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_T_3:
    if (input == 'P') {
      new_state = STATUS_STATE_P_4;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_P_4:
    if (input == '/') {
      new_state = STATUS_STATE_SLASH_5;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_SLASH_5:
    if (input == '1') {
      new_state = STATUS_STATE_1_6;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_1_6:
    if (input == '.') {
      new_state = STATUS_STATE_DOT_7;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_DOT_7:
    if (input == '1') {
      new_state = STATUS_STATE_1_8;
    } else {
      new_state = STATUS_STATE_START;
    }    
    break;
  case STATUS_STATE_1_8:
    if (input == ' ') {
      new_state = STATUS_STATE_SPACE_1;
    } else {
      new_state = STATUS_STATE_START;
    }    
    break;
  case STATUS_STATE_SPACE_1:
    if (input == '2') {
      new_state = STATUS_STATE_1_2;
    } else if (input == ' ') {
      new_state = STATUS_STATE_SPACE_1;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_1_2:
    if (input == '0') {
      new_state = STATUS_STATE_2_0;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_2_0:
    if (input == '0') {
      new_state = STATUS_STATE_3_0;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_3_0:
    if (input == ' ') {
      new_state = STATUS_STATE_SPACE_2;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_SPACE_2:
    if (input == 'O') {
      new_state = STATUS_STATE_1_O;
    } else if (input == ' ') {
      new_state = STATUS_STATE_SPACE_2;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_1_O:
    if (input == 'K') {
      new_state = STATUS_STATE_2_K;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_2_K:
    if (input == '\r') {
      new_state = STATUS_STATE_R_1;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_R_1:
    if (input == '\n') {
      new_state = STATUS_STATE_N_2;
    } else {
      new_state = STATUS_STATE_START;
    }
    break;
  case STATUS_STATE_N_2:
    new_state = state;
    break;
  default:
    break;//dbgPanic(0);
  }
  return new_state;
}


enum HEADER_LINE_STATE init_header_line_state() {
  return HEADER_STATE_START;
}

/*
 * Check if argument is a valid part of a word.
 * @param True if argument is a digit or alpha.
 */
static int valid_value_letter(char letter) {
  int is_digit = (48 <= letter) && (letter <= 57);
  int is_alpha_down = (65 <= letter) && (letter <= 90);
  int is_alpha_up = (97 <= letter) && (letter <= 122);
  int is_dash = letter == '-';
  int is_slash = letter == '/';
  int is_colon = letter == ':';
  int is_semicolon = letter == ';';
  int is_comma = letter == ',';
  int is_dot = letter == '.';
  int is_space = letter == ' ';
  int is_equal = letter == '=';
  return is_digit || is_alpha_down || is_alpha_up || is_dash || is_slash || is_colon || is_semicolon || is_comma || is_dot || is_space || is_equal;
}

static int valid_key_letter(char letter) {
  int is_digit = (48 <= letter) && (letter <= 57);
  int is_alpha_down = (65 <= letter) && (letter <= 90);
  int is_alpha_up = (97 <= letter) && (letter <= 122);
  int is_dash = letter == '-';
  int is_slash = letter == '/';
  int is_semicolon = letter == ';';
  int is_comma = letter == ',';
  int is_dot = letter == '.';
  return is_digit || is_alpha_down || is_alpha_up || is_dash || is_slash || is_semicolon || is_comma || is_dot;
}

enum HEADER_LINE_STATE update_header_line_state(enum HEADER_LINE_STATE state, char input) {
  enum HEADER_LINE_STATE new_state;
  switch (state) {
  case HEADER_STATE_START:
    if (valid_key_letter(input)) {
      new_state = HEADER_STATE_NAME;
    } else {
      new_state = HEADER_STATE_START;
    }
    break;
  case HEADER_STATE_NAME:
    if (valid_key_letter(input)) {
      new_state = HEADER_STATE_NAME;
    } else if (input == ':') {
      new_state = HEADER_STATE_SEP;
    } else {
      new_state = HEADER_STATE_START;
    }
    break;
  case HEADER_STATE_SEP:
    if (input == ' ') {
      new_state = HEADER_STATE_SPACE;
    } else if (valid_value_letter(input)) {
      new_state =  HEADER_STATE_VAL;
    } else {
      new_state = HEADER_STATE_START;
    }    
    break;
  case HEADER_STATE_SPACE:
    if (valid_value_letter(input)) {
      new_state = HEADER_STATE_VAL;
    } else {
      new_state = HEADER_STATE_START;
    }
    break;
  case HEADER_STATE_VAL:
    if (valid_value_letter(input)) {
      new_state = HEADER_STATE_VAL;      
    } else if (input == '\r') {
      new_state = HEADER_STATE_R_1;
    } else {
      new_state = HEADER_STATE_START;
    }
    break;
  case HEADER_STATE_R_1:
    if (input == '\n') {
      new_state = HEADER_STATE_N_2;
    } else {
      new_state = HEADER_STATE_START;
    }
    break;
  case HEADER_STATE_N_2:
    new_state = HEADER_STATE_N_2;
    break;
  default:
    break;//dbgPanic(0);
  }
  return new_state;
}


enum BLANK_LINE_STATE init_blank_line_state() {
  return BLANK_STATE_START;
}

enum BLANK_LINE_STATE update_blank_line_state(enum BLANK_LINE_STATE state, char input) {
  enum BLANK_LINE_STATE new_state;
  switch (state) {
  case BLANK_STATE_START:
    if (input == '\r') {
      new_state = BLANK_STATE_R_1;
    } else {
      new_state = BLANK_STATE_START;
    }
    break;    
  case BLANK_STATE_R_1:
    if (input == '\n') {
      new_state = BLANK_STATE_N_2;
    } else {
      new_state = BLANK_STATE_START;
    }    
    break;
  case BLANK_STATE_N_2:
    new_state = BLANK_STATE_N_2;
    break;
  }
  return new_state;
}



struct http_parse_state init_http_state() {
  struct http_parse_state state;
  state.state = HTTP_STATE_STATUS_LINE;
  state.header_byte_position = 0;
  state.header_line_position = 0;
  state.body_length = 0;
  state.body_position = 0;
  state.status_line_state = init_status_line_state();
  state.header_line_state = init_header_line_state();
  state.blank_line_state = init_blank_line_state();
  return state;
}

struct http_parse_state update_http_state(struct http_parse_state state, char input, char * headers, size_t max_header_size, size_t max_header_count, char * body, size_t max_body_length, _Bool * failure) {
  *failure = 0;
  switch (state.state) {
  case HTTP_STATE_STATUS_LINE:
    state.status_line_state = update_status_line_state(state.status_line_state, input);
    if (state.status_line_state == STATUS_STATE_N_2) {
      state.state = HTTP_STATE_HEADER_OR_BLANK_LINE;
    }
    if (state.status_line_state == STATUS_STATE_START) {
      *failure = 1;
      return init_http_state();
    }
    break;
  case HTTP_STATE_HEADER_OR_BLANK_LINE:
    if (input == '\r') {
      state.state = HTTP_STATE_BLANK_LINE;
      state.blank_line_state = update_blank_line_state(state.blank_line_state, input);
      char header_value[256]; //TODO! This exposes a buffer overflow vuln for header_values longer than 256 bytes.

      if (find_header("Content-Length", header_value, headers, max_header_size, state.header_line_position)) {
	if (sscanf(header_value, "%d", &state.body_length) != 1) {
	  *failure = 1;
	  return init_http_state();
	}
      }
    } else {
      state.state = HTTP_STATE_HEADER_LINE;
      headers[(state.header_line_position * max_header_size) + state.header_byte_position++] = input;
      state.header_line_state = update_header_line_state(HEADER_STATE_START, input);
      if (state.header_line_state == HEADER_STATE_START) {
	*failure = 1;
	return init_http_state();
      }
    }
    break;
  case HTTP_STATE_HEADER_LINE:
    if (state.header_line_position >= (max_header_count - 1)) {
      *failure = 1;
      return init_http_state();
    }
    if (state.header_byte_position >= (max_header_size - 1)) {
      *failure = 1;
      return init_http_state();
    }
    headers[(state.header_line_position * max_header_size) + state.header_byte_position++] = input;
    state.header_line_state = update_header_line_state(state.header_line_state, input);
    if (state.header_line_state == HEADER_STATE_N_2) {
      state.state = HTTP_STATE_HEADER_OR_BLANK_LINE;
      headers[(state.header_line_position * max_header_size) + state.header_byte_position] = '\0';
      state.header_line_position++;
      state.header_byte_position = 0;
    } else if (state.header_line_state == HEADER_STATE_START) {
      *failure = 1;
      return init_http_state();
    }    
    break;
  case HTTP_STATE_BLANK_LINE:
    state.blank_line_state = update_blank_line_state(state.blank_line_state, input);
    if (state.blank_line_state == BLANK_STATE_START) {
      *failure = 1;
      return init_http_state();
    } else if (state.blank_line_state == BLANK_STATE_N_2) {
      if (state.body_length == 0) {
	state.state = HTTP_STATE_DONE;
      } else {
	state.state = HTTP_BODY;
      }
    }
    break;
  case HTTP_BODY:
    if (state.body_position >= (max_body_length - 1)) {
      *failure = 1;
      return init_http_state();
    }
    body[state.body_position++] = input;
    if (state.body_position == state.body_length) {
      body[state.body_position] = '\0';
      state.state = HTTP_STATE_DONE;
    }
    break;
  case HTTP_STATE_DONE:
    state = init_http_state();
    state.status_line_state = update_status_line_state(state.status_line_state, input);
    if (state.status_line_state == STATUS_STATE_START) {
      *failure = 1;
      return init_http_state();
    }
    break;
  default:
    break; //dbgPanic(0x00);
  }
  return state;
}

_Bool find_header(char * key, char * header_value, char * headers, size_t header_line_size, size_t header_line_count) {
  size_t line_n;
  int success = 0;
  char format_buffer[256];
  snprintf(format_buffer, 256, "%s: %%%ds\r\n", key, header_line_size - 1);
   for (line_n = 0; line_n < header_line_count; line_n++) {
    success = sscanf(headers + (line_n * header_line_size), format_buffer, header_value);
    if (success == 1) { // Do not change this! Success is not a boolean.
      return true;
    }
  }
  return false;
}
