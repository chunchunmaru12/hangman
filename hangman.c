#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _GNU_SOURCE
#define CHANCES 3

typedef struct {
  void *next;
  char *data;
} Node;

Node *head = NULL; // head of the list
int count = 0;     // number of elements in the list

// adds a word to the list
Node *addNode(char *s) {
  Node *new = NULL;
  new = malloc(sizeof(Node));
  if (new == NULL)
    return NULL;
  if (head == NULL) {
    new->data = strdup(s);
    head = new;
    head->next = NULL;
  } else {
    new->data = strdup(s);
    new->next = head;
    head = new;
  }
  return new;
}

// prints all the elements of the list
void printList() {
  Node *current = head;
  while (current != NULL) {
    puts(current->data);
    current = current->next;
  }
  return;
}

// retrieves the n-th element of the list
Node *traverseTo(int n) {
  Node *current = head;
  for (int i = 0; i < n && current->next != NULL; i++) {
    current = current->next;
  }
  return current;
}

void hangman(Node *, int);

int main(int argc, char *argv[]) {
  // buffer to store the line
  char *buffer;
  size_t buffsize = 100;
  size_t characters;
  buffer = (char *)malloc(buffsize * sizeof(char));

  FILE *ptr;
  if ((ptr = fopen("test.txt", "r")) == NULL) {
    printf("File can't be opened\n");
    exit(-1);
  }

  // retrieves the line from the file and stores it into the list
  int last_char;
  while (getline(&buffer, &buffsize, ptr) > 0) {
    last_char = strlen(buffer) - 1;
    if (last_char > 0) {
      buffer[last_char] = buffer[last_char] == '\n' ? '\0' : buffer[last_char];
      addNode(buffer);
      count++;
    }
  }

  // get a random element from the node
  srand(time(0));
  Node *randomNode = traverseTo(rand() % count);
  hangman(randomNode, CHANCES);
  fclose(ptr);

  // freeing memory
  while (head != NULL) {
    Node *new_head = head->next;
    free(head);
    head = new_head;
  }
  free(buffer);
  return 0;
}

int isin(char c, char *a) {
  int found = 0;
  while (*a != '\0') {
    if (*a == c) {
      found = 1;
      break;
    }
    a++;
  }
  return found;
}

void hangman(Node *node, int chances) {
  char *word = node->data;
  char *guessed_word = (char *)malloc(sizeof(word) + 1);
  char guess;
  char guessed[50] = {' '};
  void add(char *, char);
  void printer(char *, char *, char *);
  char getguess();
  printer(guessed_word, guessed, word);

  for (int i = 1; i <= chances;) {
    printer(guessed_word, guessed, word);
    puts(guessed_word);
    if (strcmp(word, guessed_word) == 0)
      break;
    while (isin((guess = getguess()), guessed))
      printf("Already guessed\n");

    add(guessed, guess);

    if (!isin(guess, word)) {
      i++;
      printf("Guess again\n");
    }
  }
  free(guessed_word);
}

void printer(char *guessed_word, char *guessed, char *word) {
  for (; *word != '\0'; word++, guessed_word++) {
    int status;
    *guessed_word = isin(*word, guessed) ? *word : '_';
  }
}

void add(char *s, char c) {
  for (; *s != '\0'; s++)
    ;
  *s++ = c;
  *s = '\0';
}

char getguess() {
  char c;
  c = getchar();
  if (isspace(c))
    return getguess();
  else
    return c;
}
