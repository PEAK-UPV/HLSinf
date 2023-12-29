/*
 * stats.h
 *
 */

// function prototypes
void fn_buffer_stats(float *b, int num_items, char *text);
void fn_buffer_stats_by_name(char *name, char *text);
void fn_initializer_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items);
void fn_node_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items);
void fn_input_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items);
void fn_start_timer(int x);
void fn_stop_timer(int x);
unsigned long long fn_get_timer(int x);
