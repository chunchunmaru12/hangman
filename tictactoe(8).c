
#ifdef _WIN32
#define CLEAR "cls"
#include <Windows.h>
#else // In any other OS
#define CLEAR "clear"
#include <unistd.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct p {
  char name[50];
  int move;
  void (*makemove)(struct p *);
} Player;

// initializing the board with points and their initial values
static int board[] = {32, 32, 32, 32, 32, 32, 32, 32, 32};

void user_move(Player *);
Player *check();
void print_board(Player *);
int get_valid_move();
void random_bot(Player *);
void random_but_intelligent(Player *);
void intelligent_bot(Player *);
int get_column_sum(int);
int get_row_sum(int);
int get_diagonal_sum(int, int);

// players for the game with their respective move and how they make move
Player player1 = {"Player1", 'O', user_move};
Player player2 = {"Player2", 'X', random_but_intelligent};

int main() {
  Player *current = &player1; // current player
  Player *winner = NULL;

  int doable_moves = 9;
  while (winner == NULL && doable_moves-- > 0) {
    print_board(current);
    current->makemove(current); // does the respective move of the user
                                // according to the function passed
    winner = check();
    current =
        current == &player1 ? &player2 : &player1; // switches player's turns
  }

  // Printing the result
  print_board(NULL);
  if (winner != NULL)
    printf("\n%s Wins\n", winner->name);
  else
    printf("\nIt's draw\n");
}

void user_move(Player *player) {
  int player_move;
  int p;
  while ((player_move = get_valid_move())) {
    p = player_move - 'a';
    if (board[p] == 32) {
        board[p] = player->move;
      break;
    }
  }
}

void random_bot(Player *player) {
  int open_spaces[9];
  int count = 0;
  int i, j;
  for (i = 0, j = 0; i < 9; i++)
    if (board[i] == 32) {
      open_spaces[j] = i;
      count++;
      j++;
    }
  srand(time(0));
  if (count > 0) {
    sleep(1);
    board[open_spaces[rand() % count]] = player->move;
  }
}

