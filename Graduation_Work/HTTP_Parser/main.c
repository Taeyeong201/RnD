#include "http_response_parser.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>

void assert(int check, char* description) {
    printf("[%s] - %s\n", check ? "T" : "F", description);
}

void test_parse_from_file() {
    struct http_parse_state state;

    size_t max_head_size = 128;
    size_t max_header_count = 8;
    char headers[1024]; //128*8

    size_t max_body_length = 1000;
    char body[1024];

    _Bool failure = 0;


    state = init_http_state();

    FILE* request_f = fopen("res1.txt", "rb");
    for (;;) {
        char input = fgetc(request_f);
        if (feof(request_f)) {
            break;
        }
        //printf("input char: [%c]\n",input);
        state = update_http_state(state, input, headers, max_head_size, max_header_count, body, max_body_length, &failure);
        if (failure) {
            printf("failure on char %d\n", input);
            break;
        }
    }
    fclose(request_f);
    assert(state.state == HTTP_STATE_DONE, "On Done");

    /* ---------- print result ------------- */
    printf("http response format valid? %s\n", !failure ? "yes" : "no");

    printf("headers size:%d\n", state.header_line_position);

    for (int i = 0; i < state.header_line_position; i++) {
        printf("header %d %s", i, headers + (i * max_head_size));
    }

    printf("body len:%d, content:\n%s\n", strlen(body), body);
}

int main() {
    printf("Test HTTP Response Parser...\n");

    test_parse_from_file();

    printf("Parse finished...\n");
    return 0;
}
