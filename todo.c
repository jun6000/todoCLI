#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK 10

enum ERR_CODES {
    MALLOC_FAIL = -1,
    SUCCESS,
    FAIL
};

typedef struct Line {
    char *string;
    int len;
} LINE;

typedef struct List {
    LINE **lines;
    int lineCount;
} LIST;

LINE *createLine () {
    LINE *line = malloc (sizeof (LINE));
    if (!line) return NULL;
    line->string = NULL;
    line->len = 0;
    return line;
}

LINE *getLine (FILE *stream) {
    if (!stream) return NULL;
    
    // Get input string
    char *string = malloc (BLOCK);
    if (!string) return NULL;
    
    int i = 0;
    char c = getc (stream);

    while (c != '\n' && c != EOF) {
        if ((i + 1) % BLOCK == 0) {
            char *temp = realloc (string, (i + 1) + BLOCK);
            if (!temp) {
                free (string);
                return NULL;
            }
            string = temp;
        }
        string[i++] = c;
        c = getc (stream);
    }
    string[i] = '\0';
    
    // Allocate memory to struct pointer
    LINE *line = malloc (sizeof (LINE));
    if (!line) {
        free (string);
        return NULL;
    }

    // Assign string and len to members
    line->string = string;
    line->len = i;

    return line;
}

LIST *createList () {
    LIST *list = malloc (sizeof (LIST));
    if (!list) return NULL;
    list->lines = malloc (BLOCK * sizeof (LINE*));
    if (!(list->lines)) {
        free (list);
        return NULL;
    }
    list->lineCount = 0;
    return list;
}

int addLine (LIST **ptr, LINE *line) {

    // Realloc memory if insufficient
    if ((*ptr)->lineCount != 0 && (*ptr)->lineCount % BLOCK == 0) {
        LINE **temp = realloc ((*ptr)->lines, ((*ptr)->lineCount + BLOCK) * sizeof (LINE*));
        if (!temp) return MALLOC_FAIL;
        (*ptr)->lines = temp;
    }

    // Create a LINE *temp2 to store line
    LINE *temp2 = createLine ();
    temp2->string = malloc (line->len + 1);
    if (!(temp2->string)) 
    {
        free (temp2);
        return MALLOC_FAIL;
    }

    // Store a copy of line in temp2
    temp2->len = line->len;
    strcpy (temp2->string, line->string);
    (*ptr)->lines[((*ptr)->lineCount)++] = temp2;
    return SUCCESS;
}

int freeLine (LINE *ptr) {
    free (ptr->string);
    free (ptr);
    return SUCCESS;
}

int deleteLine (LIST **ptr, int n) {
    if (n < (*ptr)->lineCount && n >= 0) {
        freeLine ((*ptr)->lines[n]);
        for (int i = n; i < (*ptr)->lineCount - 1; i++) (*ptr)->lines[i] = (*ptr)->lines[i + 1];
        (*ptr)->lineCount--;
        return SUCCESS;
    }
    return FAIL;
}

int freeList (LIST *ptr) {
    for (int i = 0; i < ptr->lineCount; i++) freeLine (ptr->lines[i]);
    free (ptr->lines);
    free (ptr);
    return SUCCESS;
}

int printLine (LINE *ptr) {
    printf ("%s\n", ptr->string);
    return SUCCESS;
}

int printHelp () {
    	system ("clear");
        printf ("*** Help ***\n\n");
        printf ("-> Type 'add <task name>' to add a task.\n");
        printf ("-> Type 'remove <task number>' to complete a task and remove it from the list.\n");
        printf ("-> Type 'history' to view commands used in current session.\n");
        printf ("-> Type 'exit' to save and exit.\n");
        printf ("\nEnd of help.\nPress ENTER to go back..");
        getchar ();
        return SUCCESS;
}

int countTasks (FILE *fptr) {
    if (!fptr) return FAIL;
    fptr = fopen ("tasks.txt", "r");
    int count = 0;
    for (char c = getc(fptr); c != EOF; c = getc(fptr)) if (c == '\n') count++;
    fclose (fptr);
    return count;
}

