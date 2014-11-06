rem https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/350ceab8-436b-4ef1-8512-3fee4b470c0a/problem-with-manifest-and-uiaccess-set-to-true?forum=windowsgeneraldevelopmentissues

echo Ensure this is run in the .../cert/ folder.

call "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x86\\makecert.exe" -r -pe -n "CN=Test Certificate - For Internal Use Only - Midasx86" -ss PrivateCertStore testx86certMidas.cer 
call "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x86\\certmgr.exe" -add testx86certMidas.cer -s -r currentuser root

call "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x64\\makecert.exe" -r -pe -n "CN=Test Certificate - For Internal Use Only - Midasx64" -ss PrivateCertStore testx64certMidas.cer
call "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x64\\certmgr.exe" -add testx64certMidas.cer -s -r currentuser root
