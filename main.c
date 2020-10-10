#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

typedef enum Logs{
    OK,
    ERROR_SET_WINDOW_INFO,
    ERROR_SET_SCREEN_BUFFER,
    ERROR_SET_FONT,
    ERROR_OF_WRITING_OUTPUT,
    ERROR_OF_CLEAR_SCREEN,
    ERROR_OF_LOADING_DIGITS,
    ERROR_OF_LOADING_ABC
} Logs;

typedef struct Info{
    wchar_t *screen;
    int height;
    int width;
    HANDLE * console;
    Logs *logs;
    int *lenght;
    COORD coordBufCoord;
    short *cells;
    int fieldSizeX;
    int fieldSizeY;
    int left;
    int top;
    int life_min;
    int life_rise;
    int life_max;
    wchar_t **digits;
    wchar_t **abc_point;
} Info;

//Initialize console
Logs SetConsoleSize(HANDLE *console, int height, int width){
    const SMALL_RECT DisplayArea = { 0, 0, width - 1, height - 1};
    COORD bufferSize = {width, height};

    SMALL_RECT zeroWindow = {0, 0, 0, 0};
    if (SetConsoleWindowInfo(*console, TRUE, &zeroWindow) == 0)
        return ERROR_SET_WINDOW_INFO;

    if(SetConsoleScreenBufferSize(*console, bufferSize) == 0)
        return ERROR_SET_SCREEN_BUFFER;

    if(SetConsoleWindowInfo(*console, TRUE, &DisplayArea) == 0)
        return ERROR_SET_WINDOW_INFO;

    return OK;
}

Logs SetConsoleFont(HANDLE *console, CONSOLE_FONT_INFOEX *font){
    if (SetCurrentConsoleFontEx(*console, FALSE, font) == 0)
        return ERROR_SET_FONT;
    return OK;
}

void CreateNewFont(CONSOLE_FONT_INFOEX *font, int height, int width){
    COORD dwFontSize = {width, height};
    font->cbSize = sizeof(CONSOLE_FONT_INFOEX);
    font->dwFontSize = dwFontSize;
    lstrcpyW(font->FaceName, L"Lucida Console");
    font->FontFamily = FF_DONTCARE;
    font->FontWeight = FW_NORMAL;
}

void InitConsole(HANDLE *console, Logs *logs, int height, int width, int font_height, int font_width){
    *console = GetStdHandle(STD_OUTPUT_HANDLE);/*CreateConsoleScreenBuffer(
       GENERIC_READ |           // read/write access
       GENERIC_WRITE,
       FILE_SHARE_READ |
       FILE_SHARE_WRITE,        // shared
       NULL,                    // default security attributes
       CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
       NULL);

    SetConsoleMode(*console, 0x0010);
    SetConsoleActiveScreenBuffer(*console);*/
    CONSOLE_FONT_INFOEX new_font;
    /*GetCurrentConsoleFontEx(&console, FALSE, &new_font);
    COORD dwFontSize = { font_width1, font_height1};
    new_font.dwFontSize = dwFontSize;
    new_font.cbSize = sizeof(new_font);
    lstrcpyW(new_font.FaceName, L"Consolas");*/
    CreateNewFont(&new_font, font_height, font_width);

    logs[0] = SetConsoleFont(console, &new_font);
    logs[1] = SetConsoleSize(console, height, width);
}

//Initialize cells
void InitCells(short *cells, int height, int width){
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            cells[y*width + x] = 0;
}

//Randomize cells
void RandomCells(short *cells, int height, int width){
    srand (time(0));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            cells[y*width + x] = rand()%2;
}

//Printing Logs
void PrintLogs(Logs *logs, int lenght){
    for (int i = 0; i < lenght; i++){
        if (logs[i] != OK){
            switch(logs[i]){
                case ERROR_SET_SCREEN_BUFFER:
                    printf("Error with seting buffer...\n");
                    break;
                case ERROR_SET_WINDOW_INFO:
                    printf("Error with seting window info...\n");
                    break;
                case ERROR_SET_FONT:
                    printf("Error with seting new font...\n");
                    break;
                case ERROR_OF_WRITING_OUTPUT:
                    printf("Error with writing output...\n");
                    break;
                case ERROR_OF_CLEAR_SCREEN:
                    printf("Error with clear screen...\n");
                    break;
                case ERROR_OF_LOADING_DIGITS:
                    printf("Error with loading digits...\n");
                    break;
                case ERROR_OF_LOADING_ABC:
                    printf("Error with loading abc...\n");
                    break;
            }
        }
    }
}

