del *.bak /S
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

cd "./targets/iMXRT/Loader/"
del /f /q *.map
cd ..
cd ..
cd ..
