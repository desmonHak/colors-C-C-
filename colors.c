#ifndef __COLORS_C__
#define __COLORS_C__ "Desmon.hak.anon"
#include "colors.h"

// genera un valor entre 0 y 255
unsigned int jenkins_hash(
    unsigned int value, 
    unsigned int n1, unsigned int n2, unsigned int n3,
    unsigned int n4, unsigned int n5, unsigned int n6
    ) {
    value = (value + 0x7ed55d16) + (value << n1);
    value = (value ^ 0xc761c23c) ^ (value >> n2);
    value = (value + 0x165667b1) + (value << n3);
    value = (value + 0xd3a2646c) ^ (value << n4);
    value = (value + 0xfd7046c5) + (value << n5);
    value = (value ^ 0xb55a4f09) ^ (value >> n6);
    return value % 256;
}
// combina los valores de un array
void shuffle_array(int array[], int size) {
    srand(time(NULL));

    for (int i = size - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
// genera tres valores aparit de 1 usando 6 desplazamientos
void generate_three_values(
    unsigned int x, 
    unsigned int* value1, 
    unsigned int* value2, 
    unsigned int* value3,
    unsigned int n1, unsigned int n2, unsigned int n3,
    unsigned int n4, unsigned int n5, unsigned int n6) {
    if (x < 0 || x > 255) {
        printf("El número debe estar en el rango de 0 a 255.\n");
        return;
    }

    // Aplicar la función de dispersión hash a los valores iniciales
    *value1 = jenkins_hash(x, n1, n2, n3, n4, n5, n6);
    *value2 = jenkins_hash(*value1, n1, n2, n3, n4, n5, n6);
    *value3 = jenkins_hash(*value2, n1, n2, n3, n4, n5, n6);
}

void resetColorTerminal()
{
#ifdef _WIN32

    setConsoleBackgroundColor(BACKGROUND_BLACK);
    setConsoleForegroundColor(FOREGROUND_WHITE);
    //setConsoleColor(FOREGROUND_MASK, 0);
    // SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_MASK);
#else
    printf(BACKGROUND_COLOR_RESET_ANSI);
#endif
}

void __attribute__((destructor)) _RESET_COLOR__()
{
    // setConsoleColor(C_WHITE, C_BLACK);
    // SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    resetColorTerminal();
}

#ifdef _WIN32
void resetConsoleForegroundColor(){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD attributes = consoleInfo.wAttributes;
    attributes &=  0xFFF0; // Eliminar el color de la letra actual
    attributes |= foregroundColor;  // Establecer el nuevo color de la letra
    SetConsoleTextAttribute(hConsole, attributes);
}

void setConsoleForegroundColor(WORD foregroundColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD attributes = consoleInfo.wAttributes;
    attributes &=  0xFFF0; // Eliminar el color de la letra actual
    SetConsoleTextAttribute(hConsole, attributes);
}
#else
void setConsoleForegroundColor(ConsoleColor foregroundColor)
{
    printf("\033[%dm", 30 + foregroundColor);
}
#endif

#ifdef _WIN32
void resetConsoleBackgroundColor(){
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

    WORD attributes = consoleInfo.wAttributes;
    attributes &= 0x000F; // Limpiar los bits de color de fondo existentes
    SetConsoleTextAttribute(hConsole, attributes);
}
void setConsoleBackgroundColor(WORD backgroundColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

    WORD attributes = consoleInfo.wAttributes;
    attributes &= 0x000F; // Limpiar los bits de color de fondo existentes
    attributes |= backgroundColor;

    SetConsoleTextAttribute(hConsole, attributes);
}
#else
    void setConsoleBackgroundColor(ConsoleColor backgroundColor)
    {
        printf("\033[0;%dm", 40 + backgroundColor);
    }
#endif

#ifdef _WIN32
void __attribute__((constructor)) _ACTIVATE_COLORS_ANSI_WIN__()
{
    // Habilitar el soporte de colores ANSI en la consola de Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode))
    {
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        {
            // Habilitar los colores ANSI en la consola de Windows
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
}
#endif

#ifdef _WIN32
void setConsoleColor(WORD foreground, WORD background)
{
    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // SetConsoleTextAttribute(hConsole, (int)foreground | ((int)background << 4));
    setConsoleForegroundColor(foreground);
    setConsoleBackgroundColor(background);
}
#else
void setConsoleColor(ConsoleColor foreground, ConsoleColor background)
{
    setConsoleForegroundColor(foreground);
    setConsoleBackgroundColor(background);
}
#endif
void printf_color(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf_color(format, args);
    va_end(args);
}

void vprintf_color(const char *format, va_list args)
{
    unsigned char *formatted_buffer = (unsigned char *)malloc(strlen(format) * sizeof(unsigned char));
    vsprintf(formatted_buffer, format, args);
    const char *p = formatted_buffer;
    bool in_color_code = false;
    char color_code[20];
    int color_code_index = 0;

    while (*p != '\0')
    {
        if (in_color_code)
        {
            if (*p == '}')
            {
                color_code[color_code_index] = '\0';
                if (strcmp(color_code, "FG:red") == 0)
                {
                    // Cambiar a color rojo
                    LETTER_RED;
                }
                #ifdef _WIN32
                else if (strcmp(color_code, "FG:reset") == 0)
                {
                    // Restablecer colores
                    resetConsoleForegroundColor();
                }
                else if (strcmp(color_code, "BG:reset") == 0)
                {
                    // Restablecer colores
                    resetColorTerminal();
                    resetConsoleBackgroundColor();
                }
                #else

                else if (strcmp(color_code, "FG:reset") == 0 || strcmp(color_code, "BG:reset") == 0)
                {
                    // Restablecer colores
                    resetColorTerminal();
                }
                #endif
                else if (strcmp(color_code, "FG:green") == 0)
                {
                    // Cambiar a color verde
                    LETTER_GREEN;
                }
                else if (strcmp(color_code, "FG:blue") == 0)
                {
                    // Cambiar a color azul
                    LETTER_BLUE;
                }
                else if (strcmp(color_code, "FG:black") == 0)
                {
                    // Cambiar a color negro
                    LETTER_BLACK;
                }
                else if (strcmp(color_code, "FG:yellow") == 0)
                {
                    // Cambiar a color amarillo
                    LETTER_YELLOW;
                }
                else if (strcmp(color_code, "FG:purple") == 0)
                {
                    // Cambiar a color magenta
                    LETTER_MAGENTA;
                }
                else if (strcmp(color_code, "FG:cyan") == 0)
                {
                    // Cambiar a color cian
                    LETTER_CYAN;
                }
                else if (strcmp(color_code, "FG:white") == 0)
                {
                    // Cambiar a color blanco
                    LETTER_WHITE;
                }
                else if (strcmp(color_code, "BG:black") == 0)
                {
                    // Cambiar a fondo negro
                    BACKGROUND_COLOR_BLACK;
                }
                else if (strcmp(color_code, "BG:red") == 0)
                {
                    // Cambiar a fondo rojo
                    BACKGROUND_COLOR_RED;
                }
                else if (strcmp(color_code, "BG:green") == 0)
                {
                    // Cambiar a fondo verde
                    BACKGROUND_COLOR_GREEN;
                }
                else if (strcmp(color_code, "BG:yellow") == 0)
                {
                    // Cambiar a fondo amarillo
                    BACKGROUND_COLOR_YELLOW;
                }
                else if (strcmp(color_code, "BG:purple") == 0)
                {
                    // Cambiar a fondo magenta
                    BACKGROUND_COLOR_MAGENTA;
                }
                else if (strcmp(color_code, "BG:cyan") == 0)
                {
                    // Cambiar a fondo cian
                    BACKGROUND_COLOR_CYAN;
                }
                else if (strcmp(color_code, "BG:white") == 0)
                {
                    // Cambiar a fondo blanco
                    BACKGROUND_COLOR_WHITE;
                }
                else if (strcmp(color_code, "BG:blue") == 0)
                {
                    // Cambiar a fondo azul
                    BACKGROUND_COLOR_BLUE;
                } else if (strncmp(color_code, "FG:", 3) == 0)
                {
                    // Comprobar si es un color personalizado FG:r;g;b
                    unsigned char red, green, blue;
                    if (sscanf(color_code, "FG:%hhu;%hhu;%hhu", &red, &green, &blue) == 3)
                    {
                        // Cambiar a color personalizado
                        foreground_color_custom(red, green, blue);
                    }
                }else if (strncmp(color_code, "BG:", 3) == 0)
                {
                    // Comprobar si es un color personalizado FG:r;g;b
                    unsigned char red, green, blue;
                    if (sscanf(color_code, "BG:%hhu;%hhu;%hhu", &red, &green, &blue) == 3)
                    {
                        // Cambiar a color personalizado
                        background_color_custom(red, green, blue);
                    }
                }
                

                in_color_code = false;
                color_code_index = 0;
            }
            else
            {
                color_code[color_code_index] = *p;
                color_code_index++;
            }
        }
        else
        {
            if (*p == '#')
            {
                p++;
                if (*p == '{')
                {
                    in_color_code = true;
                    color_code_index = 0;
                }
                else
                {
                    putchar('#');
                    putchar(*p);
                    fflush(stdout);
                }
            }
            else
            {
                putchar(*p);
                fflush(stdout);
            }
        }

        p++;
    }

    // Restablecer colores
    resetColorTerminal();

}

void clear_line()
{
    printf(CLEAR_LINE);
}
void clear_display()
{
    printf(CLEAR_DISPLAY);
}
void set_title(char *title)
{
    printf(SET_TITLE("%d"), title);
}
void pos(unsigned char x, unsigned char y, char *data)
{
    printf(POS("%d", "%d", "%s"), x, y, data);
}
void back(char *data, unsigned char number)
{
    printf(BACK("%s", "%d"), data, number);
}
void forward(char *data, unsigned char number)
{
    printf(FORWARD("%s", "%d"), data, number);
}
void down(char *data, unsigned char number)
{
    printf(DOWN("%s", "%d"), data, number);
}
void up(char *data, unsigned char number)
{
    printf(UP("%s", "%d"), data, number);
}
static inline void foreground_color_custom_RGB(RGB_C color)
{
    foreground_color_custom_(color.r, color.g, color.b);
}
static void foreground_color_custom_(unsigned char red, unsigned char green, unsigned char blue)
{
    printf(FOREGROUND_COLOR_CUSTOM_RGB("%d","%d","%d"), red, green, blue);
}
static inline void background_color_custom_RGB(RGB_C color)
{
    background_color_custom_(color.red, color.green, color.blue);
}
static void background_color_custom_(unsigned char red, unsigned char green, unsigned char blue)
{
    printf(BACKGROUND_COLOR_CUSTOM_RGB("%d","%d","%d"), red, green, blue);
}
static inline void back_fore_color_custom_RGB(RGB_C colorBackGround, RGB_C colorForeGround)
{
    back_fore_color_custom_(
        colorBackGround.r,
        colorBackGround.g,
        colorBackGround.b,
        colorForeGround.r,
        colorForeGround.g,
        colorForeGround.b);
}
static void back_fore_color_custom_(unsigned char redB, unsigned char greenB,
                                    unsigned char blueB, unsigned char redF,
                                    unsigned char greenF, unsigned char blueF)
{
    foreground_color_custom(redF, greenF, blueF);
    background_color_custom(redB, greenB, blueB);

}

void ANSI_fore_color(ANSIColors color)
{
    printf(ANSI_COLOR_FOREGROUNG("%d"), color);
}
void ANSI_back_color(ANSIColors color)
{
    printf(ANSI_COLOR_BACKGROUNG("%d"), color);
}
#endif