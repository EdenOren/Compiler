/*
 * This source file create data structures used in this program
 * getting and setting using various functions
 * Symbol Table, Instructions Table, Address & Code Table
 */

#include "data_structure.h"
#include "conversions.h"

/*
 * Function:  add_symbol 
 * --------------------
 * Adds a new symbol to symbol table
 * head: symbol table header
 * token: symbol name
 * insert_address: initial address of symbol
 */
void add_symbol(symbol** head, const char * token, unsigned long insert_address) {
    symbol * new_sym = (symbol*)malloc(sizeof(symbol));
    symbol * temp = *head;
    int i = 0;

    if(new_sym == NULL) {
        print_fundamental_error(NO_AVAILABLE_MEMORY, NULL);
        exit(1);
    }
    /*set new symbol values*/
    new_sym -> address = insert_address; 
    strcpy(new_sym -> label, token);
    new_sym -> next = NULL;

    for (; i < ATTRIBUTE_NUM; i++)
        new_sym -> attrib[i] = 0;

    if (*head == NULL) /*if empty table, new sym is the head*/
    { 
        *head = new_sym;
        return;
    }
    while (temp -> next != NULL) /*find the last symbol */
        temp = temp -> next;
    temp -> next = new_sym; /*connect last previous symbol to the new symbol */
} /* end of add_symbol */

/*
 * Function:  set_symbol 
 * --------------------
 * set existing symbol address or attribute
 * head: symbol table header
 * token: symbol name
 * value: new address or attribute value
 * type: determines set value to address or attribute
 */
void set_symbol(symbol** head, const char * token, unsigned long value, enum SYMBOL_SET_TYPE type) {
    symbol *temp = *head;
    for(;temp != NULL; temp = temp -> next) { 
        if (!strcmp(temp -> label, token))
        {
            if (type == SET_ADDRESS)
            {
                temp -> address = value;
            }
            if (type == SET_ATTRIBUTE)
            {
                temp -> attrib[value] = 1;
            }
        }
    } /* end for */
} /*end of set_symbol */

/*
 * Function:  get_symbol 
 * --------------------
 * find existing symbol by name
 * head: symbol table header
 * token: symbol name to find
 * returns: pointer to found symbol, null if doesnt exist
 */
symbol * get_symbol(symbol** head, const char * token) {
    symbol *temp = *head;

    while (temp != NULL && (strcmp(temp -> label, token)) ) 
    {
        temp = temp -> next;
    }
    return temp;
} /*end of get symbol */

/*
 * Function:  delete_symbol_table 
 * --------------------
 * delete symbol table
 * head: symbol table header
 */
void delete_symbol_table(symbol ** head) {
  symbol *curr = *head, *temp = NULL;

    if (*head == NULL)
        return;
    curr = curr -> next;
    while (curr != NULL) 
    {
        temp = curr;
        curr = curr -> next;
        free(temp);
    }
    free(*head);
    *head = NULL; 
    head = NULL;
}/*end of delete symbol table */

/*
 * Function:  update_symbol_addresses 
 * --------------------
 * adds final instruction count value to data addresses at symbol table
 * head: symbol table header
 * icf: final instruction count
 */
void update_symbol_addresses(symbol** head, const int icf) {
    symbol *temp = *head;
    for(;temp != NULL; temp = temp -> next) 
    { 
        if (temp -> attrib[A_DATA])
            temp -> address += icf;
    }
} /* end of update symbol addresses*/

/*
 * Function:  create_instructions_table 
 * --------------------
 * returns: pointer to insturctions table header
 */
