#include <windows.h>
#include <xs/pal.h>

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	return MainEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

#ifdef XSDT_CONSOLE
int main(int argc, char *argv[])
{
	LPSTR cmdline = GetCommandLineA();
	return MainEntry(GetModuleHandle(NULL), NULL, cmdline, SW_SHOWNORMAL);
}
#endif
