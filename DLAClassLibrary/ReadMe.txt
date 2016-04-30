========================================================================
    DYNAMIC LINK LIBRARY : DLAClassLibrary Project Overview
========================================================================

This file contains a summary of what you will find in each of the files that
make up the DLAClassLibrary application.

DLAClassLibrary.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

DLAClassLibrary.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

DLAClassLibrary.cpp
    This is the main DLL source file.

DLAClassLibrary.h
    This file contains the ManagedDLA2DContainer and ManagedDLA3DContainer class declarations.

utilities.h
    This is an unmanaged native c++ code file containing useful utilities for DLA class code.
    
DLAContainer.h
    Contains the unmanaged c++ abstract class declaration for DLAContainer.
    
DLAContainer.cpp
    Implementation file for DLAContainer class.
    
DLA_2d.h
    This contains the unmanaged c++ class declaration DLA_2d inheriting from DLAContainer.
    
DLA_2d.cpp
    Implementation file for DLA_2d class.
    
DLA_3d.h 
    This contains the unmanaged c++ class declaration DLA_3d inheriting from DLAContainer.

DLA_3d.cpp
    Implementation file for DLA_3d class.

AssemblyInfo.cpp
	Contains custom attributes for modifying assembly metadata.
