typedef void * map_element_t;
typedef void * context_t;
typedef map_element_t (*maker_function_pointer)(context_t);
typedef void (*destroyer_function_pointer)(map_element_t );

map_element_t storage_get(const char * name, maker_function_pointer maker, context_t context, destroyer_function_pointer destroyer );

