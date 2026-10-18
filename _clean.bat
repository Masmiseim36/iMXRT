del *.bak /S
rmdir /s /q ".\targets\iMXRT\Loader2\out"

cd "./targets/iMXRT/Loader2/"
del /f /q *_Debug.elf
del /f /q *.map
cd ..

cd "./Loader2"
del /f /q *.map
del /f /q *.ind
del /f /q *.ld
del /f /q *_Debug.elf
del /f /q *_lto.txt
cd ..
cd ..
cd ..