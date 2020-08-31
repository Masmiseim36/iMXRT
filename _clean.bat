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

rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1011_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1011_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1015_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1015_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1020_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1020_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1050_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1050_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1060_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1060_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1064_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1064_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1170_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM7 MIMXRT1170_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM4 MIMXRT1170_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM4 MIMXRT1170_Release"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM33 MIMXRT600_Debug"
rmdir /s /q "./targets/iMXRT/Loader2/Loader_CM33 MIMXRT600_Release"


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
del /f /q *_Debug.elf
cd ..
cd ..
cd ..