//MainLoop
void MainLoop(Info *info){
    //
    wchar_t *screen = info->screen;
    HANDLE * console = info->console;
    Logs *logs = info->logs;
    COORD coordBufCoord = info->coordBufCoord;
    short *cells = info->cells;
    int *lenght = info->lenght;
    int height = info->height;
    int width = info->width;
    int fieldSizeX = info->fieldSizeX;
    int fieldSizeY = info->fieldSizeY;
    int left = info->left;
    int top = info->top;
    int life_min = info->life_min;
    int life_rise = info->life_rise;
    int life_max = info->life_max;
    wchar_t **digits = info->digits;
    wchar_t **abc_point = info->abc_point;

    short escape = GetAsyncKeyState(VK_ESCAPE);
    short up = GetAsyncKeyState(VK_UP);
    short down = GetAsyncKeyState(VK_DOWN);
    int error = 1;
    int speed = 100;
    up = 0;
    down = 0;
    int step = 1;;
    DWORD dwBytesWritten = 0;

    PrintText("Speed:", screen, abc_point, sizeof("Speed:"), 49, 1);
    PrintText("Life:", screen, abc_point, sizeof("Life:"), 49, 7);
    PrintText("Step:", screen, abc_point, sizeof("Step:"), 49, 13);
    PrintText("Up:fast", screen, abc_point, sizeof("Up:fast"), 49, 19);
    PrintText("Down:slow", screen, abc_point, sizeof("Down:slow"), 49, 25);
    PrintText("Esc:leave", screen, abc_point, sizeof("Esc:leave"), 49, 31);

    while(escape == 0 && error){
        escape = GetAsyncKeyState(VK_ESCAPE);
        up = GetAsyncKeyState(VK_UP);
        down = GetAsyncKeyState(VK_DOWN);

        if (up != 0 && speed > 0)
            speed -= 10;

        if (down != 0 && speed < 1000)
            speed += 10;

        PrintInteger(speed, screen, digits, 81, 1);
        PrintInteger(LifeCount(cells, fieldSizeX, fieldSizeY), screen, digits, 81, 7);
        PrintInteger(step, screen, digits, 81, 13);

        error = PrintScreen(screen, console, logs, coordBufCoord, lenght, height, width);
        UpdateCells(cells, fieldSizeX, fieldSizeY, life_min, life_rise, life_max);
        PrintCells(cells, screen, height, width, fieldSizeY, fieldSizeX, left, top);

        ClearScreen(screen, height, width, 81, 1, width, 18, logs, lenght);

        step++;
        Sleep(speed);
    }
}

