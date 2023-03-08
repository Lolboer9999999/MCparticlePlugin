#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>

char* open_file_dialog() {
    static char filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Object Files (*.obj)\0*.obj\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "obj";

    if (GetOpenFileName(&ofn)) {
        return filename;
    }
    else {
        return NULL;
    }
}

// function to display a file save dialog and return the selected file path
char* save_file_dialog() {
    static char filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "YAML Files (*.yml)\0*.yml\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "yml";

    if (GetSaveFileName(&ofn)) {
        return filename;
    }
    else {
        return NULL;
    }
}

#define SELECT_BUTTON_ID 1
#define START_BUTTON_ID 2
#define SAVE_BUTTON_ID 3
#define CUSTOMBORDER (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

char* buffer;
char* new_buffer;
long input_file_size;
HWND startProcessButton;
HWND saveButton;
HWND textbox;
HWND textbox2;
HWND textbox3;
int j = 0;
int numberOfVectors = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    // Register the window class
    const char CLASS_NAME[] = "MyClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    HBRUSH hBrush = CreateSolidBrush(RGB(64, 64, 64));  // Create dark grey brush
    wc.hbrBackground = hBrush;  // Set background brush of window class to dark grey brush

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        ".obj to vectorShape",                    // Window text
        CUSTOMBORDER,         // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 430, 240,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    // Remove the resizable flag
    style &= ~WS_SIZEBOX;
    // Set the new window style
    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    if (hwnd == NULL)
    {
        return 0;
    }

    // Create a button
    HWND selectButton = CreateWindow(
        "BUTTON",                   // Button class
        "select file",                 // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Button style
        30,                         // X position
        10,                         // Y position
        100,                        // Button width
        25,                         // Button height
        hwnd,                       // Parent window
        (HMENU)SELECT_BUTTON_ID,           // No menu
        hInstance,                  // Instance handle
        NULL                        // No additional data
    );

    startProcessButton = CreateWindow(
        "BUTTON",                   // Button class
        "start process",                 // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Button style
        155,                         // X position
        10,                         // Y position
        100,                        // Button width
        25,                         // Button height
        hwnd,                       // Parent window
        (HMENU)START_BUTTON_ID,           // No menu
        hInstance,                  // Instance handle
        NULL                        // No additional data
    );

    saveButton = CreateWindow(
        "BUTTON",                   // Button class
        "save file",                 // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Button style
        270,                         // X position
        10,                         // Y position
        100,                        // Button width
        25,                         // Button height
        hwnd,                       // Parent window
        (HMENU)SAVE_BUTTON_ID,           // No menu
        hInstance,                  // Instance handle
        NULL                        // No additional data
    );

    textbox = CreateWindowEx(
    0,  // Extended window style
    TEXT("STATIC"),  // Window class
    TEXT("Selected File: none"),  // Text
    WS_VISIBLE | WS_CHILD,  // Window style
    10,  // X position
    100,  // Y position
    405,  // Width
    90,  // Height
    hwnd,  // Parent window handle
    NULL,  // Menu handle
    hInstance,  // Application instance handle
    NULL  // Window creation data
    );

    textbox2 = CreateWindowEx(
    0,  // Extended window style
    TEXT("STATIC"),  // Window class
    TEXT("select file to convert"),  // Text
    WS_VISIBLE | WS_CHILD,  // Window style
    10,  // X position
    50,  // Y position
    260,  // Width
    40,  // Height
    hwnd,  // Parent window handle
    NULL,  // Menu handle
    hInstance,  // Application instance handle
    NULL  // Window creation data
    );
    textbox3 = CreateWindowEx(
    0,  // Extended window style
    TEXT("STATIC"),  // Window class
    TEXT("#Vectors generated:\n0"),  // Text
    WS_VISIBLE | WS_CHILD,  // Window style
    280,  // X position
    50,  // Y position
    135,  // Width
    40,  // Height
    hwnd,  // Parent window handle
    NULL,  // Menu handle
    hInstance,  // Application instance handle
    NULL  // Window creation data
    );

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    EnableWindow(startProcessButton, FALSE);  // Disable the button
    EnableWindow(saveButton, FALSE);  // Disable the button

    // Run the message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == SELECT_BUTTON_ID) { 
                char* input_file_path = open_file_dialog();
                if (input_file_path == NULL) {
                    printf("No file selected.\n");
                    SetWindowText(textbox2, "!!SELECTING FILE FAILED!!\npress select to try again");
                    return 1;
                }
                FILE* input_file = fopen(input_file_path, "rb");
                char input_file_path_array[261];
                strcpy(input_file_path_array,input_file_path);
                char label_text[MAX_PATH + 20];  // Allocate space for the label text
                strcpy(label_text, "Selected file: ");  // Copy the initial label text
                strcat(label_text, input_file_path_array);  // Append the file path to the label text
                SetWindowText(textbox, label_text);
                SetWindowText(textbox2, "press start to convert");
                SetWindowText(textbox3,"#Vectors generated:\n0");
                fseek(input_file, 0, SEEK_END);
                input_file_size = ftell(input_file);
                const long filesize = input_file_size;
                rewind(input_file);

                buffer = (char*) malloc(sizeof(char) * input_file_size);

                size_t size = fread(buffer, 1, input_file_size, input_file);
                fclose(input_file);
                EnableWindow(startProcessButton, TRUE);
                EnableWindow(saveButton, FALSE);
            }
            else if(LOWORD(wParam) == START_BUTTON_ID){
                int* indices = NULL;
                int index_count = 0;
                SetWindowText(textbox2, "converting please wait...");
                for (int i = 0; buffer[i]; ++i) {
                    if (buffer[i] == '\n') {
                        ++index_count;
                        indices = (int*)realloc(indices, index_count * sizeof(int));
                        if (indices == NULL) {
                            printf("Error: Out of memory.\n");
                            return 1;
                        }
                        indices[index_count - 1] = i;
                    }
                }
                int firstV = 0;

                if (buffer[0] != 'v' || buffer[1] != ' ') {
                    for(int i = 0; i < indices[0]; i++){
                        buffer[i] = '\0';
                    }
                }
                for(int i = 0; i < index_count; i++){
                    if(buffer[indices[i]+1] != 'v' || buffer[indices[i]+2] != ' '){
                        for(int j = indices[i]+1; j < indices[i+1]; j++){
                            if(j < input_file_size){
                                buffer[j] = '\0';
                            }
                        }
                    }
                }
                numberOfVectors = 0;
                for(int i = 0; i < input_file_size; i++) {
                    if (buffer[i] == 'v' && buffer[i+1] == ' ') {
                        numberOfVectors++;
                        if(firstV == 0){
                            buffer[i] = '\0';
                            buffer[i+1] = '\0';
                            firstV = 1;
                        }
                        else{
                            buffer[i] = ';';
                            buffer[i+1] = '\0';
                        }
                    } else if (buffer[i] == ' ') {
                        buffer[i] = ',';
                    }
                }
                j = 0;
                for (int i = 0; i < input_file_size; i++) {
                    if (buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r' && buffer[i] != ' ') {
                        j++;
                    }
                }

                new_buffer = (char*) calloc(j+1, sizeof(char)); // Allocate space for new buffer
                int k = 0;  // Initialize index for new buffer

                // Copy over all non-null characters from original buffer to new buffer
                for (int i = 0; i < input_file_size; i++) {
                    if (buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r') {
                        new_buffer[k++] = buffer[i];
                    }
                }
                free(buffer);  // Free original buffer
                EnableWindow(saveButton, TRUE);
                EnableWindow(startProcessButton, FALSE);
                SetWindowText(textbox2, "converting succesful\npress save to finish");
                char buffer[100];
                sprintf(buffer, "#Vectors generated:\n%d", numberOfVectors);
                SetWindowText(textbox3, buffer);            
            }
            else if(LOWORD(wParam) == SAVE_BUTTON_ID){
                SetWindowText(textbox2, "saving please wait...");
                char* output_file_path = save_file_dialog();

                if (output_file_path == NULL) {
                    SetWindowText(textbox2, "!!SAVING FAILED!!\npress save to try again or select to start over");
                    printf("No output file selected.\n");
                    return 1;
                }

                FILE* output_file = fopen(output_file_path, "wb");

                if (output_file == NULL) {
                    SetWindowText(textbox2, "!!SAVING FAILED!!\npress save to try again or select to start over");
                    printf("Error creating output file.\n");
                    return 1;
                }

                fwrite(new_buffer, 1, (j+1)*sizeof(char), output_file);
                fclose(output_file);
                free(new_buffer);
                EnableWindow(saveButton, FALSE);
                SetWindowText(textbox2, "saved succesfully\nyou can press select to start over");
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;    
        default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