instruction * create_instructions_table () {
    char * ins_name[] = {INSTRUCTIONS_TABLE}; /*this array contains all instruction commands */
    instruction * new = (instruction*) malloc(sizeof(instruction)); /* allocate memory for new instruction */
    instruction * head;
    int count = 0, opcode = 0, funct = 1;
    
    if (new == NULL)
    {
        print_fundamental_error(NO_AVAILABLE_MEMORY, NULL);
        exit(1);
    }
    new -> value = 0;
    head = new;
    /*add instructions to table*/
    while (IS_NOT_STOP(ins_name[count])) {

        /*change opcode & funct according to instruction table
          once for each type of instruction */
        if (count == R_OP_0_COUNT)
        {
            opcode = 1; 
            funct = 1; 
        } else if (count == R_COUNT) {
            opcode = OPC_I;
        } else if (count == R_COUNT + I_COUNT) {
            opcode = OPC_J;  
        }
        /*set bits according to opcode & funct */

        strcpy(new -> label, ins_name[count]);
        if (count < R_COUNT) /*is R instruction*/
        {
            new -> value = opcode << OPCODE_LOCATION;
            new -> value |= (funct++) << FUNCT_LOCATION; /*set funct value and get next*/
            
        } else {/* is I or J instruction*/
            new -> value = (opcode++) << OPCODE_LOCATION; /*set opcode value and get next*/
        }
        new = new -> next = (instruction*) malloc(sizeof(instruction)); /*allocate memory for the next instruction */
        if (new == NULL) /*check memory allocation*/
            return NULL;
        count++;
    } /*end of while */

    /*add stop to table */
    opcode = OPC_STOP;
    strcpy(new -> label, ins_name[count]);
    new -> value = (opcode++) << OPCODE_LOCATION;
    new -> next = NULL;
    
    return head;
} /*end of create insturctions table */

/*
 * Function:  get_instruction 
 * --------------------
 * find instructions by name string
 * head: instruction table header
 * token: instruction name to find
 * returns: pointer to found instruction, null otherwise
 */
instruction * get_instruction (instruction * head, char * token) {
    instruction * current = head;
    if(head == NULL) {
        return NULL;
    }
    while (strcmp(current -> label, token)) 
    {
        if(current -> next == NULL) {
            return NULL;
        } else {
            current = current -> next;
        }
    }      
    return current;
} /* end of get instruction */

/*
 * Function:  delete_instructions_table 
 * --------------------
 * delete instructions table
 * head: instructions table header
 */
void delete_instructions_table (instruction * head) {
    instruction * curr = head;
    instruction * temp = NULL;
    if (head == NULL) /*no memory to free */
    {
        return;
    }
    while (curr != NULL)
    {
        temp = curr -> next;
        free(curr);
        curr = temp;
    }
    head = NULL;
} /*end of delete instructions table */

/*
 * Function:  init_addresses 
 * --------------------
 * allocated memory to addresses value and set default values
 * curr_address: addresses
 */
void init_addresses(address ** curr_address) {
    (*curr_address) = (address*) malloc(sizeof(address));

    if((*curr_address) == NULL) {
        print_fundamental_error(NO_AVAILABLE_MEMORY, NULL);
        exit(1);
    }

    (*curr_address) -> IC = IC_MIN_ADDRESS;
    (*curr_address) -> DC = 0;
}
/*
 * Function:  set_address 
 * --------------------
 * add size of data to address count IC/DC
 * curr_address: addresses
 * add: size in bytes of data to add to address count
 * type: which address type to add to
 */
void set_address(address ** curr_address, const int add, const int type) {
    if (type == IS_IC)
        (*curr_address) -> IC += add;
    if (type == IS_DC)
    {
        (*curr_address) -> DC += add;
        add_data_size(&(*curr_address) -> data_arr, add);
    }
    if (((*curr_address) -> IC + (*curr_address) -> DC) >= MAX_MEMORY_SIZE)
    {
        print_fundamental_error(MEMORY_OVERFLOW, NULL);
        exit(1);
    }
}

/*
 * Function:  get_address 
 * --------------------
 * get current address count IC/DC
 * curr_address: addresses
 * type: which address type value to return
 * return: current address value by type
 */
