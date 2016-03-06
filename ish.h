// print prompt
void prompt();

// Top-level evaluation
void eval(char* s);

// Run program
int run(char* s);

// Generate argv for execvp
char** getargv(char* s);

// Get executable file
char* getexec(char* s);