void random_but_intelligent(Player *player) {
  int check_win(Player *);
  int d;
  Player *other = player == &player1 ? &player2 : &player1;
  d = check_win(player);
  if (d >= 0) {
    sleep(1);
    board[d] = player->move;
    return;
  }
  d = check_win(other);
  if (d >= 0) {
    sleep(1);
    board[d] = player->move;
    return;
  }

  float favority[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int row, column, diagonal1, diagonal2;
  float f;
  int i;
  for (i = 0; i < 9; i++) {
    if (board[i] != 32)
      continue;
    row = column = diagonal1, diagonal2 = f = 0;
    row = get_row_sum(i);
    column = get_column_sum(i);
    diagonal1 = get_diagonal_sum(i, 1);
    diagonal2 = get_diagonal_sum(i, 2);

    if (row == player->move + 64)
      f += 1;
    if (row == other->move + 64)
      f = f + f > 0.2 ? -0.2 : 0;
    if (column == player->move + 64)
      f += 1;
    if (column == other->move + 64)
      f = f + f > 0.2 ? -0.2 : 0;
    if (diagonal1 + diagonal2 == other->move + 160)
      f = 2;
    else {
      if (diagonal1 == player->move + 64)
        f += 1;
      if (diagonal1 == other->move + 64)
        f = f + f > 0.2 ? -0.2 : 0;
      if (diagonal2 == player->move + 64)
        f += 1;
      if (diagonal2 == other->move + 64)
        f = f + f > 0.2 ? -0.2 : 0;
    }
    favority[i] = f;
  }
  int greatest = 0;
  int favored = -1;
  for (i = 0; i < 9; i++) {
    if (favority[i] > greatest) {
      greatest = favority[i];
      favored = i;
    }
  }
  if (favored >= 0) {
    sleep(1);
    board[favored] = player->move;
    return;
  }

  random_bot(player);
}

void intelligent_bot(Player *player) {
  int check_win(Player *);
  int check_win_count(Player *);
  int d;
  int cmpfunc(const void *a, const void *b);
  Player *other = player == &player1 ? &player2 : &player1;
  d = check_win(player);
  if (d >= 0) {
    sleep(1);
    board[d] = player->move;
    return;
  }
  d = check_win(other);
  if (d >= 0) {
    sleep(1);
    board[d] = player->move;
    return;
  }
  int i, j, fav_count, unfav_count, k, l, temp;
  int favority[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int unfavority[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int fav_unfav[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  for (i = 0; i < 9; i++) {
    if (board[i] != 32)
      continue;
    board[i] = player->move;
    fav_count = check_win_count(player);
    favority[i] = fav_count;
    d = check_win(player);
    if (d >= 0) {
      board[d] = other->move;
      unfav_count = check_win(other);
      unfavority[i] = unfav_count;
      board[d] = 32;
    } else {
      for (j = 0; j < 9; j++) {
        if (board[j] != 32)
          continue;
        board[j] = other->move;
        unfav_count = check_win_count(other);
        unfavority[i] = unfav_count;
        board[j] = 32;
      }
    }
    board[i] = 32;
  }
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      k = fav_unfav[i];
      l = fav_unfav[j];
      if (favority[l] != favority[k]) {
        if (favority[l] < favority[k]) {
          temp = fav_unfav[i];
          fav_unfav[i] = fav_unfav[j];
          fav_unfav[j] = temp;
        }
      } else {
        if (unfavority[l] > unfavority[k]) {
          temp = fav_unfav[i];
          fav_unfav[i] = fav_unfav[j];
          fav_unfav[j] = temp;
        }
      }
    }
  }
  temp = get_column_sum(0) + get_column_sum(1) + get_column_sum(2);
  if (temp == 32 * 9) {
    int corners[] = {0, 2, 6, 8};
    srand(time(0));
    board[corners[rand() % 4]] = player->move;
    sleep(1);
  }
  else if (temp == (other->move + 32 * 8) &&
      (board[0] == other->move || board[2] == other->move ||
       board[6] == other->move || board[8] == other->move)) {
    board[4] = player->move;
    sleep(1);
  } else if (board[fav_unfav[0]] == 32) {
    board[fav_unfav[0]] = player->move;
    sleep(1);
  } else
    random_bot(player);
}

int get_valid_move() {
  int temp;
  temp = tolower(getchar());
  if (temp >= 'a' && temp <= 'i')
    return temp;
  else
    return get_valid_move();
}

void print_board(Player *current) {
  system(CLEAR);
  if (current != NULL)
    printf("\n%s's turn\n\n", current->name);
  int i, j;
  for (i = 0; i <= 6; i += 3) {
    for (j = 1; j <= 3; j++)
      printf(" %c %c", board[i + j - 1], j == 3 ? '\t' : '|');
    for (j = 1; j <= 3; j++)
      printf(" %c %c", 96 + i + j, j == 3 ? '\n' : '|');
    if (i != 6)
      printf("-----------\t-----------\n");
  }
}

Player *check() {
  int sum = 0;
  Player *winner(int n, Player *p1, Player *p2);
  Player *p = NULL;
  int i;
  // Checking for colulmns
  for (i = 0; p == NULL && i <= 2; i++) {
    sum = board[i] + board[i + 3] + board[i + 6];
    p = winner(sum, &player1, &player2);
  }

  // Checking for rows
  for (i = 0; p == NULL && i <= 6; i += 3) {
    sum = board[i] + board[i + 1] + board[i + 2];
    p = winner(sum, &player1, &player2);
  }

  // Checking for diaognals
  if (p == NULL)
    p = winner(board[0] + board[4] + board[8], &player1, &player2);
  if (p == NULL)
    p = winner(board[2] + board[4] + board[6], &player1, &player2);
  return p;
}

Player *winner(int n, Player *p1, Player *p2) {
  if (n == p1->move * 3)
    return p1;
  else if (n == p2->move * 3)
    return p2;
  else
    return NULL;
}

int check_win(Player *player) {
  int sum = 0;
  int i, j;
  for (i = 0; i <= 2; i++) {
    sum = board[i] + board[i + 3] + board[i + 6];
    if (sum == player->move * 2 + 32)
      return board[i] == 32 ? i : board[i + 3] == 32 ? i + 3 : i + 6;
  }
  for (i = 0; i <= 6; i += 3) {
    sum = board[i] + board[i + 1] + board[i + 2];
    if (sum == player->move * 2 + 32)
      return board[i] == 32 ? i : board[i + 1] == 32 ? i + 1 : i + 2;
  }

  if (board[0] + board[4] + board[8] == player->move * 2 + 32)
    return board[0] == 32 ? 0 : board[4] == 32 ? 4 : 8;

  if (board[2] + board[4] + board[6] == player->move * 2 + 32)
    return board[2] == 32 ? 2 : board[4] == 32 ? 4 : 6;

  return -1;
}

int check_win_count(Player *player) {
  int count = 0;
  int sum = 0;
  int i, j;
  for (i = 0; i <= 2; i++) {
    sum = board[i] + board[i + 3] + board[i + 6];
    if (sum == player->move * 2 + 32)
      count += 1;
  }
  for (i = 0; i <= 6; i += 3) {
    sum = board[i] + board[i + 1] + board[i + 2];
    if (sum == player->move * 2 + 32)
      count += 1;
  }
  if (board[0] + board[4] + board[8] == player->move * 2 + 32)
    count += 1;

  if (board[2] + board[4] + board[6] == player->move * 2 + 32)
    count += 1;
  return count;
}

int get_column_sum(int n) {
  int sum = 0;
  if (n >= 0 && n <= 2)
    sum = board[n] + board[n + 3] + board[n + 6];
  else if (n >= 3 && n <= 5)
    sum = board[n - 3] + board[n] + board[n + 3];
  else
    sum = board[n - 6] + board[n - 3] + board[n];
  return sum;
}

int get_row_sum(int n) {
  int sum = 0;
  if (n % 3 == 0)
    sum = board[n] + board[n + 1] + board[n + 2];
  else if (n == 1 || n == 4 || n == 7)
    sum = board[n - 1] + board[n] + board[n + 1];
  else
    sum = board[n - 2] + board[n - 1] + board[n];
  return sum;
}

int get_diagonal_sum(int i, int d) {
  int sum = 0;
  if (d == 1) {
    if (i == 0 || i == 4 || i == 8)
      sum += (board[0] + board[4] + board[8]);

  } else {
    if (i == 2 || i == 4 || i == 6)
      sum += (board[2] + board[4] + board[6]);
  }
  return sum;
}

