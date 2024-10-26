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

typedef union jsonVarUnion{
	char* string;
	float number;
	struct jsonNode* object;
	struct jsonArray array;
	bool jsonBool;
	void* jsonNull;
}jsonVarUnion;

typedef struct jsonArrayNode{
	union jsonVarUnion variable;
	jsonVarType varType;
}jsonArrayNode;

typedef struct jsonArrayListNode{
	union jsonVarUnion variable;
	jsonVarType varType;
	struct jsonArrayListNode* next;
}jsonArrayListNode;

typedef struct jsonNode{
	char* key;
	unsigned long size;
	jsonVarUnion variable;
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
void parseJsonArray(FILE* file, char* charBuffer, jsonArray* node);
jsonNode* newJsonNode(const char* key);

/*print functions*/
void printArray(jsonVarUnion var, unsigned int level);
void printJson(jsonNode* obj, unsigned int level);
void printJsonNode(jsonNode* obj, unsigned int level);

/*search functions*/
jsonNode* findFirstSibling(const char* key, jsonNode* node);
jsonNode* findFirstDescendant(const char* key, jsonNode* node);

/*deletion functions*/
void deleteJsonArray(jsonVarUnion* var);
void deleteJsonObject(jsonNode* obj);

/*utility functions*/
jsonArrayListNode* newJsonArrayListNode(jsonVarUnion var);
void deleteJsonArrayList(jsonArrayListNode* node);
jsonCharListNode* newCharListNode(char c);
void deleteCharList(jsonCharListNode* node);

/*FUNCTION DEFINITIONS*/
jsonNode* parseJsonObject(FILE* file, char* charBuffer){
	jsonNode* node = NULL;
	jsonNode** current = &node;
	char buffer[128];
	unsigned int index;
	while(*charBuffer != '}'){

		*charBuffer = fgetc(file);

		if(*charBuffer == '"'){

			*current = malloc(sizeof(jsonNode));
			(*current)->sibling = NULL;
			if((*current) == NULL){
				printf("<How the fuck did it not allocate>\n");
			}
			index = 0;
			*charBuffer = fgetc(file);
			while(*charBuffer != '"'){

				buffer[index] = *charBuffer;
				*charBuffer = fgetc(file);
				index++;

			}

			buffer[index] = '\0';
			(*current)->key = malloc(sizeof(char) * (index + 1));
			strcpy((*current)->key, buffer);
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
		(*node)->variable.number = temp;
	}else{
		*charBuffer = fgetc(file);
		switch(*charBuffer){
			/*boolean cases*/
			case 't':
				(*node)->varType = jsonBool;
				(*node)->variable.jsonBool = true;
				break;
			case 'f':
				(*node)->varType = jsonBool;
				(*node)->variable.jsonBool = false;
				break;
			/*null case*/
			case 'n':
				(*node)->varType = jsonNull;
				(*node)->variable.jsonNull = NULL;
				break;
			/*string*/
			case '"':
				*charBuffer = fgetc(file);
				(*node)->varType = string;
				index = 0;
				while(*charBuffer != '"'){
					buffer[index] = *charBuffer;
					*charBuffer = fgetc(file);
					index++;
				}
				buffer[index] = '\0';
				(*node)->variable.string = malloc(sizeof(char)*(index+1));

				strcpy((*node)->variable.string, buffer);
				if(*charBuffer == '"')*charBuffer = fgetc(file);
				break;
			/*array*/
			case '[':
				(*node)->varType = array;
				parseJsonArray(file, charBuffer, &((*node)->variable.array));
				while(*charBuffer != ']')*charBuffer = fgetc(file);
				if(*charBuffer == ']')*charBuffer = fgetc(file);
				break;
			/*object*/
			case '{':
				(*node)->varType = object;
				(*node)->variable.object = parseJsonObject(file, charBuffer);
				if(*charBuffer == '}')*charBuffer = fgetc(file);
				break;
		}
	}
	
	while(*charBuffer != ',' && *charBuffer != '}' && *charBuffer != ']'){
		*charBuffer = fgetc(file);
	}
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
			(*current)->variable.number = temp;
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
					(*current)->variable.jsonBool = true;
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
				case 'f':
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = jsonBool;
					(*current)->variable.jsonBool = false;
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
				/*null case*/
				case 'n':
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = jsonNull;
					(*current)->variable.jsonNull = NULL;
					(*current)->next = NULL;
					current = &(*current)->next;
					size++;
					break;
				/*string*/
				case '"':
					*charBuffer = fgetc(file);
					(*current) = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = string;
					index = 0;
					while(*charBuffer != '"'){
						buffer[index] = *charBuffer;
						*charBuffer = fgetc(file);
						index++;
					}
					(*current)->variable.string = malloc(sizeof(char)*(index+1));
					buffer[index] = '\0';
					strcpy((*current)->variable.string, buffer);
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
					parseJsonArray(file, charBuffer, (&((*current)->variable.array)));
					if(*charBuffer == ']')*charBuffer = fgetc(file);
					current = &(*current)->next;
					size++;
					break;
				/*object*/
				case '{':
					*current = malloc(sizeof(jsonArrayListNode));
					(*current)->varType = object;
					(*current)->variable.object = parseJsonObject(file, charBuffer);
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
		node->node[index].variable = (*current)->variable;
		node->node[index].varType = (*current)->varType;
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
void printArray(jsonVarUnion var, unsigned int level){
	unsigned long i;
	printf("[");
	for(i = 0; i < var.array.size - 1; i++){
		switch((var.array.node[i]).varType){
			case string:
				printf("\"%s\"", var.array.node[i].variable.string);
				break;
			case number:
				printf("%f", var.array.node[i].variable.number);
				break;
			case object:
				printf("\n");
				printJson(var.array.node[i].variable.object, level + 1);
				break;
			case array:
				printArray(var.array.node[i].variable, level);
				break;
			case jsonBool:
				printf("%s", (var.array.node[i].variable.jsonBool)?"true":"false");
				break;
			case jsonNull:
				printf("null");
				break;
		}
		printf(", ");
	}
	if(var.array.size != 0){
		switch((var.array.node[i]).varType){
			case string:
				printf("%s", var.array.node[i].variable.string);
				break;
			case number:
				printf("%f", var.array.node[i].variable.number);
				break;
			case object:
				printf("\n");
				printJson(var.array.node[i].variable.object, level + 1);
				break;
			case array:
				printArray(var.array.node[i].variable, level);
				break;
			case jsonBool:
				printf("%s", (var.array.node[i].variable.jsonBool)?"true":"false");
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
				printf("\"%s\"", current->variable.string);
				break;
			case number:
				printf("%f", current->variable.number);
				break;
			case object:
				printJson(current->variable.object, level + 1);
				break;
			case array:
				printArray(current->variable, level);
				break;
			case jsonBool:
				printf("%s", (current->variable.jsonBool)?"true":"false");
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
				printf("\"%s\"", obj->variable.string);
				break;
			case number:
				printf("%f", obj->variable.number);
				break;
			case object:
				printJson(obj->variable.object, level + 1);
				break;
			case array:
				printArray(obj->variable, level);
				break;
			case jsonBool:
				printf("%s", (obj->variable.jsonBool)?"true":"false");
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
						(*tail)->node = head->node->variable.object;
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
void deleteJsonArray(jsonVarUnion* var){
	unsigned int i;
	for(i = 0; i < var->array.size; i++){
		switch((var->array.node[i]).varType){
			case string:
				free(var->array.node[i].variable.string);
				break;
			case object:
				deleteJsonObject(var->array.node[i].variable.object);
				break;
			case array:
				deleteJsonArray(&(var->array.node[i].variable));
				break;
		}
	}
	free(var->array.node);
}

void deleteJsonObject(jsonNode* obj){
	if(obj->sibling != NULL)deleteJsonObject(obj->sibling);
	switch(obj->varType){
		case string:
			free(obj->variable.string);
			break;
		case object:
			deleteJsonObject(obj->variable.object);
			break;
		case array:
			deleteJsonArray(&(obj->variable));
			break;
	}
	free(obj->key);
	free(obj);
}

/*ARRAY LIST NODES FUNCTIONS*/
jsonArrayListNode* newJsonArrayListNode(jsonVarUnion var){
	jsonArrayListNode* node = malloc(sizeof(jsonArrayListNode));
	node->variable = var;
	node->next = NULL;
	return node;
}

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
