#include <windows.h>


int main (void){
    const char message[]="My message!";
    const char title[]="Ignore";
    int WinBox= MessageBoxA(
        NULL,
        message,
        title,
        MB_OK
    );
    return EXIT_SUCCESS ;
}