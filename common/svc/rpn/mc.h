/* mc Morse code reading methods.
*/
#define BUFFLEN 6 
struct mc_buffer {
    char buffer[BUFFLEN];
    int bidx;
};

void mc_reset_buffer(struct mc_buffer * mcb);

/* Read an input into a morse code buffer 
 * Input: mcb = buffer to read into
 *        c = character to read into buffer ('.' or '-', ignored otherwise).
 */
void mc_read(struct mc_buffer * mcb, char c);

/* Write the contents of a morse code buffer to a char
 * If the buffer contains gibberish write nothing.
 * Input: mcb = buffer containing morse code
 *        *c = location to write decoded char to
 */
void mc_writechar(struct mc_buffer * mcb, char * c);