//Main Menu
void MainMenu(Info *info){
    wchar_t *screen = info->screen;
    HANDLE * console = info->console;
    wchar_t **digits = info->digits;
    wchar_t **abc_point = info->abc_point;
    int height = info->height;
    int width = info->width;
    Logs *logs = info->logs;
    COORD coordBufCoord = info->coordBufCoord;
    int *lenght = info->lenght;
    short *cells = info->cells;
    int fieldSizeY = info->fieldSizeY;
    int fieldSizeX = info->fieldSizeX;
    int left_f = info->left;
    int top_f = info->top;
    COORD cursor_p = {0, 0};

    wchar_t *screen0 = malloc(width * height * sizeof(wchar_t));
    for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			screen0[y*width + x] = L' ';
		}

    PrintText("Select", screen0, abc_point, sizeof("Select"), 42, 32);

    short left = GetAsyncKeyState(VK_LEFT);
    short right = GetAsyncKeyState(VK_RIGHT);
    short space = GetAsyncKeyState(VK_RIGHT);
    short pos = 0;
    short enter = GetAsyncKeyState(VK_RETURN);
    while(enter == 0){
        left = GetAsyncKeyState(VK_LEFT);
        right = GetAsyncKeyState(VK_RIGHT);
        enter = GetAsyncKeyState(VK_RETURN);

        PrintText("Random", screen0, abc_point, sizeof("Random"), 24, 25);
        PrintText("Create", screen0, abc_point, sizeof("Create"), 62, 25);

        if (right != 0)
            pos = 1;

        if (left != 0)
            pos = 0;

        if (pos){
            PrintBorders(screen0, height, width, 5, 35, 61, 24);
        }
        else{
            PrintBorders(screen0, height, width, 5, 35, 23, 24);
        }

        PrintScreen(screen0, console, logs, coordBufCoord, lenght, height, width);
        ClearScreen(screen0, height, width, 23, 24, 106, 31, logs, lenght);
    }

    if (pos){
        Sleep(500);
        while ( getchar() != '\n' );
        ClearScreen(screen0, height, width, 0, 0, width, height, logs, lenght);
        PrintBorders(screen0, height, width, fieldSizeY, fieldSizeX, left_f, top_f);
        enter = 0;
        short up = GetAsyncKeyState(VK_UP);
        short down = GetAsyncKeyState(VK_DOWN);
        short remove = GetAsyncKeyState(VK_DELETE);
        int x_pos = 0;
        int y_pos = 0;
        int counter = 0;

        PrintText("Enter:leave", screen0, abc_point, sizeof("Enter:leave"), 49, 1);
        PrintText("Move:", screen0, abc_point, sizeof("Move:"), 49, 7);
        PrintText("Up,Down,", screen0, abc_point, sizeof("Up,Down,"), 49, 13);
        PrintText("Left,Right", screen0, abc_point, sizeof("Left,Right"), 49, 19);
        PrintText("Mark:", screen0, abc_point, sizeof("Mark:"), 49, 25);
        PrintText("space", screen0, abc_point, sizeof("space"), 49, 31);
        PrintText("Delete:", screen0, abc_point, sizeof("Delete:"), 49, 37);
        PrintText("key del", screen0, abc_point, sizeof("key del"), 49, 43);

        while(enter == 0){
            enter = GetAsyncKeyState(VK_RETURN);
            left = GetAsyncKeyState(VK_LEFT);
            right = GetAsyncKeyState(VK_RIGHT);
            space = GetAsyncKeyState(VK_SPACE);
            up = GetAsyncKeyState(VK_UP);
            down = GetAsyncKeyState(VK_DOWN);
            remove = GetAsyncKeyState(VK_DELETE);

            if (counter % 70 == 0){
                if (left != 0 && x_pos > 0)
                    x_pos--;
                if (right != 0 && x_pos < fieldSizeX - 1)
                    x_pos++;
                if (up != 0 && y_pos > 0)
                    y_pos--;
                if (down != 0 && y_pos < fieldSizeY - 1)
                    y_pos++;

                counter = 0;
            }

            counter++;

            if (space != 0)
                cells[y_pos * fieldSizeX + x_pos] = 1;

            if (remove != 0)
                cells[y_pos * fieldSizeX + x_pos] = 0;

            PrintCells(cells, screen0, height, width, fieldSizeY, fieldSizeX, left_f, top_f);
            screen0[(y_pos + left_f + 1)*width + x_pos + left_f + 1] = L'A';
            PrintScreen(screen0, console, logs, coordBufCoord, lenght, height, width);
            ClearScreen(screen0, height, width, left_f + 1, top_f + 1, fieldSizeX + left_f, fieldSizeY + top_f, logs, lenght);
            SetConsoleCursorPosition(*console, cursor_p);
        }

        while ( getchar() != '\n' );
    }
    else{
        RandomCells(cells, fieldSizeY, fieldSizeX);
        while ( getchar() != '\n' );
        Sleep(500);
    }

    SetConsoleCursorPosition(*console, cursor_p);

    free(screen0);
}

//Print screen
int PrintScreen(wchar_t *screen, HANDLE * console, Logs *logs, COORD coordBufCoord, int *lenght, int height, int width){
    DWORD dwBytesWritten = 0;

    if (WriteConsoleOutputCharacterW(*console, screen, width * height, coordBufCoord, &dwBytesWritten) == 0){
        logs = realloc(logs, (*lenght + 1) * sizeof(Logs));
        logs[*lenght] = ERROR_OF_WRITING_OUTPUT;
        *lenght += 1;

        return 0;
    }

    return 1;
}

