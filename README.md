# A simple C JSON parser
## About
This is a simple JSON parser written in C for C projects.
## Features
- Reading JSON files
- Writing JSON files
- Searching within JSON files (or objects already loaded to be precise)
- modifying JSON objects
- basic character escaping
## Features that are not included
- Creating JSON nodes (you have to do that by hand)
- Verifying JSON files
- Beautifying JSON output ~~At least the output is functional though~~
- unicode support
- emoji support
## Usage

> [!IMPORTANT]
> The way JSON files are represented in this parser is a linked list, where each object is a reference to the first pair of <"key" : value> and any subsequent entry in a JSON file are refered to as "siblings"

To explain how json is stored with this parser let's look at this example: 

```json
{
	"first key" : "The next key will contain a number",
	"second key" : 200,
	"third key, which is an object" : {
		"key in the object" : ["that is an array", 125]
	},
	"final key" : null
}
```

The following example would be represented as something like this:

```
("first key" : (string value))->("second key" : 200)->("third key": |)->("final key" : NULL)->NULL
                                                                    |
                                                                    v
                                                   ("key in the object":(array variable))->NULL
```

~~I know this looks terrible, please cut me some slack~~
Whenever you load a JSON file (with the example shown [here](#loading-json-files)), you get the first element, which in this case is the node with the key "first key" and a string variable. To get to the "third key", we need to go through the "second key", like this:

```c

	//Some code before, where you load the JSON file and store it in, let's say, rootNode

	jsonNode* thirdKeyJsonNode = rootNode->sibling->sibling;

	//Whatever it is you need to do with this node

```

### Loading JSON files
To load a JSON file, you must supply the parseJsonObject function with a file pointer and a character pointer, like this:

```c
char charBuffer;
FILE* file = fopen("yourJsonFile.json", "r");
jsonNode* rootJsonNode = parseJsonObject(file, &charBuffer);
fclose(file);

//Whatever that you want to do here

deleteJsonObject(rootJsonNode); //We're C programmers, we must clean up after ourselves, unlike :vomiting_face: Java

//The rest of your program

```

### Getting the variable of a node
To get the enum kind of variable is stored in the node, you can use the following code!

```c
switch(myJsonNode->varType){
	case json_string:
		//do whatever it is you want with a string by referencing it as myJsonNode->string
		break;
	case json_number:
		//do whatever it is you want with a number, which is stored as a float, by referencing it as myJsonNode->number
		break;
	case json_object:
		//do whatever it is you want with an object, which has the type jsonNode*, by referencing it as myJsonNode->object
		break;
	case json_array:
		//arrays are a bit peculiar, in that they're handled like jsonNode, but without a key
		//as in if we have an array of numbers, we can reference the first one by using myJsonNode->array.node[0].number
		//the size of the array can be checked via myJsonNode->array.size
		//and of course, each element has its own type, which can be read with myJsonNode->array.node[i].varType
		break;
	case json_bool:
		//do whatever it is you want with a bool, which is of type bool (duh!), by referencing it as myJsonNode->bool_
		break;
	case json_null;
		//I don't really know what you would do with this, but it's here anyway
		//This has the type of void* assigned with a NULL value and can be accessed with myJsonNode->null
		break;
}
```

All of the possible variable types can be found in the jsonVarType enum at the top of the header file, but for convenience's sake, they'll be listed below:

- json_string
- json_number
- json_object
- json_array
- json_bool
- json_null

And of course the list of references to the variable itself, which are referenced as myJsonNode.(the variable you want to access (without parentheses, of course)):

- string
- number
- object
- array (where to reference the variables in the array, you do myJsonNode->array.node[i].(same variable naming convention applies here))
- bool_ (**pay attention to __the underscore__ at the end!**)
- null (**all lowercase**)

### Searching in the JSON file

There are two functions at your disposal: findFirstSibling and findFirstDescendant. Both take in two parametres: a string representing the key to be searched and the node from which you want to search.
Before we can look at how they function, let's make an example JSON file in which we will search:
```json
{
	"first key" : "I'm technically the root node",
	"second key" : {
		"interesting key" : "Woah! I'm part of an object!",
		"number key" : 42,

		"boring key" : "This is the one you're looking for",
		"nasty key" : [{"you will never find this one" : true}, 123, "abc"]
	},
	"third key" : "cool",
	"fourth key" : null
}
```
Both functions have jsonNode\* as their return values. However, if you try to search for a value that is not in the scope of the function or is not present in the file, then the functions will return NULL instead.
Let's look at an example of findFirstSibling:

```c
//We loaded the json before into a rootJsonNode
jsonNode* thirdKeyNode = findFirstSibling("third key", rootJsonNode); //Will find the node
jsonNode* boringKeyNode = findFirstSibling("boring key", rootJsonNode); //Will not find, as this is a descendant node
jsonNode* impossibleNode = findFirstSibling("you will never find this one", rootJsonNode); //This parser is incapable of searching in arrays, sorry
jsonNode* secondKeyNode = findFirstSibling("second key", rootJsonNode); //Pretty trivial at this point
jsonNode* firstKeyNodeFromSecond = findFirstSibling("first key", secondKeyNode); //Although "first key" is present, this will not find it, as it appears before the second key
```
~~It's pretty bad, I know~~

Now, let's look at findFirstDescendant instead:
```c
//We loaded the json before into a rootJsonNode
jsonNode* thirdKeyNode = findFirstDescendant("third key", rootJsonNode); //Same as findFirstSibling
jsonNode* boringKeyNode = findFirstDescendant("boring key", rootJsonNode); //Will find, as this is a descendant node
jsonNode* impossibleNode = findFirstDescendant("you will never find this one", rootJsonNode); //This parser is incapable of searching in arrays, sorry
jsonNode* secondKeyNode = findFirstDescendant("second key", rootJsonNode); //Pretty trivial at this point
jsonNode* firstKeyNodeFromSecond = findFirstSibling("first key", secondKeyNode); //Although "first key" is present, this will not find it, as it appears before the second key, same as findFirstSibling
```
> [!WARNING]
> findFirstDescendant uses a BFS algorithm, so it will search everything that's on the same level (or in the same object) first, before going to the next level of objects

### Creating/Modifying JSON objects

> [!CAUTION]
> This parser does **NOT** include any functions to create or modify nodes, and as such they have to be done __by hand__. Below is an example of how it would be done.

```c
//Create the root node
jsonNode* myRootNode = malloc(sizeof(jsonNode));

//Assign it a string variable
myRootNode->varType = string;
myRootNode->string = "Evil incarnate\0";

//give the root node a sibling
myRootNode->sibling = malloc(sizeof(jsonNode));

//assign the sibling node a number variable
myRootNode->sibling->varType = number;
myRootNode->sibling->number = 42;

//end the json object
myRootNode->sibling->sibling = NULL;


//Be sure to deleteJsonObject(myRootNode) after you're done working with this!
```

### Outputting JSON objects

To output a json object to the terminal window, you can use the printJson function, which takes in two parametres: json node from which we'll start printing and the indentation level (you should just use 0)
Example:
```c
//Let's say we suffered and made a file in the code, which starts at rootJsonNode

printJson(rootJsonNode, 0);

//The rest of the program
```

As for outputting to a file, we can use the printToFileJson function, which also takes in a file pointer as its first argument
Example:
```c
//Create jsonObject with root at rootJsonNode

//First, you must open the file for writing (not appending!)
FILE* file = fopen(".pathToYourFile/yourFileName.json", "w");

//Output the object
printToFileJson(file, rootJsonNode, 0);

//Close the file
fclose(file);

```

> [!NOTE]
> While the output will be functional, I can hardly gurantee that it will be pretty (that is if you mix objects and arrays, otherwise it's pretty ok)
