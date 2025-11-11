#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include <sys/stat.h>

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

// Takes a file pointer as input and returns a heap-allocated character buffer
// containing the file’s contents. Closes the file pointer as a side effect.
char* read_file(FILE *file_ptr){
	if(!file_ptr) return NULL;
	char *ret_buff;
	
	if(fseek(file_ptr, 0L, SEEK_END) == 0){
		long bufffer_size = ftell(file_ptr);
		if(bufffer_size == -1) return NULL;
		if(fseek(file_ptr, 0L, SEEK_SET) != 0) return NULL;

		ret_buff = malloc(sizeof(char) * (bufffer_size + 1));
		size_t file_len = fread(ret_buff, sizeof(char), bufffer_size, file_ptr);
		if(ferror(file_ptr) == 0){
			ret_buff[file_len++] = '\0';
			fclose(file_ptr);
			return ret_buff;
		}

		return NULL;
	}

	return ret_buff;
}

int main (int argc, char** args){
	FILE *json_ptr = fopen(args[1], "r");
	if(!json_ptr){
		fprintf(stderr, "Error while opening question file\n");
		exit(EXIT_FAILURE);
	}
	
	char *buffer = read_file(json_ptr);
	if(!buffer){
		fprintf(stderr, "Error while reading question file\n");
		exit(EXIT_FAILURE);
	}

	cJSON *question_json = cJSON_Parse(buffer);
	free(buffer);
	
	if (!question_json) {
        const char *err = cJSON_GetErrorPtr();
        fprintf(stderr, "Error while parsing question file\n");
        return EXIT_FAILURE;
    }


	int questions_number = cJSON_GetArraySize(question_json);
	question_t *questions = malloc(questions_number * sizeof(question_t));
	if(!questions){
		fprintf(stderr, "Error while creating question_t array\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < questions_number; i++){
		cJSON *entry = cJSON_GetArrayItem(question_json, i);
		printf("%s\n", cJSON_Print(entry));
	}

	/*
	heap_t *questions_heap = heap_t_creator();
	if(questions_heap == NULL){
		printf("Couldn't allocate memory for questions\n");
	}
	*/
	return 0;
}
