#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
heap_t* heap_t_creator(question_t* questions, size_t size){
	heap_t *new_heap		= (heap_t*) malloc(sizeof(heap_t));
	new_heap -> questions	= questions;
	new_heap -> size		= size;
	if(!new_heap || !new_heap->questions) return NULL;
	return new_heap;
}

question_t* question_array_allocator(cJSON* question_json, size_t size){
	question_t *questions = malloc(sizeof(question_t) * size);
	if(!questions) return NULL;		

	for(int i = 0; i < size; i++){
		cJSON *entry = cJSON_GetArrayItem(question_json, i);

		cJSON *qst = cJSON_GetObjectItem(entry, "question");
		cJSON *ans = cJSON_GetObjectItem(entry, "answer");
		cJSON *msr = cJSON_GetObjectItem(entry, "miss_rate");

		questions[i].question = malloc(strlen(qst->valuestring) + 1);
		questions[i].answer = malloc(strlen(ans->valuestring) + 1);
		//TODO: handle malloc fail
		
		strcpy(questions[i].question, qst->valuestring);
		strcpy(questions[i].answer, ans->valuestring);
		questions[i].miss_rate = msr->valueint;
		//TODO: conver 0x???? into uint16_t

		printf("%d\n", questions[i].miss_rate);
	}


	return questions ;
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
	question_t *questions = question_array_allocator(question_json, questions_number);
	if(!questions){
		fprintf(stderr, "Error while creating question_t array\n");
		exit(EXIT_FAILURE);
	}

	heap_t *questions_heap = heap_t_creator(questions, (size_t)questions_number);
	if(questions_heap == NULL){
		printf("Couldn't allocate memory for questions\n");
	}
	return 0;
}
