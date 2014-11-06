echo Signing Midas.exe with Test Certificate - For Internal Use Only - JHH

set platform=%1
set config=%2

echo platform
echo config

if (config == "Debug") True
(
    echo "11Debug"
    call "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x86\\signtool.exe" sign /v /s PrivateCertStore /n "Test Certificate - For Internal Use Only - Midasx86" /t "http://timestamp.verisign.com/scripts/timestamp.dll" ../Debug/Midas.exe

    copy ..\Debug\Midas.exe "C:\\Program Files (x86)" /Y /B
)
else
(
    echo "DebugElse"
    call "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x64\\signtool.exe" sign /v /s PrivateCertStore /n "Test Certificate - For Internal Use Only - Midasx64" /t "http://timestamp.verisign.com/scripts/timestamp.dll" ../Debug/Midas.exe

    copy ..\Debug\Midas.exe "C:\\Program Files" /Y /B
)

echo done.

pause