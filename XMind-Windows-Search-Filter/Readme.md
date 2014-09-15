XMind Windows Search Filter
================================


This project builds a DLL that enables Windows Search to index the full-text content XMind mindmaps.

This DLL works by using an XSL transform to convert the mindmap's content.xml into HTML.

Installing
-------------

Use regsvr32 to register the DLL. This will update the Windows registry.


Register the DLL that is appropriate for your system (x86 for 32-bit, x64 for 64-bit).
```
regsvr32 "d:\test\xmind-search-filter-x64.dll"
```

For debugging register the debug version:
```
regsvr32 "d:\test\xmind-search-filter-dbg-x64.dll"
```


Testing
-------------

Debug builds will write error information via OutputDebugString. To see this output use a tool like [DebugView].

There is also a mechanism to manually transform a mindmap into HTML:

```
c:\windows\SysWOW64\rundll32.exe "d:\test\xmind-search-filter-dbg-x86.dll",xmind2html c:\users\user\documents\mind maps\test.xmind
```
*Note:* Don't put quotes around the filename when using rundll32


[DebugView]:http://technet.microsoft.com/en-us/sysinternals/bb896647.aspx

