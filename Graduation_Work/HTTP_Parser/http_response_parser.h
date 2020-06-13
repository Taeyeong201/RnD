#ifndef HTTP_RESP_PARSER_H
#define HTTP_RESP_PARSER_H

#include <stddef.h>
#include <stdbool.h>

// #include "http_status_parse.h"

enum STATUS_LINE_STATE {
  STATUS_STATE_START,
  STATUS_STATE_H_1,
  STATUS_STATE_T_2,
  STATUS_STATE_T_3,
  STATUS_STATE_P_4,
  STATUS_STATE_SLASH_5,
  STATUS_STATE_1_6,
  STATUS_STATE_DOT_7,
  STATUS_STATE_1_8,  
  STATUS_STATE_SPACE_1,
  STATUS_STATE_1_2,
  STATUS_STATE_2_0,
  STATUS_STATE_3_0,
  STATUS_STATE_SPACE_2,
  STATUS_STATE_1_O,
  STATUS_STATE_2_K,
  STATUS_STATE_R_1,
  STATUS_STATE_N_2,
};

/*
 * The initial state for the HTTP status line parser.
 * @return The initial state value for the HTTP status line parser.
 */
enum STATUS_LINE_STATE init_status_line_state();

/*
 * Update the state for the HTTP status line parser.
 * @param state The current state.
 * @param input The input character.
 * @return The new state.
 */
enum STATUS_LINE_STATE update_status_line_state(enum STATUS_LINE_STATE state, char input);

// #include "http_header_parse.h"

enum HEADER_LINE_STATE {
  HEADER_STATE_START,
  HEADER_STATE_NAME,
  HEADER_STATE_SEP,
  HEADER_STATE_SPACE,
  HEADER_STATE_VAL,
  HEADER_STATE_R_1,
  HEADER_STATE_N_2
};

/*
 * The initial state for the HTTP header line parser.
 * @return The initial state value for the HTTP header line parser.
 */
enum HEADER_LINE_STATE init_header_line_state();

/*
 * Update the state for the HTTP header line parser.
 * @param state The current state.
 * @param input The input character.
 * @return The new state.
 */
enum HEADER_LINE_STATE update_header_line_state(enum HEADER_LINE_STATE state, char input);

// HTTP_BLANK_PARSE_H
// #include "http_blank_line_parse.h"

enum BLANK_LINE_STATE {
  BLANK_STATE_START,
  BLANK_STATE_R_1,
  BLANK_STATE_N_2,
};

/*
 * The initial state for the HTTP blank line parser.
 * @return The initial state value for the HTTP blank line parser.
 */
enum BLANK_LINE_STATE init_blank_line_state();

/*
 * Update the state for the HTTP blank line parser.
 * @param state The current state.
 * @param input The input character.
 * @return The new state.
 */
enum BLANK_LINE_STATE update_blank_line_state(enum BLANK_LINE_STATE state, char input);




enum HTTP_PARSE_STATE {
  HTTP_STATE_STATUS_LINE,
  HTTP_STATE_HEADER_OR_BLANK_LINE,
  HTTP_STATE_HEADER_LINE,
  HTTP_STATE_BLANK_LINE,
  HTTP_BODY,
  HTTP_STATE_DONE
};

struct http_parse_state {
  enum HTTP_PARSE_STATE state;
  size_t header_byte_position;
  size_t header_line_position;
  size_t body_length;
  size_t body_position;
  enum STATUS_LINE_STATE status_line_state;
  enum HEADER_LINE_STATE header_line_state;
  enum BLANK_LINE_STATE blank_line_state;
};

/*
 * Initialize the HTTP state.
 * @return A fresh empty initialized HTTP state.
 */
struct http_parse_state init_http_state();

/*
 * Parse one character of a HTTP state.
 * @param state The previous state. On the first call, this should the inital state.
 * @param input The character to be parsed.
 * @param headers A array of character buffers to hold headers.
 * @param max_header_size The maximum length of a header line. Should be the length of a header in headers.
 * @param max_header_count THe maximum amount of headers. Should be the length of headers.
 * @param body A buffer to hold body data.
 * @param max_body_length The maximum length of the body. Should be the size of the body buffer parameter.
 * @param failure True if there was an error. False if successful.
 * @return The new state object. Should be used for future calls to http_parse_state.
 */
struct http_parse_state update_http_state(struct http_parse_state state, char input, char * headers, size_t max_header_size, size_t max_header_count, char * body, size_t max_body_length, _Bool * failure);

/*
 * Retrieve a value from a a buffer of HTTP headers.
 * @param key The name of the HTTP header field.
 * @param header_value A buffer for storing the HTTP header value. It is assume to be as big as the header_line_size.
 * @param header_line_size The size of each header_line.
 * @param header_line_count The amount of lines in the header.
 */
_Bool find_header(char * key, char * header_value, char * headers, size_t header_line_size, size_t header_line_count);

#endif