//Initialize screen
void InitScreen(wchar_t *screen, int height, int width){
    for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			screen[y*width + x] = L' ';
		}
}

//Print cells
void PrintCells(short *cells, wchar_t *screen, int height_screen, int width_screen, int height_field, int width_field, int left, int top){
    for(int y = 0; y < height_field; y++)
        for(int x = 0; x < width_field; x++)
            if  (cells[y*height_field + x] != 0 && left + x + 1 < width_screen && top + y + 1 < height_screen)
                screen[(top + y + 1)*width_screen + left + 1 + x] = L'#';
            else
                screen[(top + y + 1)*width_screen + left + 1 + x] = L' ';
}

//Print borders
void PrintBorders(wchar_t* screen, int height_screen, int width_screen, int height_field, int width_field, int left, int top){
    if (left + width_field < width_screen - 1 && top + height_field < height_screen - 1){
        screen[top*width_screen + left] = L'╔';
        screen[(top + height_field + 1)*width_screen + left] = L'╚';
        screen[top*width_screen + left + width_field + 1] = L'╗';
        screen[(top + height_field + 1)*width_screen + left + width_field + 1] = L'╝';

        for (int i = 1; i < width_field + 1; i++){
            screen[top*width_screen + left + i] = L'═';
            screen[(top + height_field + 1)*width_screen + left + i] = L'═';
        }

        for (int i = 1; i < height_field + 1; i++){
            screen[(top + i)*width_screen + left] = L'║';
            screen[(top + i)*width_screen + left + width_field + 1] = L'║';
        }
    }
}

//Clear Screen
void ClearScreen(wchar_t *screen, int height, int width, int left, int top, int right, int bottom, Logs *logs, int *lenght){
    if (right > left && bottom > top && right <= width && bottom <= height){
        for (int y = top; y < bottom; y++)
            for (int x = left; x < right; x++)
                screen[y*width + x] = L' ';
    }
    else{
        logs = realloc(logs, (*lenght + 1) * sizeof(Logs));
        logs[*lenght] = ERROR_OF_CLEAR_SCREEN;
        *lenght += 1;
    }
}

//Checking logs for errors
int CheckLogs(Logs *logs, int lenght){
    for (int i = 0; i < lenght; i++)
        if (logs[i] != OK)
            return 0;

    return 1;
}

