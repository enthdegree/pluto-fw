#include "rpn.h"
#include "lcd.h"
#include "util.h"
#include "rpn/cordic_wrapped.h"
#include "common/hal/hal.h"

static struct svc_rpn_t svc_rpn;

/* Return the location of the rpn calculator's data structures
 */
struct svc_rpn_t * svc_rpn_get(void) {
	return &svc_rpn;
}

/* Initialize calculator data structure
*/
void svc_rpn_init(void) {
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

/* Print [idx] down from top of stack.
 */
void svc_rpn_print_stack(int idx) { 
	int ns = svc_rpn.cs.n_stack;
	if(idx >= ns) svc_lcd_puts(0, "empty");
	else {
		double d = svc_rpn.cs.stack[ns-1-idx];
		if(d == 0) {
			svc_lcd_puts(0,"0");
			return;
		}
		else if(isnan(d)) {
			svc_lcd_puts(0,"nan");
			return;
		}
		else if(d == (1.0)/(0.0)) {
			svc_lcd_puts(0,"inf");
			return;
		}
		else if(d == (-1.0)/(0.0)) {
			svc_lcd_putc(9, '-');
			svc_lcd_puts(0,"inf");
			return;
		}


		// Sign
		int neg = (d<0);
		if(neg) { 
			svc_lcd_putc(9, '-');
			d = -d;
		}

		int om = (int) floor(log_wc(d)/log_wc(10));

		// Exponent sign
		int expneg = (om<0);
		if(expneg) {
			svc_lcd_putc(6, '-');
		}

		// Make a 4-number-long 'digits' var
		int digits;
		if(om<0) digits = round(1000.0*d*dipow(10.0, -om));
		else digits = round(1000.0*d/dipow(10.0, om));
		if(digits>9999) {
			digits = 1000;
			om++;
		}
		svc_lcd_puti(0,4,digits);

		if((!expneg) && (om < 10)) svc_lcd_puti(5,1,om);
		else if(expneg && (om > -10)) svc_lcd_puti(5,1,-om);
		else if((!expneg) && (om >= 10)) svc_lcd_puti(4,2,om);
		else if(expneg && (om <= -10)) svc_lcd_puti(4,2,-om);
		else if(om>99) {
			// Overflow, print order of magnitude in main 4
		       	svc_lcd_puti(0,3,om);
		       	svc_lcd_puts(4,"of");
		}
		else if(om< -99) {
			// Underflow, print order of magnitude in main 4
			svc_lcd_puti(0,3,-om);
			svc_lcd_puts(4,"uf");
		}
	}
	return;
}
