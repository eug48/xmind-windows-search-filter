XMind Windows Search Filter
================================


This project builds a DLL that enables Windows Search to index the full-text content XMind mindmaps.

This DLL works by using an XSL transform to convert the mindmap's content.xml into HTML. The HTML is then passed to Microsoft's built-in HTML search filter which does most of the work ;)

Installing
-------------

Pre-compiled DLLs are available in the GitHub releases.

Use regsvr32 to register the DLL. This will update the Windows registry. You may need to rebuild the Windows Search index via Indexing Options.

Register the DLL that is appropriate for your system (x86 for 32-bit, x64 for 64-bit).
```
regsvr32 "d:\test\xmind-search-filter-x64.dll"
```

For debugging register the debug version:
```
regsvr32 "d:\test\xmind-search-filter-dbg-x64.dll"
```

Building/Compiling
------------------

Currently this project has been compiled with Visual Studio 2013 Ultimate. You can also try the free Express for Windows Desktop edition but you will need to install the ATL library from the WDK:
 1. Download [WDK 7.1.0]
 2. Install the Build Environments
 3. Open this project in Visual Studio and go to View → Other Windows → Property Manager
 4. Double-click on Microsoft.Cpp.Win32.user and go to VC++ Directories
 5. Add C:\WinDDK\7600.16385.1\inc\atl71 to the Include Directories
 6. Add C:\WinDDK\7600.16385.1\lib\ATL\i386 to the Library Directories
 7. Save and double-click on Microsoft.Cpp.x64.user
 8. Add C:\WinDDK\7600.16385.1\inc\atl71 to the Include Directories
 9. Add C:\WinDDK\7600.16385.1\lib\ATL\amd64 to the Library Directories
 10. Open the file c:\WinDDK\7600.16385.1\inc\atl71\atlbase.h and from the 3 "#pragma section" lines delete ", shared". This fixes some linker warnings (thanks to [Oliver] on StackOverflow).

[WDK 7.1]:http://www.microsoft.com/en-au/download/details.aspx?id=11800



Testing
-------------

Debug versions (with dbg in the filename) will write error information via OutputDebugString. To see this output use a tool like [DebugView]. If the filter is working you should see messages like
```
[xmindfilter] Going to load C:\Users\test\Documents\mindmaps\test.xmind (221472 bytes)
```

There is also a mechanism to manually transform a mindmap into HTML:

```
c:\windows\SysWOW64\rundll32.exe "d:\test\xmind-search-filter-dbg-x86.dll",xmind2html c:\users\user\documents\mind maps\test.xmind
```
*Note:* Don't put quotes around the filename when using rundll32



[DebugView]:http://technet.microsoft.com/en-us/sysinternals/bb896647.aspx

Limitations
-------------------

 - This search filter is currently using an XSL file from XMorgDown (https://github.com/sky-y/xmorgdown/blob/master/content.xsl).
It seems to support only 6 levels of topics and doesn't seem to handle multi-line notes well.
 - Attachments are not indexed.
 - Highlighted previews in Windows Explorer don't appear for an unknown reason.
 - Does the Windows Search index really have to be rebuilt to index existing xmind files?
 - No doubt other things I'm not aware of yet as this hasn't been extensively tested.



Thanks to
--------------------

 - XMind
 - Microsoft
 - sky-y for [XMorgDown]
 - [unzip.cpp] by Jean-Loup Gailly, Mark Adler, the info-zip team, ljw1004 and others
 - [Oliver] from StackOverflow
 - Many others

[XMorgDown]: https://github.com/sky-y/xmorgdown
[unzip.cpp]: http://www.codeproject.com/Articles/7530/Zip-Utils-clean-elegant-simple-C-Win
[Oliver]:http://stackoverflow.com/a/14245255