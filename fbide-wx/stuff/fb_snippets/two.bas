#define TEST _
    asm : _
        mov eax, 22 ; _
        mov dword ptr [i], eax; _
    end asm
    
        dim i as uinteger
        TEST
        print i
    
sleep
