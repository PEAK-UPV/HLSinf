/*
 * stats.h
 *
 */

// function prototypes
void fn_initializer_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items);
void fn_node_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items);
void fn_input_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items);
void fn_start_timer();
void fn_stop_timer();
unsigned long long fn_get_timer();
