del *.bak /S
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1011_QuadSPI_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1011_QuadSPI_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1015_QuadSPI_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1015_QuadSPI_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1021_QuadSPI_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1021_QuadSPI_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT105x_QuadSPI_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT105x_QuadSPI_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT105x_HYPER_256KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT105x_HYPER_256KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT105x_EcoXiP_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT105x_EcoXiP_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT106x_HYPER_256KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT106x_HYPER_256KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT106x_QSPI_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT106x_QSPI_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT106x_EcoXiP_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT106x_EcoXiP_4KB_SEC Release"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1064_QSPI_4KB_SEC Debug"
rmdir /s /q "./targets/iMXRT/Loader/iMXRT Loader MIMXRT1064_QSPI_4KB_SEC Release"

rmdir /s /q "./targets\iMXRT\Loader2\out"


rmdir /s /q "./samples/iMXRT/SayHello THUMB Debug"
rmdir /s /q "./samples/iMXRT/SayHello THUMB Release"
rmdir /s /q "./samples/iMXRT/DualCore_M4 THUMB Debug"
rmdir /s /q "./samples/iMXRT/DualCore_M4 THUMB Release"
rmdir /s /q "./samples/iMXRT/DualCore_M7 THUMB Debug"
rmdir /s /q "./samples/iMXRT/DualCore_M7 THUMB Release"

cd "./targets/iMXRT/Loader/"
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
