#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_INDENT_LENGTH (50)

#define TOKEN_ILLEGAL    ( 0)
#define TOKEN_EOF        ( 1)
#define TOKEN_IDENT      ( 2)
#define TOKEN_INT        ( 3)
#define TOKEN_EQUALS     ( 4)
#define TOKEN_PLUS       ( 5)
#define TOKEN_COMMA      ( 6)
#define TOKEN_SEMICOLON  ( 7)
#define TOKEN_L_PAREN    ( 8)
#define TOKEN_R_PAREN    ( 9)
#define TOKEN_L_SQ       (10)
#define TOKEN_R_SQ       (11)
#define TOKEN_FUNCTION   (12)
#define TOKEN_LET        (13)

typedef struct {
   int type;
   char *literal;
} Token;

typedef struct {
   char *input;
   int position;
   int read_position;
   char ch;
} Lexer;

Lexer new_lexer(char *input);
void read_char(Lexer *l);
Token new_token(int type, char ch);
Token next(Lexer *l);

char *strndup(const char *str, size_t n) {
    char *copy = malloc(n + 1);
    if (copy) {
        strncpy(copy, str, n);
        copy[n] = '\0';
    }
    return copy;
}

int is_letter(char ch) {
   return isalpha(ch) || ch == '_';
}

int is_digit(char ch) {
   return isdigit(ch);
}

void skip_whitespace(Lexer *l) {
   while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') { read_char(l); }
}

char* read_identifier(Lexer *l) {
   int start = l->position;
   while (is_letter(l->ch) || is_digit(l->ch)) { read_char(l); }

   return strndup(l->input + start, l->position - start);
}

char* read_number(Lexer *l) {
   int start = l->position;
   while (is_digit(l->ch)) read_char(l);

   return strndup(l->input + start, l->position - start);
}

Token new_token(int type, char ch) {
   char *literal = malloc(2);
   literal[0] = ch;
   literal[1] = '\0';
   return (Token){type, literal};
}

Token next(Lexer *l) {
   Token token;

   skip_whitespace(l);

   switch (l->ch) {
      case '=':
         if (l->input[l->read_position] == '=') {
    	    char ch = l->ch;
            read_char(l);
            token = (Token){TOKEN_EQUALS, "==\0"};
         } else {
            token = new_token(TOKEN_EQUALS, l->ch);
         }
         break;
      case '+':
            token  = new_token(TOKEN_PLUS, l->ch);
         break;
      case ',':
         token = new_token(TOKEN_COMMA, l->ch);
         break;
      case ';':
         token = new_token(TOKEN_SEMICOLON, l->ch);
         break;
      case '(':
         token = new_token(TOKEN_L_PAREN, l->ch);
         break;
      case ')':
         token = new_token(TOKEN_R_PAREN, l->ch);
         break;
      case '{':
         token = new_token(TOKEN_L_SQ, l->ch);
         break;
      case '}':
         token = new_token(TOKEN_R_SQ, l->ch);
         break;
      case '\0':
         token = (Token){TOKEN_EOF, ""};
         break;
      default:
         if (is_letter(l->ch)) {
            char *ident = read_identifier(l);
            int t_type = TOKEN_IDENT;
            if (strcmp(ident, "function") == 0) {
                     t_type = TOKEN_FUNCTION;
            } else if (strcmp(ident, "let") == 0) {
                     t_type = TOKEN_LET;
            }

            token = (Token){t_type, ident};
         } else if (is_digit(l->ch)) {
            char *num = read_number(l);
            token = (Token){TOKEN_INT, num};
         } else {
                  token = (Token){TOKEN_ILLEGAL, l->ch};
         }
   }

   read_char(l);
   return token;
}

void read_char(Lexer *l) {
   if (l->read_position >= strlen(l->input)) {
      l->ch = "\0";
   } else  {
      l->ch = l->input[l->read_position];
   }

   l->position = l->read_position;
   l->read_position++;
}

Lexer new_lexer(char *input) {
   Lexer l;
   l.input = input;
   l.position = 0;
   l.read_position = 0;
   read_char(&l);
   return l;
}

int main() {
   char *input = "let x = 5; let y = 10;\nfunction add(x, y) {\n return x + y;\n}\n";
   Lexer lexer = new_lexer(input);
   
   Token token;
   do {
      token = next(&lexer);
      printf("Type: %d, Literal: %s\n", token.type, token.literal);
   } while (token.type != TOKEN_EOF);

   return 0;
}
