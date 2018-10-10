#include <string.h>

#include "mc.h"

#define RESET_BUFFER \
    memset((*mcb).buffer, '\0', BUFFLEN*sizeof(char));\
    (*mcb).bidx = 0;

/* mc_reset_buffer Initialize or reset an MC buffer
 *
 * Input: mcb = location of buffer to reset
 */
void mc_reset_buffer(struct mc_buffer * mcb) {
    memset((*mcb).buffer, '\0', BUFFLEN*sizeof(char));
    (*mcb).bidx = 0;
    return;
}

/* mc_dec Decode a Morse code string
 *
 * Input: b = Null-terminated string containing up to (BUFFLEN-1) '.' and '-'
 * Output: c = Character b represents, or '\0' if not a Morse code. 
 */
char mc_dec(char b[BUFFLEN]) {
    const char DEC_KEY[] = "\0 etianmsurwdkgohvf\0l\0pjbxcyzq\0\0\x35\x34\0\x33\0\0\0\x32\0\0\0\0\0\0\0\x31\x36\0\0\0\0\0\0\0\x37\0\0\0\x38\0\x39\x30\0\0\0\0\0\0\0\0\0\0\0\0?\0\0\0\0\0\0\0\0.\0\0\0\0\0\0\0\0\0\0\0-\0\0\0\0\0\0\0\0\0!\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    int pos = 1;
    for(int idx=0;idx<BUFFLEN;idx++) {
        if('.' == b[idx]) pos = 2*pos;
        else if('-' == b[idx]) pos = 2*pos+1;
        else if(!b[idx]) break;
    }
    return DEC_KEY[pos];
}

/* mc_read Read an input into a morse code buffer 
 *
 * Input: mcb = buffer to read into
 *        c = character to read into buffer ('.' or '-', ignored otherwise).
 */
void mc_read(struct mc_buffer * mcb, char c) {
    if( ('.' == c) || ('-' == c)) {
        if((*mcb).bidx == BUFFLEN-1) { 
			mc_reset_buffer(mcb);
            return;
        }
        (*mcb).buffer[(*mcb).bidx] = c;
        (*mcb).bidx = ((*mcb).bidx+1)%BUFFLEN;
    }
    return;
}

/* mc_writechar Write the contents of a morse code buffer to a char
 * If the buffer contains gibberish write nothing.
 *
 * Input: mcb = buffer containing morse code
 *        c = location to write decoded char to
 */
void mc_writechar(struct mc_buffer * mcb, char * c) {
    char d = mc_dec((*mcb).buffer);
    if(!('\0' == d)) *c = d;
    return;
}

