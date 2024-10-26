#ifndef JSON_PARSER_H
#define JSON_PARSER_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*TYPE DEFENITIONS*/
typedef enum jsonVarType{string, number, object, array, jsonBool, jsonNull}jsonVarType;

//typedef enum bool{false, true}bool;

typedef struct jsonCharListNode{
	char c;
	struct jsonCharListNode* next;
}jsonCharListNode;

typedef struct jsonArray{
	struct jsonArrayNode* node;
	unsigned long size;
}jsonArray;

typedef struct jsonArrayNode{
	union{
		char* string;
		float number;
		struct jsonNode* object;
		struct jsonArray array;
		bool jsonBool;
		void* jsonNull;
	};
	jsonVarType varType;
}jsonArrayNode;

typedef struct jsonArrayListNode{
	union{
		char* string;
		float number;
		struct jsonNode* object;
		struct jsonArray array;
		bool jsonBool;
		void* jsonNull;
	};
	jsonVarType varType;
	struct jsonArrayListNode* next;
}jsonArrayListNode;

typedef struct jsonNode{
	char* key;
	unsigned long size;
	union{
		char* string;
		float number;
		struct jsonNode* object;
		struct jsonArray array;
		bool jsonBool;
		void* jsonNull;
	};
	jsonVarType varType;
	struct jsonNode* sibling;
}jsonNode;

typedef struct jsonNodeListNode{
	jsonNode* node;
	struct jsonNodeListNode* next;
}jsonNodeListNode;

/*FUNCTION DECLARATIONS*/

/*parsing functions*/
jsonNode* parseJsonObject(FILE* file, char* charBuffer);
void parseJsonVar(FILE* file, char* charBuffer, jsonNode** node);
char* parseJsonString(FILE* file, char* charBuffer);
void parseJsonArray(FILE* file, char* charBuffer, jsonArray* node);
jsonNode* newJsonNode(const char* key);

/*print functions*/
void printArrayFromArray(jsonArray* node, unsigned int level);
void printArray(jsonNode* node, unsigned int level);
void printJson(jsonNode* obj, unsigned int level);
void printJsonNode(jsonNode* obj, unsigned int level);

/*search functions*/
jsonNode* findFirstSibling(const char* key, jsonNode* node);
jsonNode* findFirstDescendant(const char* key, jsonNode* node);

/*deletion functions*/
void deleteJsonArray(jsonArray* var);
void deleteJsonObject(jsonNode* obj);

/*utility functions*/
void deleteJsonArrayList(jsonArrayListNode* node);
jsonCharListNode* newCharListNode(char c);
void deleteCharList(jsonCharListNode* node);

/*FUNCTION DEFINITIONS*/
/*parsing functions*/
jsonNode* parseJsonObject(FILE* file, char* charBuffer){
	jsonNode* node = NULL;
	jsonNode** current = &node;
	while(*charBuffer != '}'){

		*charBuffer = fgetc(file);

		if(*charBuffer == '"'){

			*current = malloc(sizeof(jsonNode));
			(*current)->sibling = NULL;
			if((*current) == NULL){
				printf("<How the fuck did it not allocate>\n");
			}
			(*current)->key = parseJsonString(file, charBuffer);
			while(*charBuffer != ':')*charBuffer = fgetc(file);
			parseJsonVar(file, charBuffer, current);

			current = &(*current)->sibling;
		}
	}
	return node;
}

