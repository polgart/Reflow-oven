# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles\RO_PC_com_v1.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\RO_PC_com_v1.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\RO_PC_com_v1.dir\flags.make

CMakeFiles\RO_PC_com_v1.dir\main.cpp.obj: CMakeFiles\RO_PC_com_v1.dir\flags.make
CMakeFiles\RO_PC_com_v1.dir\main.cpp.obj: ..\main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/RO_PC_com_v1.dir/main.cpp.obj"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\RO_PC_com_v1.dir\main.cpp.obj /FdCMakeFiles\RO_PC_com_v1.dir\ /FS -c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\main.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RO_PC_com_v1.dir/main.cpp.i"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe > CMakeFiles\RO_PC_com_v1.dir\main.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\main.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RO_PC_com_v1.dir/main.cpp.s"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\RO_PC_com_v1.dir\main.cpp.s /c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\main.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.obj: CMakeFiles\RO_PC_com_v1.dir\flags.make
CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.obj: ..\SerialPort.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/RO_PC_com_v1.dir/SerialPort.cpp.obj"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.obj /FdCMakeFiles\RO_PC_com_v1.dir\ /FS -c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\SerialPort.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RO_PC_com_v1.dir/SerialPort.cpp.i"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe > CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\SerialPort.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RO_PC_com_v1.dir/SerialPort.cpp.s"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.s /c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\SerialPort.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.obj: CMakeFiles\RO_PC_com_v1.dir\flags.make
CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.obj: ..\dataParser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/RO_PC_com_v1.dir/dataParser.cpp.obj"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.obj /FdCMakeFiles\RO_PC_com_v1.dir\ /FS -c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\dataParser.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RO_PC_com_v1.dir/dataParser.cpp.i"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe > CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\dataParser.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RO_PC_com_v1.dir/dataParser.cpp.s"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.s /c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\dataParser.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.obj: CMakeFiles\RO_PC_com_v1.dir\flags.make
CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.obj: ..\stateMachine.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/RO_PC_com_v1.dir/stateMachine.cpp.obj"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.obj /FdCMakeFiles\RO_PC_com_v1.dir\ /FS -c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\stateMachine.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RO_PC_com_v1.dir/stateMachine.cpp.i"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe > CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\stateMachine.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RO_PC_com_v1.dir/stateMachine.cpp.s"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.s /c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\stateMachine.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.obj: CMakeFiles\RO_PC_com_v1.dir\flags.make
CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.obj: ..\socketHandler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/RO_PC_com_v1.dir/socketHandler.cpp.obj"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.obj /FdCMakeFiles\RO_PC_com_v1.dir\ /FS -c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\socketHandler.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RO_PC_com_v1.dir/socketHandler.cpp.i"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe > CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\socketHandler.cpp"
<<

CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RO_PC_com_v1.dir/socketHandler.cpp.s"
	C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.s /c "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\socketHandler.cpp"
<<

# Object files for target RO_PC_com_v1
RO_PC_com_v1_OBJECTS = \
"CMakeFiles\RO_PC_com_v1.dir\main.cpp.obj" \
"CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.obj" \
"CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.obj" \
"CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.obj" \
"CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.obj"

# External object files for target RO_PC_com_v1
RO_PC_com_v1_EXTERNAL_OBJECTS =

RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\main.cpp.obj
RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\SerialPort.cpp.obj
RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\dataParser.cpp.obj
RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\stateMachine.cpp.obj
RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\socketHandler.cpp.obj
RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\build.make
RO_PC_com_v1.exe: CMakeFiles\RO_PC_com_v1.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable RO_PC_com_v1.exe"
	"C:\Program Files\JetBrains\CLion 2020.3.3\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\RO_PC_com_v1.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\mt.exe --manifests  -- C:\PROGRA~2\MICROS~3\2019\BUILDT~1\VC\Tools\MSVC\1427~1.291\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\RO_PC_com_v1.dir\objects1.rsp @<<
 /out:RO_PC_com_v1.exe /implib:RO_PC_com_v1.lib /pdb:"C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\RO_PC_com_v1.pdb" /version:0.0  /machine:X86 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\RO_PC_com_v1.dir\build: RO_PC_com_v1.exe

.PHONY : CMakeFiles\RO_PC_com_v1.dir\build

CMakeFiles\RO_PC_com_v1.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\RO_PC_com_v1.dir\cmake_clean.cmake
.PHONY : CMakeFiles\RO_PC_com_v1.dir\clean

CMakeFiles\RO_PC_com_v1.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1" "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1" "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug" "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug" "C:\Users\EDU_OBOS_5057\Documents\Reflow oven\Design\SW\RO_PC_com_v1\cmake-build-debug\CMakeFiles\RO_PC_com_v1.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles\RO_PC_com_v1.dir\depend

