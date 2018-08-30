#pragma once

#define DLLEXPORT _declspec(dllexport)



extern "C"{	
	DLLEXPORT void __stdcall SFVersion(char* buffer, int size);
	DLLEXPORT float __stdcall GetProgress(int* check);
	DLLEXPORT int __stdcall SFCompress(char* srcFile, char* dstFile);
	DLLEXPORT int __stdcall SFDecompress(char* srcFile, char* dstFile);
	//DLLEXPORT void __stdcall SetProgressFun(void (*ptr)(float, int));

}
