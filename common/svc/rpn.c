#include "rpn.h"

static struct svc_rpn_t svc_rpn;

/* Return the location of the rpn calculator's data structures
 */
struct svc_rpn_t * svc_rpn_get(void) {
	return &svc_rpn;
}

/* Initialize calculator data structure
*/
void svc_rpn_init() {
	init_calc(&(svc_rpn.cs));
	mc_reset_buffer(&(svc_rpn.mcb));
	svc_rpn.is_init = 1;
	return;
}

/* Write a morse code event into the morse code buffer.
 * Input: c = character to write 
 */
void svc_rpn_writemc(char c) {
	if('x' == c) {
		// Morse code group ended, write decoding to char d
		char d;
		mc_writechar(&(svc_rpn.mcb), &d);
		mc_reset_buffer(&(svc_rpn.mcb));

		// Input this char to the calculator
		input_to_calc(&(svc_rpn.cs), d);
	}
	else mc_read(&(svc_rpn.mcb), c);
	return;
}
