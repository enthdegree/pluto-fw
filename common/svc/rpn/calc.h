/* calc.h Calculator internals 
*/
#include "math.h"

#define STACK_SIZE 16
#define REG_SIZE 4 
#define TOKEN_SIZE 24

struct calc_state {
    double stack[STACK_SIZE];
    double reg[REG_SIZE];
    char token[TOKEN_SIZE];
    int tidx;
    int pflag;
    int n_stack; 
};

/* input_to_calc Input a character to the calculator
 * Control chars:
 *      'v', 'w'
 *
 * Input: c = char to add to the token
 */
void input_to_calc(struct calc_state * cs, char c);

/* init_calc Initialize calculator
*/
void init_calc(struct calc_state * cs);

/* str_rep Replace all instances of a with b in str (stop at null)
 * Input: str = initial string
 * rep = replacement location
 * a = original char
 * b = thing to replace a with
 */
void str_rep(char * str, char * rep, char a, char b);

/* process_token Process the stack in the way specified by the token.
 * Otherwise, try to convert it to a double and add it to the stack.
 * If that fails, return -1.
 *
 * Input: char * token = Pointer to a token string.
 *        struct calc_state * cs = The stack to process. 
 * Output: 0 if token subroutine completed normally.
 *        -1 if token did not match any subroutine, or stack processing failed.
 *        -2 if stack error.
 *        -3 if register error.
 */
int process_token(char * token, struct calc_state * cs);
