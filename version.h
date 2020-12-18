
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/


/*********************************************************************************************

    This program is public domain software that was developed by 
    the U.S. Naval Oceanographic Office.

    This is a work of the US Government. In accordance with 17 USC 105,
    copyright protection is not available for any work of the US Government.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*********************************************************************************************/

#ifndef VERSION

#define     VERSION     "PFM Software - charts_list V2.33 - 07/23/14"

#endif

/*

    Version 1.0
    Jan C. Depner
    08/10/03


    Version 1.1
    Jan C. Depner
    08/22/03

    Added ability to view images associated with timestamp.


    Version 1.2
    Jan C. Depner
    05/27/04

    Added tide check option.


    Version 1.3
    Jan C. Depner
    06/16/04

    Minor clean up.  Removed xv and replaced with ee (Electric Eyes).  Made 
    this the default and changed the -x option to -i for Image Magick.


    Version 2.0
    Jan C. Depner
    09/09/04

    Removed image viewing capability.  This is now taken care of by the
    ChartsPic program.


    Version 2.1
    Jan C. Depner
    10/11/04

    Added -d option to suppress listing of null (-998.0) records.


    Version 2.11
    Jan C. Depner
    04/11/05

    Removed "Press enter to finish" messages.  No longer needed with Qt pfmEdit as 
    the launching program.


    Version 2.2
    Jan C. Depner
    08/22/05

    Added -w option to retrieve one minute averages of water_level from HOF files.


    Version 2.21
    Jan C. Depner
    10/22/07

    Added fflush calls after prints to stderr since flush is not automatic in Windows.


    Version 2.22
    Jan C. Depner
    04/07/08

    Replaced single .h files from utility library with include of nvutility.h


    Version 2.23
    Jan C. Depner
    07/29/09

    Changed %lf in printf statements to %f.  The MinGW folks broke this even though it works on every
    compiler known to man (including theirs up to the last version).  No compiler warnings or errors
    are issued.  Many thanks to the MinGW folks for fixing what wasn't broken.


    Version 2.24
    Jan C. Depner
    06/03/10

    Added option to dump data as simple YXZ ASCII.


    Version 2.25
    Jan C. Depner
    10/27/10

    Added option to dump shot data as 32 bit float, 32 bit int, and two 16 bit ints.


    Version 2.26
    Jan C. Depner
    01/31/11

    Added -W option to dump all water level data without averaging.


    Version 2.27
    Jan C. Depner
    05/06/11

    Fixed problem with getopt that only happens on Windows.


    Version 2.28
    Jan C. Depner
    06/16/11

    Don't use invalid data for water level computation.


    Version 2.30
    Jan C. Depner
    07/05/11

    Added optional distance from geodetic point when extracting water level.


    Version 2.31
    Jan C. Depner
    09/29/11

    Added SRTM check for water level.


    Version 2.32
    Jan C. Depner
    11/08/11

    Corrected timestamp averaging bug in water_level mode.


    Version 2.33
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).

*/
