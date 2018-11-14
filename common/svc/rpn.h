#pragma once
#include "rpn/mc.h"
#include "rpn/calc.h"

struct svc_rpn_t {
   struct calc_state cs;
   struct mc_buffer mcb; 
   int is_init;
};

/* Return the location of the rpn calculator's data structures
 */
struct svc_rpn_t * svc_rpn_get(void);

/* Initialize calculator data structure
 */
void svc_rpn_init(void);

/* Write morse code to buffer.
 */
void svc_rpn_writemc(char c);

/* Print top of stack.
 */
void svc_rpn_print_stack(int idx);
