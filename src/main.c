#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define QUESTIONS_ARRAY_SIZE 128

typedef struct question_stuct{
	char		*question;
	char		*answer;
	uint8_t		miss_rate;		// [0-255]
	uint16_t	category;		// [AAAAAA|PPPPPP|TTTT] Argument, Paragraph, Topic
								// 64 Arguments, 64 Paragraphs, 16 Topics
} question_t;

typedef struct heap_struct{
	question_t	*questions;
	uint32_t	size;			//Heap elements number
	
} heap_t;

// i could also consider of providing a pointer for questions alredy allocated and populated and then 
// call a heapify inside here so that after calling heap_t_creator i won't have to do any more operations
// so obtaining the same semantic use in OOP
heap_t* heap_t_creator(void){
	heap_t *new_heap		= (heap_t*) malloc(sizeof(heap_t));
	new_heap -> questions	= malloc(QUESTIONS_ARRAY_SIZE * sizeof(question_t));
	new_heap -> size		= QUESTIONS_ARRAY_SIZE;
	if(!new_heap || !new_heap->questions) return NULL;
	return new_heap;
}

int main (int argc, char** args){
	heap_t *questions_heap = heap_t_creator();
	if(questions_heap == NULL){
		printf("Couldn't allocate memory for questions\n");
	}
	return 0;
}