//Checking cell for life possibility
short CheckerOfLife(int x, int y, short *cells, int fieldSizeX, int fieldSizeY, int life_min, int life_rise, int life_max){
    short counter = 0;

    if (x > 0 && x < fieldSizeX - 1 && y > 0 && y < fieldSizeY - 1){
        if (cells[(y - 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x - 1] != 0)
            counter += 1;
    }
    else if (x > 0 && x < fieldSizeX - 1 && y == 0){
        if (cells[(fieldSizeY - 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(fieldSizeY - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(fieldSizeY - 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x - 1] != 0)
            counter += 1;
    }
    else if (x > 0 && x < fieldSizeX - 1 && y == fieldSizeY - 1){
        if (cells[(y - 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x - 1] != 0)
            counter += 1;
    }
    else if(y > 0 && y < fieldSizeY - 1 && x == 0){
        if (cells[(y - 1)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;
    }
    else if(y > 0 && y < fieldSizeY - 1 && x == fieldSizeX - 1){
        if (cells[(y - 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x - 1] != 0)
            counter += 1;
    }
    else if (x == 0 && y == 0){
        if (cells[(fieldSizeY - 1)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;

        if (cells[(fieldSizeY - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(fieldSizeY - 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;
    }
    else if(x == 0 && y == fieldSizeY - 1){
        if (cells[(0)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x + 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + fieldSizeX - 1] != 0)
            counter += 1;
    }
    else if (x == fieldSizeX - 1 && y == 0){
        if (cells[(fieldSizeY - 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(fieldSizeY - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(fieldSizeY - 1)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y + 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x - 1] != 0)
            counter += 1;
    }
    else if (x == fieldSizeX - 1 && y == fieldSizeY - 1){
        if (cells[(y - 1)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(y - 1)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + 0] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x] != 0)
            counter += 1;

        if (cells[(0)*fieldSizeX + x - 1] != 0)
            counter += 1;

        if (cells[(y)*fieldSizeX + x - 1] != 0)
            counter += 1;
    }

    if (cells[y*fieldSizeX + x] != 0){
        if (counter >= life_min && counter <= life_max)
            return 1;
        else
            return 0;
    }
    else{
        if (counter == life_rise)
            return 1;
        else
            return 0;
    }
}

//Update Cells
void UpdateCells(short *cells, int fieldSizeX, int fieldSizeY, int life_min, int life_rise, int life_max){
    short *buf_cells = malloc(fieldSizeX*fieldSizeY*sizeof(short));

    for(int y = 0; y < fieldSizeY; y++)
        for(int x = 0; x < fieldSizeX; x++)
            buf_cells[y*fieldSizeX +x] = CheckerOfLife(x, y, cells, fieldSizeX, fieldSizeY, life_min, life_rise, life_max);

    for(int y = 0; y < fieldSizeY; y++)
        for(int x = 0; x < fieldSizeX; x++)
            cells[y*fieldSizeX +x] = buf_cells[y*fieldSizeX +x];

    free(buf_cells);
}

//LoadData
void LoadData(wchar_t **abc_point, wchar_t **digits, Logs *logs, int *lenght){
    FILE *f;
	unsigned char buffer1[10*15];
	unsigned char buffer2[33*25];
	int n = 0;

    f = fopen("digits.bin", "rb");
	if (f)
	{
		n = fread(buffer1, 10*15, 1, f);
	}

	if (n == 0){
        logs = realloc(logs, (*lenght + 1) * sizeof(Logs));
        logs[*lenght] = ERROR_OF_LOADING_DIGITS;
        *lenght += 1;
	}

	//digits = malloc(10*sizeof(wchar_t *));
	for (int i = 0; i < 10; i++){
        //digits[i] = malloc(15*sizeof(wchar_t));
        for (int j = 0; j < 15; j++)
            if (buffer1[15*i + j] == 1)
                digits[i][j] = L'#';
            else
                digits[i][j] = L' ';
	}

    close(f);

    n = 0;
    f = fopen("abc_points.bin", "rb");
	if (f)
	{
		n = fread(buffer2, 33*25, 1, f);
	}

	if (n == 0){
        logs = realloc(logs, (*lenght + 1) * sizeof(Logs));
        logs[*lenght] = ERROR_OF_LOADING_ABC;
        *lenght += 1;
	}

    //abc_point = malloc(33*sizeof(wchar_t *));
	for (int i = 0; i < 33; i++){
        //abc_point[i] = malloc(25*sizeof(wchar_t));
        for (int j = 0; j < 25; j++)
            if (buffer2[25*i + j] == 1)
                abc_point[i][j] = L'#';
            else
                abc_point[i][j] = L' ';
	}

    close(f);
}

//Print Character
void PrintCharacter(wchar_t *character, wchar_t *screen, int x, int y, int width, int height){
    for (int y_ = 0; y_ < height; y_++)
        for (int x_ = 0; x_ < width; x_++)
            screen[(y + y_)*120 + (x + x_)] = character[y_*width + x_];
}

//Print Integer
void PrintInteger(int i, wchar_t *screen, wchar_t **digits, int x, int y){
    int size = 0;

    if (i == 0)
        size++;

    int buf = i;
    while (buf > 0){
        buf = buf / 10;
        size++;
    }

    for (int j = 0; j < size; j++){
        PrintCharacter(digits[i % 10], screen, 4*(size - 1 - j) + x, y, 3, 5);
        i = i / 10;
    }
}

//Print Text
void PrintText(char *str, wchar_t *screen, wchar_t **abc_point, int lenght, int x, int y){
    for (int i = 0; i < lenght; i++){
        if (str[i] >= 65 && str[i] <= 90)
            PrintCharacter(abc_point[str[i] - 65], screen, 6*i + x, y, 5, 5);
        if (str[i] >= 97 && str[i] <= 122)
            PrintCharacter(abc_point[str[i] - 97], screen, 6*i + x, y, 5, 5);
        if (str[i] == 32)
            PrintCharacter(abc_point[32], screen, 6*i + x, y, 5, 5);
        if (str[i] == 46)
            PrintCharacter(abc_point[26], screen, 6*i + x, y, 5, 5);
        if (str[i] == 44)
            PrintCharacter(abc_point[29], screen, 6*i + x, y, 5, 5);
        if (str[i] == 33)
            PrintCharacter(abc_point[27], screen, 6*i + x, y, 5, 5);
        if (str[i] == 63)
            PrintCharacter(abc_point[28], screen, 6*i + x, y, 5, 5);
        if (str[i] == 58)
            PrintCharacter(abc_point[30], screen, 6*i + x, y, 5, 5);
        if (str[i] == 59)
            PrintCharacter(abc_point[31], screen, 6*i + x, y, 5, 5);
    }
}

int LifeCount(short *cells, int fieldSizeX, int fieldSizeY){
    int count = 0;

    for (int y = 0; y < fieldSizeY; y++)
        for (int x = 0; x < fieldSizeX; x++)
            if (cells[y * fieldSizeX + x] != 0)
                count++;

    return count;
}

int main()
{
    int logs_size = 2;
    const int width_screen = 120;
    const int height_screen = 55;
    const int font_width = 12;
    const int font_height = 12;
    const int new_font_width = 10;
    const int new_font_height = 12;
    const int fieldSizeX = 45;
    const int fieldSizeY = 45;
    const int field_left = 1;
    const int field_top = 1;
    const int life_min = 2;
    const int life_rise = 3;
    const int life_max = 3;

    HANDLE console;
    COORD coordBufCoord = {0, 0};
    Logs *logs = malloc(logs_size * sizeof(Logs));
    wchar_t *screen = malloc(width_screen * height_screen * sizeof(wchar_t));
    short * cells = malloc(fieldSizeX * fieldSizeY * sizeof(short));

    wchar_t **abc_point = malloc(33*sizeof(wchar_t *));
    for (int i = 0; i < 33; i++)
        abc_point[i] = malloc(25*sizeof(wchar_t));

    wchar_t **digits = malloc(10*sizeof(wchar_t *));
    for (int i = 0; i < 10; i++)
        digits[i] = malloc(15*sizeof(wchar_t));


    LoadData(abc_point, digits, logs, &logs_size);
    InitConsole(&console, logs,  height_screen, width_screen, font_height, font_width);
    InitScreen(screen, height_screen, width_screen);
    InitCells(cells, fieldSizeY, fieldSizeX);

    PrintBorders(screen, height_screen, width_screen, fieldSizeY, fieldSizeX, field_left, field_top);
    //RandomCells(cells, fieldSizeY, fieldSizeX);
    PrintCells(cells, screen, height_screen, width_screen, fieldSizeY, fieldSizeX, field_left, field_top);

    Info info;
    info.cells = cells;
    info.console = &console;
    info.coordBufCoord = coordBufCoord;
    info.fieldSizeX = fieldSizeX;
    info.fieldSizeY = fieldSizeY;
    info.height = height_screen;
    info.left = field_left;
    info.lenght = &logs_size;
    info.life_max = life_max;
    info.life_min = life_min;
    info.life_rise = life_rise;
    info.screen = screen;
    info.top = field_top;
    info.width = width_screen;
    info.digits = digits;
    info.abc_point = abc_point;

    if (CheckLogs(logs, logs_size)){
        MainMenu(&info);
        MainLoop(&info);
    }
    else{
        PrintLogs(logs, logs_size);
    }

    ClearScreen(screen, height_screen, width_screen, 0, 0, width_screen, height_screen, logs, &logs_size);
    PrintScreen(screen, &console, logs, coordBufCoord, &logs_size, height_screen, width_screen);
    InitConsole(&console, logs, height_screen, width_screen, new_font_height, new_font_width);

    if (!CheckLogs(logs, logs_size)){
        PrintLogs(logs, logs_size);
        printf("Press enter to leave...\n");
        getchar();
    }
    else{
        printf("Press enter to leave...\n");
        getchar();
    }

    free(logs);
    free(screen);
    free(cells);

    for (int i = 0; i < 10; i++)
        free(digits[i]);
    for (int i = 0; i < 33; i++)
        free(abc_point[i]);
    free(digits);
    free(abc_point);

    return 0;
}
