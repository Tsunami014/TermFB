@echo off
termfb.exe
for /f "delims=" %%d in (C:\temp\termfb_dir.txt) do cd /d "%%d"
del C:\temp\termfb_dir.txt

