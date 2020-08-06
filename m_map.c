
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "m_map.h"


	m_granule* make_granule(){
		m_granule* mem = malloc( sizeof( *mem) );
		if(mem==NULL){
			printf(" failed make_granule\n");
			exit(0);
		}
		mem ->next		= NULL;
		mem ->previous	= NULL;
		return mem;
	}
	
	m_granule* alloc_granule(m_map* map, void* address){
		
		m_granule* new_mem 		= make_granule();
		if( new_mem==NULL){ 
			printf(" alloc granule erro\n");
			exit(0);
		}
		new_mem->address			= address;
		if( map->list_end==NULL){
			map->list_start 		= new_mem;
			map->list_end			= new_mem;
		}else{
			map->list_end->next	= new_mem;
			new_mem->previous		= map->list_end;
			map->list_end			= new_mem;
		}
		
		return new_mem;
	}
	
	m_map* init_mem(size_t var_size, int quant){
			//quant+=1;
			m_map* mem				= (m_map*)malloc( sizeof(m_map));
			void* heap				= (void*)malloc( var_size * quant);
			
			if(heap==NULL) {
				printf (" invalid init_mem \n");
				exit(0);
			}
			
			mem->var_size			= var_size;
			mem->start				= (char*)heap;
			mem->end 				= mem->start+(var_size*(quant+1));
			mem->end_arena			= mem;
			mem->previous_arena 	= NULL;
			mem->list_end			= NULL;
			
			for( size_t loc = 0 ; loc < quant; loc++){
				alloc_granule( mem, (void*)(mem->start + (var_size*loc) ));
			}
			
			return mem;
	}
	
	void** calloc_mem(m_map* heap, size_t size){
	
		char* address		= heap->list_start->address;
		m_granule* list	= heap->list_start;
		size_t count		= 0;
		
		while( count != size){
		
			if(list->next->address == address++) {
				list = list->next;
				address++;
				count++;
			}else{
				count = 0;
			}
		
		}
	
	}
	
	void* alloc_mem(m_map* heap){
		if( heap==NULL){
			printf(" alloc_mem err: NULL HEAP\n");
			exit(0);
		}
		if ( heap->list_start->next == NULL) {
			extend_mem(&heap, &heap->list_end);
		}
		void* mem ;
		mem					= (void*)heap->list_start->address;
		heap->list_start	= heap->list_start->next;
		
		
		return mem;
	}
	
	void* release_mem(void* address,m_map*  heap){
		void* add = (char*) address;
		if( heap==NULL) {
			printf("release_mem: null heap\n");
			exit(0);
		}
		heap->list_start				= heap->list_start->previous;
		heap->list_start->address 	= address;
		*((void**)address)=NULL;
	}
	
	void extend_mem(m_map** heap, m_granule** end){
	
		m_map* arena;
		arena 									= init_mem((*heap)->var_size, 2000);
		arena->var_size						= (*heap)->var_size;
		arena->previous_arena				= (*heap)->end_arena;
		(*heap)->end_arena					= arena;
		(*heap)->list_end->next 			= arena->list_start;
		arena->list_start->previous		= (*heap)->list_end;
		(*heap)->list_start->next				= arena->list_start;
		(*heap)->list_end						= arena->list_end;
	}
	
	void free_end_arena(m_map** mem){
	
		m_map* end = (*mem)->end_arena;
		(*mem)->end_arena= end->previous_arena;
		free( ((end)->start));
		free( end );
		
	}
	
	void free_mem(m_map* mem){
		del_list(mem);
		
		while( mem->end_arena!=mem){
			free_end_arena(&mem);
		}
		
		free( mem->start );
		free( mem );
		
	}
	
	
	void del_list(m_map* mem){
		if ((mem)==NULL) printf(" null mem \n");
		
		m_granule* list	= (mem)->list_end;
		m_granule* tmp		= NULL;
		
		while(list!=NULL){
		
			if(list->previous==NULL || list->address == (mem)->start ) {
				break;
			}
			tmp		= list;
			list		= list->previous;
			free(tmp);
		}
		free(list);
	}
	
	void reset ( m_map* mem){
	m_granule* list = mem->list_start;
	
	while(1){
		if(list->previous==NULL) break;
		list=list->previous;
	}
	mem->list_start= list;
	
	}
	
	m_granule* defrag(m_map* mem){
	
		m_granule* list = mem->list_start;
		
		if( mem->previous_arena != NULL){
			list	= defrag(mem->previous_arena);
		}
		
		void* address = mem->start;
		while( 1 ){
			
			list->address = address;
			
			if(list->address == mem->end || list->next==NULL){
				break;
			}
			address	= address + mem->var_size;
			list		= list->next;
		}
		
		if( list->next!=NULL) return list;
		return NULL;
	}
	
	void defrag_mem ( m_map* mem){
		defrag(mem->end_arena);
	}
	