void parseJsonVar(FILE* file, char* charBuffer, jsonNode** node){
	float temp;
	char* ctemp = NULL;
	unsigned int index;
	char buffer[128];
	if(fscanf(file, "%f", &temp)){
		(*node)->varType = number;
		(*node)->number = temp;
	}else{
		*charBuffer = fgetc(file);
		switch(*charBuffer){
			/*boolean cases*/
			case 't':
				(*node)->varType = jsonBool;
				(*node)->jsonBool = true;
				break;
			case 'f':
				(*node)->varType = jsonBool;
				(*node)->jsonBool = false;
				break;
			/*null case*/
			case 'n':
				(*node)->varType = jsonNull;
				(*node)->jsonNull = NULL;
				break;
			/*string*/
			case '"':
				(*node)->varType = string;
				(*node)->string = parseJsonString(file, charBuffer);
				if(*charBuffer == '"')*charBuffer = fgetc(file);
				break;
			/*array*/
			case '[':
				(*node)->varType = array;
				parseJsonArray(file, charBuffer, &((*node)->array));
				while(*charBuffer != ']')*charBuffer = fgetc(file);
				if(*charBuffer == ']')*charBuffer = fgetc(file);
				break;
			/*object*/
			case '{':
				(*node)->varType = object;
				(*node)->object = parseJsonObject(file, charBuffer);
				if(*charBuffer == '}')*charBuffer = fgetc(file);
				break;
		}
	}
	
	while(*charBuffer != ',' && *charBuffer != '}' && *charBuffer != ']'){
		*charBuffer = fgetc(file);
	}
}

char* parseJsonString(FILE* file, char* charBuffer){
	char* result = NULL;
	jsonCharListNode* cNode = NULL;
	jsonCharListNode** currentCNode = &cNode;
	size_t stringLength = 0;
	*charBuffer = fgetc(file);

	while(*charBuffer != '"'){
		if(*charBuffer == '\\'){
			*charBuffer = fgetc(file);
		}
		*currentCNode = newCharListNode(*charBuffer);
		currentCNode = &((*currentCNode)->next);
		*charBuffer = fgetc(file);
		stringLength++;
	}

	result = malloc(sizeof(char) * (stringLength + 1));
	currentCNode = &cNode;

	for(size_t i = 0; i < stringLength; i++){
		result[i] = (*currentCNode)->c;
		currentCNode = &((*currentCNode)->next);
	}

	result[stringLength] = '\0';
	deleteCharList(cNode);
	return result;
}

void parseJsonArray(FILE* file, char* charBuffer, jsonArray* node){
	unsigned long size = 0;
	float temp;
	unsigned int index;
	char buffer[128];
	jsonArrayListNode* n = NULL;
	jsonArrayListNode** current = &n;
	while(*charBuffer != ']'){
		if(fscanf(file, "%f", &temp)){
			*current = malloc(sizeof(jsonArrayListNode));
			(*current)->varType = number;
			(*current)->number = temp;
			(*current)->next = NULL;
			current = &(*current)->next;
			size++;
		}else{
			*charBuffer = fgetc(file);
			switch(*charBuffer){
				/*boolean cases*/
				case 't':
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = jsonBool;
					(*current)->jsonBool = true;
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
				case 'f':
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = jsonBool;
					(*current)->jsonBool = false;
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
				/*null case*/
				case 'n':
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = jsonNull;
					(*current)->jsonNull = NULL;
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
				/*string*/
				case '"':
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = string;
					(*current)->string = parseJsonString(file, charBuffer);
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					if(*charBuffer == '"')*charBuffer = fgetc(file);
					break;
				/*array*/
				case '[':
					*current = malloc(sizeof(jsonArrayListNode));
					(*current)->next = NULL;
					(*current)->varType = array;
					parseJsonArray(file, charBuffer, (&((*current)->array)));
					if(*charBuffer == ']')*charBuffer = fgetc(file);
					current = &(*current)->next;
					size++;
					break;
				/*object*/
				case '{':
					*current = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = object;
					(*current)->object = parseJsonObject(file, charBuffer);
					if(*charBuffer == '}')*charBuffer = fgetc(file);
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
			}
		}
		while(*charBuffer != ',' && *charBuffer != '}' && *charBuffer != ']'){
			*charBuffer = fgetc(file);
		} 
	}
	current = &n;
	node->node = malloc(sizeof(jsonArrayNode) * size);
	node->size = size;
	index = 0;
	for(index = 0; index < size; index++){
		switch((*current)->varType){
			case string:
				node->node[index].string = (*current)->string;
				node->node[index].varType = string;
				break;
			case number:
				node->node[index].number = (*current)->number;
				node->node[index].varType = number;
				break;
			case object:
				node->node[index].object = (*current)->object;
				node->node[index].varType = object;
				break;
			case array:
				node->node[index].array = (*current)->array;
				node->node[index].varType = array;
				break;
			case jsonBool:
				node->node[index].jsonBool = (*current)->jsonBool;
				node->node[index].varType = jsonBool;
				break;
			case jsonNull:
				node->node[index].jsonNull = (*current)->jsonNull;
				node->node[index].varType = jsonNull;
				break;
		}
		
		current = &(*current)->next;
	}
	deleteJsonArrayList(n);
	
}

