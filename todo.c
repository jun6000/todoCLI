#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK 10

typedef struct list {
    char **lines;
    int lineCount;
} LIST;

char *getLine (FILE *stream) {
    if (!stream) return "No stream specified!!";
    
    char *line = malloc (BLOCK);
    if (!line) return "Line malloc failed!!";
    
    int i = 0;
    char c = getc (stream);

    while (c != '\n' && c != EOF) {
        if ((i + 1) % BLOCK == 0) {
            char *temp = realloc (line, (i + 1) + BLOCK);
            if (!temp) {
                free (line);
                return "Temp malloc failed!!";
            }
            line = temp;
        }
        line[i++] = c;
        c = getc (stream);
    }
    line[i] = '\0';
    
    return line;
}

LIST *createList () {
    LIST *list = malloc (sizeof (LIST));
    if (!list) return NULL;
    list->lines = malloc (BLOCK * sizeof (char*));
    if (!(list->lines)) {
        free (list);
        return NULL;
    }
    list->lineCount = 0;
    return list;
}

int addLine (LIST **ptr, char *line) {
    if ((*ptr)->lineCount != 0 && (*ptr)->lineCount % BLOCK == 0) {
        char **temp = realloc ((*ptr)->lines, ((*ptr)->lineCount + BLOCK) * sizeof (char*));
        if (!temp) return -1;
        (*ptr)->lines = temp;
    }
    char *temp2 = malloc (strlen (line) + 1);
    if (!temp2) return -1;
    (*ptr)->lines[((*ptr)->lineCount)] = temp2;
    strcpy ((*ptr)->lines[((*ptr)->lineCount)++], line);
    return 0;
}

int deleteLine (LIST **ptr, int n) {
    if (n < (*ptr)->lineCount && n >= 0) {
        free ((*ptr)->lines[n]);
        for (int i = n; i < (*ptr)->lineCount - 1; i++) (*ptr)->lines[i] = (*ptr)->lines[i + 1];
        (*ptr)->lineCount--;
        return 0;
    }
    return -1;
}

int printLines (LIST *ptr) {
    for (int i = 0; i < ptr->lineCount; i++) printf ("%s\n", ptr->lines[i]);
    return 0;
}

int printUI () {
    	system ("clear");
        printf ("*** Help: ***\n\n");
        printf ("-> Type 'add <task name>' to add a task.\n");
        printf ("-> Type 'remove <task number> to complete a task and remove it from the list.\n");
        printf ("-> Type 'history' to view commands used in current session.\n");
        printf ("-> Type 'exit' to save and exit.\n");
        return 0;
}

int countTasks (FILE *fptr) {
    fptr = fopen ("tasks.txt", "r");
    int count = 0;
    for (char c = getc(fptr) ; c != EOF ; c = getc(fptr)) if (c == '\n') count++;
    fclose (fptr);
    return count;
}

int printTasksUI (LIST *tasks) {
    printf("\n*** Tasks to Complete ***\n\n");
    for (int i = 0; i < tasks->lineCount; i++) printf ("%d. %s\n", i + 1, tasks->lines[i]);
    printf("\n*************\ncommand> ");
    return 0;
}

int printHistoryUI (LIST *history) {
    system ("clear");
    printf("*** History ***\n\n");
    for (int i = 0; i < history->lineCount; i++) printf ("-> %s\n", history->lines[i]);
    printf ("\nEnd of history.\nPress ENTER to go back..");
    getchar ();
    return 0;
}

int invalidCommand () {
    printf ("\nInvalid command!\nPress ENTER to try again..");
    getchar ();
    return 0;
}

int add (LIST **ptr, char *input) {
    addLine (ptr, input + 4);
    return 0;
}

int rem (LIST **ptr, char *input, LIST **hptr) {
    char *temp = input + 7;
    if (*temp == '\0') goto error;
    int i = 0;
    while (temp[i] != '\0') {
        if (!(isdigit (temp[i]))) goto error;
        i++;
    }
    int n;
    sscanf (temp, "%d", &n);
    if (n <= (*ptr)->lineCount && n > 0) {
        temp = malloc (strlen ((*ptr)->lines[n - 1]) + 10);
        if (!temp) return -1;
        strcpy (temp, "removed: ");
        strcat (temp, (*ptr)->lines[n - 1]);
        addLine (hptr, temp);
        free (temp);
        deleteLine (ptr, n - 1);
        return 0;
    }
    error:
        invalidCommand ();
        return -1;
}

int commandCheck (char *input, LIST **tasks, LIST **history) {
    if (strstr (input, "add ") == input) add (tasks, input);
    else if (strstr (input, "remove ") == input) rem (tasks, input, history);
    else if (strcmp (input, "history") == 0) printHistoryUI (*history);
    else invalidCommand ();
    return 0;
}

int saveFile (FILE *fptr, LIST *tasks) {
    remove ("tasks.txt");
    fptr = fopen ("tasks.txt", "a+");
    for (int i = 0; i < tasks->lineCount; i++) fprintf (fptr, "%s\n", tasks->lines[i]);
    fclose (fptr);
    return 0;
}

int freeAll (LIST *tasks, LIST *history) {
    for (int i = 0; i < tasks->lineCount; i++) free (tasks->lines[i]);
    free (tasks->lines);
    free (tasks);
    for (int i = 0; i < history->lineCount; i++) free (history->lines[i]);
    free (history->lines);
    free (history);
    return 0;
}

int main () {

    FILE *fptr;
    LIST *tasks = createList ();
    LIST *history = createList ();
    
    // Copy lines from file and store in buffer
    fptr = fopen ("tasks.txt", "r");
    char *temp;
    for (int i = 0; i < countTasks (fptr); i++) {
        temp = getLine (fptr);
        addLine (&tasks, temp);
        free (temp);
    }
    fclose (fptr);
    
    // Start interactive loop
    char *input;
    while (1) {
        printUI ();
        printTasksUI (tasks);
        input = getLine (stdin);
        if (strcmp (input, "exit") == 0) {
            free (input);
            break;
        }
        addLine (&history, input);
        commandCheck (input, &tasks, &history);
        free (input);
    }
    system ("clear");
    saveFile (fptr, tasks);
    freeAll (tasks, history);

    return 0;
}
