del *.bak /S
rmdir /s /q ".\targets\iMXRText\Loader2\out"

cd "./targets/iMXRText/Loader2/"
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
