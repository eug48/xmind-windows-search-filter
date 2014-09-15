XMind Windows Search Filter
================================


This project builds a DLL that enables Windows Search to index the full-text content XMind mindmaps.

This DLL works by using an XSL transform to convert the mindmap's content.xml into HTML. The HTML is then passed to Microsoft's built-in HTML search filter which does most of the work ;)


Building/Compiling
------------------

 - Currently this project has been compiled with Visual Studio 2013 Ultimate.
 - It should be possible to use the free Express for Windows Desktop edition along with the ATL library from the WDK but this hasn't been tested yet.
 - Pre-compiled DLLs are available in the GitHub release.


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

Debug versions (with dbg in the filename) will write error information via OutputDebugString. To see this output use a tool like [DebugView].

There is also a mechanism to manually transform a mindmap into HTML:

```
c:\windows\SysWOW64\rundll32.exe "d:\test\xmind-search-filter-dbg-x86.dll",xmind2html c:\users\user\documents\mind maps\test.xmind
```
*Note:* Don't put quotes around the filename when using rundll32


[DebugView]:http://technet.microsoft.com/en-us/sysinternals/bb896647.aspx


Limitations
-------------------

 - This search filter is currently using an XSL file from XMorgDown https://github.com/sky-y/xmorgdown/blob/master/content.xsl
It seems to support only 6 levels of topics and doesn't seem to handle multi-line notes well.
 - No doubt other things I'm not aware of yet as this hasn't been extensively tested.
