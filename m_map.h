#ifndef M_MAP_C
#define M_MAP_C


typedef struct m_granule{
	char* 				address;
	struct m_granule* next;
	struct m_granule* previous;
} m_granule;


typedef struct m_map{
	void* 			start;
	void* 			end;
	size_t 			var_size;	
	m_granule*		list_start;
	m_granule*		list_end;
	struct m_map*	previous_arena;
	struct m_map*	end_arena;

}m_map;


m_granule*	make_granule();

m_granule*	alloc_granule(m_map* , void* );

m_map*		init_mem(size_t , int );

void*			alloc_mem(m_map*);

void*			release_mem(void*, m_map*);

void			extend_mem( m_map**, m_granule**);

void			free_mem(m_map*);
void 			del_list(m_map*);
void			reset(m_map*);
void			defrag_mem(m_map*);

#endif