jsonNode* newJsonNode(const char* key){
	jsonNode* result = malloc(sizeof(jsonNode));
	unsigned long size = strlen(key) + 1;
	result->key = malloc(size * sizeof(char));
	for(unsigned long i = 0; i < size - 1; i++){
		result->key[i] = key[i];
	}
	result->key[size-1] = '\0';
	return result;
}

/*PRINT FUNCTIONS*/
void printArrayFromArray(jsonArray* node, unsigned int level){
	unsigned long i;
	printf("[");
	for(i = 0; i < node->size - 1; i++){
		switch((node->node[i]).varType){
			case string:
				printf("\"%s\"", node->node[i].string);
				break;
			case number:
				printf("%f", node->node[i].number);
				break;
			case object:
				printf("\n");
				printJson(node->node[i].object, level + 1);
				break;
			case array:
				printArrayFromArray(&node->node[i].array, level);
				break;
			case jsonBool:
				printf("%s", (node->node[i].jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
		}
		printf(", ");
	}
	if(node->size != 0){
		switch((node->node[i]).varType){
			case string:
				printf("%s", node->node[i].string);
				break;
			case number:
				printf("%f", node->node[i].number);
				break;
			case object:
				printf("\n");
				printJson(node->node[i].object, level + 1);
				break;
			case array:
				printArrayFromArray(&node->node[i].array, level);
				break;
			case jsonBool:
				printf("%s", (node->node[i].jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
		}
	}
	printf("]");
}

void printArray(jsonNode* node, unsigned int level){
	unsigned long i;
	printf("[");
	for(i = 0; i < node->array.size - 1; i++){
		switch(node->array.node[i].varType){
			case string:
				printf("\"%s\"", node->array.node[i].string);
				break;
			case number:
				printf("%f", node->array.node[i].number);
				break;
			case object:
				printf("\n");
				printJson(node->array.node[i].object, level + 1);
				break;
			case array:
				printArrayFromArray(&node->array.node[i].array, level);
				break;
			case jsonBool:
				printf("%s", (node->array.node[i].jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
		}
		printf(", ");
	}
	if(node->array.size != 0){
		switch((node->array.node[i]).varType){
			case string:
				printf("%s", node->array.node[i].string);
				break;
			case number:
				printf("%f", node->array.node[i].number);
				break;
			case object:
				printf("\n");
				printJson(node->array.node[i].object, level + 1);
				break;
			case array:
				printArrayFromArray(&node->array.node[i].array, level);
				break;
			case jsonBool:
				printf("%s", (node->array.node[i].jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
		}
	}
	printf("]");
}

void printJson(jsonNode* obj, unsigned int level){
	jsonNode* current = obj;
	unsigned int i;
	for(i = 0; i < level; i++){
		printf("\t");
	}
	printf("{\n");
	if(current == NULL)printf("hey the thing is NULL\n");;
	while(current != NULL){
		for(i = 0; i <= level; i++){
			printf("\t");
		}
		printf("%s : ", current->key);
		switch(current->varType){
			case string:
				printf("\"%s\"", current->string);
				break;
			case number:
				printf("%f", current->number);
				break;
			case object:
				printJson(current->object, level + 1);
				break;
			case array:
				printArray(current, level);
				break;
			case jsonBool:
				printf("%s", (current->jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
			
		}
		current = current->sibling;
		if(current != NULL)printf(",");
		printf("\n");
	}
	for(i = 0; i < level; i++){
		printf("\t");
	}
	printf("}\n");
}

void printJsonNode(jsonNode* obj, unsigned int level){
	unsigned int i;
	for(i = 0; i < level; i++){
		printf("\t");
	}
	printf("{\n");
	if(obj == NULL)printf("hey the thing is NULL\n");
	else{
		for(i = 0; i <= level; i++){
			printf("\t");
		}
		printf("%s : ", obj->key);
		switch(obj->varType){
			case string:
				printf("\"%s\"", obj->string);
				break;
			case number:
				printf("%f", obj->number);
				break;
			case object:
				printJson(obj->object, level + 1);
				break;
			case array:
				printArray(obj, level);
				break;
			case jsonBool:
				printf("%s", (obj->jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
			
		}
		printf("\n");
	}
	for(i = 0; i < level; i++){
		printf("\t");
	}
	printf("}\n");
}

/*SEARCH FUNCTIONS*/
jsonNode* findFirstSibling(const char* key, jsonNode* node){
	jsonNode* current = node;
	jsonNode* result = NULL;
	while(current != NULL){
		if(strcmp(current->key, key) == 0){
			result = current;
			break;
		}else{
			current = current->sibling;
		}
	}
	return result;
}

jsonNode* findFirstDescendant(const char* key, jsonNode* node){
	jsonNodeListNode* head = malloc(sizeof(jsonNodeListNode));
	jsonNodeListNode** tail = NULL;
	jsonNode* result = NULL;
	head->next = NULL;
	head->node = node;
	tail = &head;
	if(head->node != NULL){
		while(head != NULL){
			if(head->node != NULL){
				if(strcmp(head->node->key, key) == 0){
					result = head->node;
					break;
				}else{
					if(head->node->varType == object){
						(*tail)->next = malloc(sizeof(jsonNodeListNode));
						tail = &((*tail)->next);
						(*tail)->next = NULL;
						(*tail)->node = head->node->object;
					}
					head->node = head->node->sibling;
					if(head->node == NULL){
						jsonNodeListNode* temp = head->next;
						free(head);
						head = temp;
					}
				}
			}
		}
		while(head != NULL){
			jsonNodeListNode* temp = head->next;
			free(head);
			head = temp;
		}
	}else{
		free(head);
	}
	return result;
}

/*DELETION FUNCTIONS*/
void deleteJsonArray(jsonArray* var){
	unsigned int i;
	for(i = 0; i < var->size; i++){
		switch((var->node[i]).varType){
			case string:
				free(var->node[i].string);
				break;
			case object:
				deleteJsonObject(var->node[i].object);
				break;
			case array:
				deleteJsonArray(&(var->node[i].array));
				break;
		}
	}
	free(var->node);
}

void deleteJsonObject(jsonNode* obj){
	if(obj->sibling != NULL)deleteJsonObject(obj->sibling);
	switch(obj->varType){
		case string:
			free(obj->string);
			break;
		case object:
			deleteJsonObject(obj->object);
			break;
		case array:
			deleteJsonArray(&(obj->array));
			break;
	}
	free(obj->key);
	free(obj);
}

/*ARRAY LIST NODES FUNCTIONS*/
void deleteJsonArrayList(jsonArrayListNode* node){
	if(node->next != NULL)deleteJsonArrayList(node->next);
	free(node);
}

/*CHARACTER LIST NODES FUNCTIONS*/
jsonCharListNode* newCharListNode(char c){
	jsonCharListNode* node = malloc(sizeof(jsonCharListNode));
	node->c = c;
	node->next = NULL;
	return node;
}

void deleteCharList(jsonCharListNode* node){
	if(node->next != NULL){
		deleteCharList(node->next);
	}
	free(node);
}
#endif