unsigned long get_address(address ** curr_address, enum COUNTER type) {
    unsigned long result = 0;
    switch (type)
    {
        case (IS_IC):
            result = (*curr_address) -> IC;
            break;
        case (IS_DC):
            result = (*curr_address) -> DC;
            break;
    }
    return result;
}

/*
 * Function:  init_code_table 
 * --------------------
 * allocated memory to code table
 * code_table: code image
 */
void init_code_table(code_t ** code_table) {
    (*code_table) = (code_t *) calloc(MAX_MEMORY_SIZE, sizeof(code_t));

    if((*code_table) == NULL) {
        print_fundamental_error(NO_AVAILABLE_MEMORY, NULL);
        exit(1);
    }
}

/*
 * Function:  add_to_image 
 * --------------------
 * adds encoding bit code to existing code_table at specific address
 * code_table: code image
 * code_address: address of code to change
 * encoding: bit code to add to image at address
 * data_size: size of data to add in bytes
 */
void add_to_image(code_t * code_table, unsigned long code_address, unsigned long encoding, int data_size) {
    unsigned long i;
    unsigned long mask = create_mask(BITS_IN_BYTE); /*byte of 1's*/

    reverse(encoding, data_size);

    for (i = 0; i < data_size ; i++) /*add encoding to code_table in each byte of total data_size*/
    {
        code_table[code_address + i].value |= ((encoding >> ((data_size - 1 - i) * BITS_IN_BYTE)) & mask);
    } 
}

/*
 * Function:  get_from_image 
 * --------------------
 * get encoding bit code from existing code_table at specific address
 * code_table: code image
 * code_address: address of code to get
 * encoding: gets image at address
 * data_size: size of data to get in bytes
 */
void get_from_image(code_t * code_table, unsigned long code_address, unsigned long * encoding, int data_size) {
    unsigned long i;
    unsigned char temp = 0;

    for (i = 0; i < data_size ; i++)
    {
        temp = code_table[code_address + i].value;
        *encoding |= temp;
        if (i != data_size-1) /*prevent last shift*/
            *encoding <<= BITS_IN_BYTE;
    } 
}
/*
 * Function:  create_code_image
 * --------------------
 * add instruction and data images to code image
 * code_image: instruction image, converted to code_image
 * d_image: data image
 * dcf: final data count
 * icf: final instruction count
 */
void create_code_image(code_t * code_image, code_t * d_image, const int dcf, const int icf) {
    int i;

    for (i = 0; i < dcf; i++)
    {
        code_image[icf + i].value |= d_image[i].value;
    }
}
/*
 * Function:  add_data_size 
 * --------------------
 * add size of data_image to list
 * head: data_size table header
 * add: size of data in data image
 */
void add_data_size(data_array ** head, int add) {
    data_array * new_data = (struct data_array *) malloc(sizeof(struct data_array));
    data_array * node = *head;

    if(new_data == NULL) {
        print_fundamental_error(NO_AVAILABLE_MEMORY, NULL);
        exit(1);
    }
    new_data -> data_size = add; 
    new_data -> next = NULL;
    if (*head == NULL) { /*if empty table, new data is the head*/
        *head = new_data;
        return;
    }
    while ((node -> next) != NULL) /*find the last data */
    {
        node = node -> next;
    }
    node -> next = new_data; /*connect last data to the new data */
} /* end of add_data_size */

/*
 * Function:  delete_data_size_table 
 * --------------------
 * delete data_size table
 * head: data_size array header
 */
void delete_data_size_table(data_array ** head) {
  data_array *curr = *head, *temp = NULL;
    if (*head == NULL)
        return;

    curr = curr -> next; 
    while (curr != NULL) 
    {
        temp = curr;
        curr = curr -> next;
        free(temp);
    }
    free(*head);
    *head = NULL;
    head = NULL; 
}/*end of delete_data_size_table */
