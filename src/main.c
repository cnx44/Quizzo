#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <cjson/cJSON.h>
#include <sys/stat.h>

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


void heap_t_dealloc(heap_t* heap){
	for(int i=0; i<heap->size; i++){
		free(heap->questions[i].question);
		free(heap->questions[i].answer);
	}
	free(heap->questions);
	free(heap);
}

// returns 0 if str is not hexadecimal of chars number == 6, 2 for 0x, 4 for value
int str_is_4dig_hex(char *str){
	const char *pattern = "^0[xX][0-9A-Fa-f]{4}$";
	regex_t regex;
	int ret = regcomp(&regex, pattern, REG_EXTENDED);
	if(ret != 0){
		regfree(&regex);
		return 0;
	}	

	ret = regexec(&regex, str, 0, NULL, 0);
	regfree(&regex);
	return 1-ret;
}

// Takes a pointer for a cJSON entity and a size_t
// and returns a pointer of a question_t array, strings are copied via strcpy
// base 16 values are caster via strtol. 
question_t* question_array_allocator(cJSON* question_json, size_t size){
	question_t *questions = malloc(sizeof(question_t) * size);
	if(!questions || !cJSON_IsArray(question_json)) return NULL;		

	for(int i = 0; i < size; i++){
		cJSON *entry = cJSON_GetArrayItem(question_json, i);
		if(!cJSON_IsObject(entry)) return NULL;

		cJSON *qst = cJSON_GetObjectItem(entry, "question");
		cJSON *ans = cJSON_GetObjectItem(entry, "answer");
		cJSON *msr = cJSON_GetObjectItem(entry, "miss_rate");
		cJSON *cat = cJSON_GetObjectItem(entry, "category");
		if(!qst || !ans || !msr || !cat) return NULL;
		if(!cJSON_IsString(qst) || !cJSON_IsString(ans) || 
				!cJSON_IsNumber(msr) || !cJSON_IsString(cat)) return NULL;

		questions[i].question = malloc(strlen(qst->valuestring) + 1);
		questions[i].answer = malloc(strlen(ans->valuestring) + 1);
		if(!questions[i].question || !questions[i].answer || !str_is_4dig_hex(cat->valuestring)) 
			return NULL;	

		strcpy(questions[i].question, qst->valuestring);
		strcpy(questions[i].answer, ans->valuestring);
		questions[i].miss_rate = msr->valueint;
		questions[i].category = (uint16_t) strtol(cat->valuestring, NULL, 16); // could use endptr to enfore more safety on the cast lengt
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
		cJSON_Delete(question_json);
		fprintf(stderr, "Error while creating question_t array\n");
		exit(EXIT_FAILURE);
	}
	cJSON_Delete(question_json);

	heap_t *questions_heap = heap_t_creator(questions, (size_t)questions_number);
	if(questions_heap == NULL){
		heap_t_dealloc(questions_heap);
		printf("Couldn't allocate memory for questions\n");
	}


	heap_t_dealloc(questions_heap);
	return 0;
}