int printTasksUI (LIST *tasks) {
    system ("clear");
    printf ("*** Tasks to do ***\n\n");
    for (int i = 0; i < tasks->lineCount; i++) printf ("%d. %s\n", i + 1, (tasks->lines[i])->string);
    printf ("\ncommand> ");
    return SUCCESS;
}

int printHistoryUI (LIST *history) {
    system ("clear");
    printf("*** History ***\n\n");
    for (int i = 0; i < history->lineCount; i++) printf ("-> %s\n", (history->lines[i])->string);
    printf ("\nEnd of history.\nPress ENTER to go back..");
    getchar ();
    return SUCCESS;
}

int invalidCommand () {
    printf ("\nInvalid command! (Try \"h\" for help)\nPress ENTER to try again..");
    getchar ();
    return SUCCESS;
}

int invalidNumber () {
    printf ("\nPlease enter a valid number! (Try \"h\" for help)\nPress ENTER to try again..");
    getchar ();
    return SUCCESS;
}

int add (LIST **ptr, LINE *input) {
    char *temp = malloc (input->len - 3);
    if (!temp) return MALLOC_FAIL;

    // Copy substring after "add" keyword
    strcpy (temp, (input->string) + 4);
    LINE *temp2 = createLine ();
    temp2->string = temp;
    temp2->len = input->len - 4;
    addLine (ptr, temp2);
    freeLine (temp2);
    return SUCCESS;
}

int rem (LIST **ptr, LINE *input, LIST **hptr) {
    
    // Check for valid input after "remove" keyword
    char *temp = (input->string) + 7;
    if (*temp == '\0') goto error;
    int i = 0;
    while (temp[i] != '\0') {
        if (!(isdigit (temp[i]))) goto error;
        i++;
    }

    // Get task number of task to be removed
    int n;
    sscanf (temp, "%d", &n);
    if (n <= (*ptr)->lineCount && n > 0) {
        
        // Append string to be removed to history and delete it from tasks
        LINE *temp2 = createLine ();
        temp2->string = malloc (((*ptr)->lines[n - 1])->len + 10);
        if (!(temp2->string)) {
            free (temp2);
            return MALLOC_FAIL;
        }
        strcpy (temp2->string, "removed: ");
        strcat (temp2->string, ((*ptr)->lines[n - 1])->string);
        temp2->len = ((*ptr)->lines[n - 1])->len + 9;
        addLine (hptr, temp2);
        freeLine (temp2);
        deleteLine (ptr, n - 1);
        return SUCCESS;
    }
    error:
        invalidNumber ();
        return FAIL;
}

int commandCheck (LINE *input, LIST **tasks, LIST **history) {
    if (strstr (input->string, "add ") == input->string) add (tasks, input);
    else if (strstr (input->string, "remove ") == input->string) rem (tasks, input, history);
    else if (strcmp (input->string, "history") == 0) printHistoryUI (*history);
    else if (strcmp (input->string, "h") == 0) printHelp ();
    else invalidCommand ();
    return SUCCESS;
}

int saveFile (FILE *fptr, LIST *tasks) {
    
    // Remove existing file and replace with modified file
    remove ("tasks.txt");
    fptr = fopen ("tasks.txt", "a+");
    for (int i = 0; i < tasks->lineCount; i++) fprintf (fptr, "%s\n", (tasks->lines[i])->string);
    fclose (fptr);
    return SUCCESS;
}

int main () {

    // Declare required pointers
    FILE *fptr;
    LIST *tasks = createList ();
    LIST *history = createList ();
    
    // Copy lines from file and store in buffer
    fptr = fopen ("tasks.txt", "r");
    LINE *temp;
    for (int i = 0; i < countTasks (fptr); i++) {
        temp = getLine (fptr);
        addLine (&tasks, temp);
        freeLine (temp);
    }
    fclose (fptr);
    
    // Start interactive loop
    LINE *input;
    while (1) {
        printTasksUI (tasks);
        input = getLine (stdin);
        if (strcmp (input->string, "exit") == 0) {
            freeLine (input);
            break;
        }
        if (strcmp (input->string, "history") != 0) addLine (&history, input);
        commandCheck (input, &tasks, &history);
        freeLine (input);
    }
    system ("clear");
    saveFile (fptr, tasks);
    freeList (tasks);
    freeList (history);

    return SUCCESS;
}