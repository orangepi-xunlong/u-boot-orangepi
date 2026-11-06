/* SCHEME.H */

#ifndef _SCHEME_H
#define _SCHEME_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USE_OBJECT_LIST 1
#define USE_NO_FEATURES 1
#define USE_DEBUG_IO 1

/*
 * Default values for #define'd symbols
 */
#ifndef BAREMETAL
#define BAREMETAL 0
#endif

#define USE_STRCASECMP 1
#define USE_STRLWR 1

/*
 * Leave it defined if you want continuations, and also for the Sharp Zaurus.
 * Undefine it if you only care about faster speed and not strict Scheme compatibility.
 */
#define USE_SCHEME_STACK

#ifndef USE_STRCASECMP /* stricmp for Unix */
#define USE_STRCASECMP 0
#endif

#ifndef USE_STRLWR
#define USE_STRLWR 1
#endif

#ifndef STDIO_ADDS_CR /* Define if DOS/Windows */
#define STDIO_ADDS_CR 0
#endif

#ifndef INLINE
#define INLINE
#endif

#ifndef SHOW_ERROR_LINE /* Show error line in file */
#define SHOW_ERROR_LINE 1
#endif

typedef struct scheme scheme;
typedef struct cell *pointer;

typedef void *(*func_alloc)(size_t);
typedef void (*func_dealloc)(void *);

/* num, for generic arithmetic */
typedef struct num {
	char is_fixnum;
	union {
		long ivalue;
		long rvalue;
	} value;
} num;

int scheme_init_custom_alloc(scheme *sc, func_alloc malloc, func_dealloc free);
void scheme_deinit(scheme *sc);
void scheme_set_input_port_string(scheme *sc, char *start, char *past_the_end);
void scheme_set_output_port_string(scheme *sc, char *start, char *past_the_end);
void scheme_load_string(scheme *sc, const char *cmd);
pointer scheme_apply0(scheme *sc, const char *procname);
pointer scheme_call(scheme *sc, pointer func, pointer args);
pointer scheme_eval(scheme *sc, pointer obj);
void scheme_set_external_data(scheme *sc, void *p);
void scheme_define(scheme *sc, pointer env, pointer symbol,
				 pointer value);

typedef pointer (*foreign_func)(scheme *, pointer);

pointer _cons(scheme *sc, pointer a, pointer b, int immutable);
pointer mk_integer(scheme *sc, long num);
pointer mk_real(scheme *sc, long num);
pointer mk_symbol(scheme *sc, const char *name);
pointer gensym(scheme *sc);
pointer mk_string(scheme *sc, const char *str);
pointer mk_counted_string(scheme *sc, const char *str, int len);
pointer mk_empty_string(scheme *sc, int len, char fill);
pointer mk_character(scheme *sc, int c);
pointer mk_foreign_func(scheme *sc, foreign_func f);
void putstr(scheme *sc, const char *s);
int list_length(scheme *sc, pointer a);
int eqv(pointer a, pointer b);

typedef struct scheme_registerable {
	foreign_func f;
	const char *name;
} scheme_registerable;

void scheme_register_foreign_func_list(scheme *sc, scheme_registerable *list,
				       int n);


#ifdef __cplusplus
}
#endif

#endif
