#pragma once


#ifdef _DEBUG
#ifdef ATLTRACE 
#undef ATLTRACE
#undef ATLTRACE2

#define ATLTRACE CustomTrace
#define ATLTRACE2 ATLTRACE
#endif // ATLTRACE
#endif // _DEBUG

void CustomTrace(const wchar_t* format, ...);
void CustomTrace(const char* format, ...);