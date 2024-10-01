#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <strings.h>

FILE * output;
uint8_t book[4096];
uint8_t subb[4096];
uint8_t moves[100];
int book_indx, subb_indx;
char name[] = "....#0x1000.BIN";

char lowercase(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    else return c;
}

void parse_branch(bool copy) {
    int level = 1;
    while (level != 0 && subb[subb_indx]) {
        uint8_t byte = subb[subb_indx++];
        if (copy) { putc(byte, output); fflush(output); }
        switch (byte & 0xC0) {
            case 0x40: level--;
            case 0x00: break;
            case 0xC0: if (byte != 0xC0)  { printf("%02x in %s at %03x\n", byte,  name, subb_indx-1); break; }
            case 0x80: level++;
        }
    }
}

void synchronize(int depth) {
    bool found = false;
    subb_indx = 0;
    int level = 0;
    while (subb[subb_indx] && level>=0) {
        uint8_t byte = subb[subb_indx++];
        if (byte == 0xC0) byte = subb[subb_indx++];
        if ((byte & 0x3f) == moves[level]) {
            level++;
            if (level == depth) { found = true; parse_branch(true); }
        } else
            parse_branch(false);
    }
    if (!found) {
        putc(0x41, output);
        printf("1 branch not found in %s\n", name);
    }
}

void read_subbook(int depth) {
    for (int i = 0; i < 4; i++)
        name[i] = lowercase(book[book_indx++]);
    FILE *file = fopen(name, "r");
    assert( file );
    bzero(subb, sizeof subb);
    fread(subb, sizeof subb, 1, file);
    fclose( file );

    synchronize(depth);
}


void parse(int level) {
  int flags;
  do {
    uint8_t byte = book[book_indx++];
    if (byte == 0xC5) {
        read_subbook(level);
        break;
    }
    putc(byte, output); fflush(output);

    flags = byte & 0xC0;
    if (flags == 0xC0) {
        byte = book[book_indx++];
        putc(byte, output); fflush(output);
    }

    moves[level] = byte & 0x3f;

    if (flags == 0x40) break;
    parse(level+1);
  } while (flags & 0x80);
}

int main(void) {
    FILE *file = fopen("b000#0x1000.BIN","r");
    fread(book, sizeof book, 1, file);
    fclose(file);

    output = fopen("full_book", "w");
    parse(0);
    fclose(output);
}

