#include "common/app/app.h"
#include "common/app/apps.h"
#include "common/svc/svc.h"
#include "common/hal/hal.h"
#include "common/hal/lcd_segments.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    APP_PRIV_COMMON
} priv_t;

#define PRIV(a) ((priv_t*)((a)->priv))

static void print_current_mc_char(struct svc_rpn_t * rpn) { 
	char curr; 
	mc_writechar(&((*rpn).mcb), &curr); 
	int mcc = (*rpn).mcb.bidx;
	svc_lcd_putc(7, curr);
	svc_lcd_puti(8, 1, mcc);
	return;
}

static void print_current_token(struct svc_rpn_t * rpn) {
		int nlen = strlen((*rpn).cs.token);
		svc_lcd_puts(0, (*rpn).cs.token + nlen-MIN(nlen,6));	
		return;
}

static void main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {

	struct svc_rpn_t * rpn = svc_rpn_get();
	if(!((*rpn).is_init == 1)) svc_rpn_init();

    if(event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
	// Bottom Right button was pressed (dot)
		svc_rpn_writemc('.');	
		hal_lcd_clear();
		print_current_mc_char(rpn);
		print_current_token(rpn);
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_UP) {
	// Top Left button was pressed (dash)
		svc_rpn_writemc('-'); 
		hal_lcd_clear();
		print_current_mc_char(rpn);
		print_current_token(rpn);
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
		// Bottom Left button was pressed (end morse code group, print end of current token)
		hal_lcd_clear();
		svc_rpn_writemc('x'); 
		print_current_token(rpn);
	}

	else if(event & SVC_MAIN_PROC_EVENT_KEY_ENTER_LONG) {
	// Bottom Right long press (print stack)
		hal_lcd_clear();

		char curr; 
		mc_writechar(&((*rpn).mcb), &curr); 

		if(curr >= '1' && curr <= '9') {
			svc_rpn_print_stack((int)(curr-'1'));
			svc_lcd_puti(7, 1, (int)(curr-'1'));
		}
		else {
			svc_rpn_print_stack(0);
			svc_lcd_puti(8, 1, 1);
		}
		svc_lcd_puti(7, 1, (*rpn).cs.n_stack);	

		mc_reset_buffer(&((*rpn).mcb));
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
