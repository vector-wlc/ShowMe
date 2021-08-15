#include "Memory.h"

HWND g_hwnd = nullptr;
HANDLE g_handle = nullptr;
uintptr_t g_pvzbase = 0;    //��ַ
uintptr_t g_mainobject = 0; //��Ϸ�����ַ

// ��ȡ�ڴ��ж��Ƿ�Ӧ����ʾ��Ϣ
// ���� true : ������ʾ��Ϣ
// ���� false : ������ʾ��Ϣ
bool IsDisplayed()
{
    if (GameUi() != 3) {
        g_hwnd = FindWindowW(L"MainWindow", L"Plants vs. Zombies");
        if (GetForegroundWindow() != g_hwnd) { // �� pvz ���ڶ���ʱ�ر���ʾ
            return false;
        }
        DWORD pid;
        GetWindowThreadProcessId(g_hwnd, &pid);
        if (g_handle != nullptr) {
            CloseHandle(g_handle);
        }
        g_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
        //��ȡ��Ϸ��ַ
        g_pvzbase = ReadMemory<uintptr_t>(0x006a9ec0);
    }
    g_mainobject = ReadMemory<uintptr_t>(g_pvzbase + 0x768);
    return GameUi() == 3 && IsMouseInPvZ();
}

/* place */
PlaceMemory::PlaceMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0x11C);
    index = 0;
}

PlaceMemory::PlaceMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0x11C);
    index = i;
}

void PlaceMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0x11C);
}

/*  seed  */
SeedMemory::SeedMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0x144);
    index = 0;
}

SeedMemory::SeedMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0x144);
    index = i;
}

void SeedMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0x144);
}

/*  plant  */
PlantMemory::PlantMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0xAC);
    index = 0;
}

PlantMemory::PlantMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0xAC);
    index = i;
}

void PlantMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0xAC);
}

/* zombie */
ZombieMemory::ZombieMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0x90);
    index = 0;
}

ZombieMemory::ZombieMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0x90);
    index = i;
}

void ZombieMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0x90);
}

ItemMemory::ItemMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0xE4);
    index = 0;
}

ItemMemory::ItemMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0xE4);
    index = i;
}

void ItemMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0xE4);
}
