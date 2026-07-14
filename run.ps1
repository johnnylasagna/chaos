cmake --build build
.\build\chaos-assembler.exe .\files\boot.chasm
.\build\chaos-assembler.exe .\files\kernel.chasm
.\build\chaos-formatter.exe .\img\disk.img
.\build\chaos-vm.exe img/disk.img                  