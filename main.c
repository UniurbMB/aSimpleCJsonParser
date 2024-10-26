#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "json.h"

int main(int argc, char* argv[]){
	char buffer[128];
	char charBuffer;
	int index = 0;
	FILE* file;
	clock_t t;
	if(argc >= 2){

		file = fopen(argv[1], "r");

		if(file != NULL){
			
			t = clock();
			jsonNode* node = parseJsonObject(file, &charBuffer);

			if(node == NULL)printf("failed to actually store the thing, god fucking damn it\n");
			t = clock() - t;
			printf("Read the file in: %f seconds and %ld clicks\n", ((float)t)/CLOCKS_PER_SEC, t);

			//jsonNode* likes = findFirstSibling("likes", node);			
			//if(likes != NULL)printf("Got likes\n");;
			//printf("%s, %f\n", likes->array.node[0].string, likes->array.node[1].number);

			printJson(node, 0);
			deleteJsonObject(node);
			
			fclose(file);
		}else{
			printf("Could not open %s!\n", argv[1]);
		}
	}else printf("Please pass a filename in commandline arguments\n");
/*
	printf("\n--------\ntest dump\n---------\n");
	file = fopen("test.gltf", "r");
	while(!feof(file)){
		printf("%c", fgetc(file));
	}
	fclose(file);
*/
	return 0;
}

/*
void parseJsonObject(FILE* file, char* charBuffer){
	char buffer[128];
	unsigned int index;
	printf("{\n");
	while(*charBuffer != '}'){
		*charBuffer = fgetc(file);
		if(*charBuffer == '"'){
			index = 0;
			*charBuffer = fgetc(file);
			while(*charBuffer != '"'){
				buffer[index] = *charBuffer;
				*charBuffer = fgetc(file);
				index++;
			}
			buffer[index] = '\0';
			printf("\"%s\" :", buffer);
			while(*charBuffer != ':')*charBuffer = fgetc(file);
			parseJsonVar(file, charBuffer);
		}
	}
	printf("\n}\n");
}

void parseJsonVar(FILE* file, char* charBuffer){
	float temp;
	if(fscanf(file, "%f", &temp)){
		printf("%f\n", temp);
		//while(*charBuffer != ',' && *charBuffer != '}')*charBuffer = fgetc(file);
	}else{
		*charBuffer = fgetc(file);
		if(*charBuffer == 't'){
			printf("true\n");
			//while(charBuffer != ',' && charBuffer != '}')charBuffer = fgetc(file);
		}
		else if(*charBuffer == 'f'){
			printf("false\n");
			//while(charBuffer != ',' && charBuffer != '}')charBuffer = fgetc(file);
		}else if(*charBuffer == 'n'){
			printf("null\n");
			//while(charBuffer != ',' && charBuffer != '}')charBuffer = fgetc(file);
		}else if(*charBuffer == '"'){
			*charBuffer = fgetc(file);
			printf("\"");
			while(*charBuffer != '"'){
				printf("%c", *charBuffer);
				*charBuffer = fgetc(file);
			}
			//while(charBuffer != ',' && charBuffer != '}')charBuffer = fgetc(file);
			printf("\"\n");
			//charBuffer = fgetc(file);
		}else if(*charBuffer == '['){
			// *charBuffer = fgetc(file);
			printf("[");
			while(*charBuffer != ']'){
			//	printf("%c", charBuffer);
			//	*charBuffer = fgetc(file);
				parseJsonVar(file, charBuffer);
			}
			if(*charBuffer == ']')*charBuffer = fgetc(file);
			//while(charBuffer != ',' && charBuffer != '}')charBuffer = fgetc(file);
			printf("]\n");
		}else if(*charBuffer == '{'){
				parseJsonObject(file, charBuffer);
				if(*charBuffer == '}')*charBuffer = fgetc(file);
			}
	}
	while(*charBuffer != ',' && *charBuffer != '}' && *charBuffer != ']'){
		//printf("%c", charBuffer);
		*charBuffer = fgetc(file);
	}
}
*/
