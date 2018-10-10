#include "common/app/app.h"
#include "common/app/apps.h"
#include "common/svc/svc.h"
#include "common/hal/hal.h"
#include "common/hal/lcd_segments.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    APP_PRIV_COMMON
} priv_t;

#define PRIV(a) ((priv_t*)((a)->priv))
#define PRINT_CURRENT_MC_CHAR char curr; \
		hal_lcd_clear(); \
		mc_writechar(&((*rpn).mcb), &curr); \
		int mcc = (*rpn).mcb.bidx;\
		svc_lcd_putc(7, curr);\
		svc_lcd_puti(8, 1, mcc);\

static void main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {

	struct svc_rpn_t * rpn = svc_rpn_get();
	if(!((*rpn).is_init == 1)) svc_rpn_init();

    if(event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
	// Bottom Right button was pressed (dot)
		svc_rpn_writemc('.');	
		PRINT_CURRENT_MC_CHAR
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_UP) {
	// Top Left button was pressed (dash)
		svc_rpn_writemc('-'); 
		PRINT_CURRENT_MC_CHAR
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
	// Bottom Left button was pressed (end morse code group, print end of current token)
		svc_rpn_writemc('x'); 
		hal_lcd_clear();
		int nlen = strlen((*rpn).cs.token);
		svc_lcd_puts(0, (*rpn).cs.token + nlen-MIN(nlen,6));	
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_ENTER_LONG) {
	// Bottom Right long press  (print top of stack)
		hal_lcd_clear();
		if(0 == (*rpn).cs.n_stack) svc_lcd_puts(0, "empty");
		else {
			double top = (*rpn).cs.stack[(*rpn).cs.n_stack-1];
			
			// Sign
			int sgn = (top < 0);
			if(sgn) svc_lcd_putc(9, '-');
			
			top = abs(top);

			// Exponent's sign
			int esgn = (top < 1) && (top > 0);
			if(esgn) {
				svc_lcd_putc(6, '-');
				top = 1.0/top;
			}

			char topstr[8];
			sprintf(topstr, "%.3g", top);
			int lts = strlen(topstr);
			svc_lcd_puts(0, topstr + lts - MIN(lts,6));	
		}
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_UP_LONG) {
	// Top Left long press (print number of things on stack)
		hal_lcd_clear();
		svc_lcd_puti(1, 1, (*rpn).cs.n_stack);	
	}

	// Bottom Left long press 
	else if(event & SVC_MAIN_PROC_EVENT_KEY_DOWN_LONG) app_exit(); 

	if((*rpn).cs.pflag < 0) {
		hal_lcd_clear();
		if((*rpn).cs.pflag == -1) svc_lcd_puts(0, "comerr");	
		else if((*rpn).cs.pflag == -2) svc_lcd_puts(0, "stkerr");	
		else if((*rpn).cs.pflag == -3) svc_lcd_puts(0, "regerr");	
		(*rpn).cs.pflag = 0;
	}

	return;
}

static app_view_t view = {
	.main = main
};

static priv_t priv = {0};

const app_t app_app_rpn = {
	.n_views = 1,
	.priv = (app_priv_t*)(&priv),
	.views = &view
};
