
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

 /********************************************************************
 *
 * Module Name : main.c
 *
 * Author/Date : Jan C. Depner
 *
 * Description : Lists CHARTS .hof and tof files in ASCII format.
 *
 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <math.h>


/* Local Includes. */

#include "nvutility.h"

#include "FileHydroOutput.h"
#include "FileTopoOutput.h"
#include "FileWave.h"

#include "version.h"


void usage ()
{
  fprintf (stderr, "\nUsage: charts_list [-n RECORD NUMBER] [-s] [-t] [-d] [-y] [-w | -W] [-g \"lat,lon\"] HOF_OR_TOF_FILENAME\n");
  fprintf (stderr, "\nWhere:\n\n");
  fprintf (stderr, "\t-s  =  dump the shot data from the associated waveform file (HOF only).\n");
  fprintf (stderr, "\t-t  =  check the entire file for tide corrections.\n");
  fprintf (stderr, "\t-n  =  list RECORD NUMBER only.\n");
  fprintf (stderr, "\t-d  =  don't list null (-998.0) records.\n");
  fprintf (stderr, "\t-y  =  output ASCII Y,X,Z instead of entire record\n");
  fprintf (stderr, "\t-w  =  output water levels (HOF only) averaged over 2 second intervals\n");
  fprintf (stderr, "\t-W  =  output water levels (HOF only, not averaged)\n");
  fprintf (stderr, "\t-g  =  when used with -w or -W, append distance in meters from\n");
  fprintf (stderr, "\t\tthe provided lat,lon position.  The lat,lon position must\n");
  fprintf (stderr, "\t\tbe comma separated.  Latitude and longitude may be in any\n");
  fprintf (stderr, "\t\tof the following formats:\n\n");
  fprintf (stderr, "\t\t\tHemisphere Degrees Minutes Seconds.decimal\n");
  fprintf (stderr, "\t\t\tHemisphere Degrees Minutes.decimal\n");
  fprintf (stderr, "\t\t\tHemisphere Degrees.decimal\n");
  fprintf (stderr, "\t\t\tSign Degrees Minutes Seconds.decimal\n");
  fprintf (stderr, "\t\t\tSign Degrees Minutes.decimal\n");
  fprintf (stderr, "\t\t\tSign Degrees.decimal\n\n");
  fprintf (stderr, "\t\tUse the find command to output all of the averaged water level\n");
  fprintf (stderr, "\t\tvalues from a number of files to a single file.\n\n");
  fprintf (stderr, "\t\tExample:\n\n");
  fprintf (stderr, "\t\tfind . -name \\*.hof -exec charts_list -w {} >>output_file.txt \\;\n\n");
  fprintf (stderr, "\tIf RECORD NUMBER is not specified all records will be listed.\n");
  fprintf (stderr, "\tIf using -w and appending using the find command make sure that\n");
  fprintf (stderr, "\toutput_file.txt does not exist prior to running the find command.\n");
  fprintf (stderr, "\t-w, -t, and -n are mutually exclusive.\n\n");
  exit (-1);
}



void dump_shot_data (WAVE_DATA_T *wave_data)
{
  int32_t            i, j;

  union 
  {
    float            f;
    int32_t          i32;
    int16_t          i16[2];
    uint8_t          b[4];
  } shot;
    

  for (i = 0 ; i < 24 ; i++)
    {
      for (j = 0 ; j < 4 ; j++)
        {
          shot.b[j] = wave_data->shot_data[i * 4 + j];
        }          
      printf ("%d %f %d %hd %hd\n", i, shot.f, shot.i32, shot.i16[0], shot.i16[1]);
    }
}



int32_t main (int32_t argc, char **argv)
{
  char               file[512], tmp_file[512], wave_file[512], string[1024], cut[1024];
  int32_t            type = 0, rec_num = -1, i, total = 0, zero_tide = 0, wl_count = 0, year, jday, hour, minute;
  int64_t            timestamp, start_time = -1, last_time = -1;
  double             sum = 0.0, sumlat = 0.0, sumlon = 0.0, lat, lon, dist, az, per_ten_sec = 10000.0;
  float              second, level;
  NV_F64_COORD2      geo = {-999.0, -999.0};
  FILE               *fp = NULL, *wfp = NULL;
  HOF_HEADER_T       hof_header;
  HYDRO_OUTPUT_T     hof;
  TOPO_OUTPUT_T      tof;
  WAVE_HEADER_T      wave_header;
  WAVE_DATA_T        wave_data;
  uint8_t            tide_check = NVFalse, list_null = NVTrue, water_level = NVFalse, average = NVTrue, yxz = NVFalse, shot_data = NVFalse, geo_check = NVFalse,
                     srtm_check = NVFalse;
  char               c;
  extern char        *optarg;
  extern int         optind;


  fprintf (stderr, "\n\n %s \n\n\n", VERSION);


  while ((c = getopt (argc, argv, "tdwWysn:g:")) != EOF)
    {
      switch (c)
        {
        case 's':
          shot_data = NVTrue;
          break;

        case 't':
          tide_check = NVTrue;
          break;

        case 'd':
          list_null = NVFalse;
          break;

        case 'y':
          yxz = NVTrue;
          break;

        case 'n':
          sscanf (optarg, "%d", &rec_num);
          break;

        case 'w':
          water_level = NVTrue;
          average = NVTrue;
          break;

        case 'W':
          water_level = NVTrue;
          average = NVFalse;
          break;

        case 'g':
          strcpy (string, optarg);

          strcpy (cut, strtok (string, ","));
          posfix (cut, &geo.y, POS_LAT);
          strcpy (cut, strtok (NULL, ","));
          posfix (cut, &geo.x, POS_LON);

          geo_check = NVTrue;
          break;

        default:
          usage ();
          break;
        }
    }


  /* Make sure we got the mandatory file name argument.  */

  if (optind >= argc) usage ();

  if (geo_check && !water_level) usage ();

  if (tide_check || water_level) rec_num = -1;


  strcpy (file, argv[optind]);


  if (strstr (file, ".hof"))
    {
      if ((fp = open_hof_file (file)) == NULL)
        {
          perror (file);
          exit (-1);
        }
      type = 0;


      if (water_level)
        {
          srtm_check = NVFalse;
          if (!check_srtm_mask (3)) srtm_check = NVTrue;

          printf ("#%s\n", file);
        }


      hof_read_header (fp, &hof_header);

      last_time = -1;

      per_ten_sec = (double) hof_header.text.system_rep_rate * 10.0L;


      if (shot_data)
        {
          strcpy (wave_file, file);
          sprintf (&wave_file[strlen (wave_file) - 4], ".inh");

          wfp = open_wave_file (wave_file);

          if (wfp == NULL)
            {
              perror (wave_file);
              exit (-1);
            }

          wave_read_header (wfp, &wave_header);
        }
    }
  else if (strstr (file, ".tof"))
    {
      if (water_level)
        {
          fprintf (stderr, "\nCannot get water level from TOF files - Doh!\n\n");
          exit (-1);
        }


      if (tide_check)
        {
          fprintf (stderr, "\nCannot tide check TOF files - Doh!\n\n");
          exit (-1);
        }

      if ((fp = open_tof_file (file)) == NULL)
        {
          perror (file);
          exit (-1);
        }
      type = 1;
    }
  else
    {
      fprintf (stderr,"\nUnknown file extension %s\n", file);
      exit (-1);
    }


  if (type && shot_data) usage ();


  fprintf (stderr, "\n\nFile : %s\n\n", file);


  if (rec_num != -1)
    {
      fprintf (stderr, "\n\n");


      if (type)
        {
          tof_read_record (fp, rec_num, &tof);

          if (list_null || tof.elevation_last != -998.0)
            {
              if (yxz)
                {
                  if (tof.elevation_first != -998.0) printf ("%.11f,%.11f,%.2f\n", tof.latitude_first, tof.longitude_first, tof.elevation_first);
                  printf ("%.11f,%.11f,%.2f\n", tof.latitude_last, tof.longitude_last, tof.elevation_last);
                }
              else
                {
                  tof_dump_record (&tof);
                }

              timestamp = tof.timestamp;
            }
        }
      else
        {
          hof_read_record (fp, rec_num, &hof);

          if (list_null || hof.correct_depth != -998.0)
            {
              if (shot_data)
                {
                  wave_read_record (wfp, rec_num, &wave_data);

                  dump_shot_data (&wave_data);
                }

              if (yxz)
                {
                  printf ("%.11f,%.11f,%.2f\n", hof.latitude, hof.longitude, hof.correct_depth);
                }
              else
                {
                  hof_dump_record (&hof);
                }

              timestamp = hof.timestamp;
            }
        }

      remove (tmp_file);
    }
  else
    {
      /*
       * Read all of the data from this file.  
       */

      if (type)
        {
          while (tof_read_record (fp, TOF_NEXT_RECORD, &tof))
            {
              if (list_null || tof.elevation_last != -998.0)
                {
                  if (yxz)
                    {
                      if (tof.elevation_first != -998.0) printf ("%.11f,%.11f,%.2f\n", tof.latitude_first, tof.longitude_first, tof.elevation_first);
                      printf ("%.11f,%.11f,%.2f\n", tof.latitude_last, tof.longitude_last, tof.elevation_last);
                    }
                  else
                    {
                      tof_dump_record (&tof);
                    }
                }
            }
        }
      else
        {
          for (i = 0 ; i < hof_header.text.number_shots ; i++)
            {
              hof_read_record (fp, i + 1, &hof);

              if (tide_check)
                {
                  if (hof.reported_depth != -998.0)
                    {
                      if ((hof.reported_depth + hof.tide_cor_depth) == 0.0) zero_tide++;
                      total++;
                    }
                }
              else if (water_level)
                {
                  /*  Valid depth, valid water level, KGPS, not Shoreline Depth Swapped, not Shallow Water Algorithm, greater than 70 (70 = land),
                      skip the first and last ten seconds, and check SRTM land mask.  */

                  if (hof.correct_depth != -998.0 && hof.kgps_water_level != -998.0 && hof.data_type == 1 && hof.abdc != 72 &&
                      hof.abdc != 74 && hof.abdc > 70 && i > per_ten_sec && i < hof_header.text.number_shots - per_ten_sec)
                    {
                      if (srtm_check && !read_srtm_mask (hof.latitude, hof.longitude))
                        {
                          if (start_time < 0) start_time = hof.timestamp;


                          /*  If we're averaging and we encounter more than a second of bad data we don't want to use this section.  */

                          if (average && hof.timestamp - last_time > 1000000)
                            {
                              start_time = hof.timestamp;

                              sum = 0.0;
                              sumlat = 0.0;
                              sumlon = 0.0;
                              wl_count = 0;
                            }

                          if ((!average || hof.timestamp - start_time > 2000000) && last_time != -1)
                            {
                              timestamp = start_time + (last_time - start_time) / 2;
                              start_time = timestamp;

                              if (average)
                                {
                                  level = (float) (sum / (double) wl_count);
                                  lat = sumlat / (double) wl_count;
                                  lon = sumlon / (double) wl_count;
                                }
                              else
                                {
                                  level = hof.kgps_water_level;
                                  lat = hof.latitude;
                                  lon = hof.longitude;
                                }

                              charts_cvtime (timestamp, &year, &jday, &hour, &minute, &second);

                              if (geo_check)
                                {
                                  invgp (NV_A0, NV_B0, geo.y, geo.x, lat, lon, &dist, &az);

                                  printf ("%.9f %.9f %d %03d %02d:%02d:%05.2f %.3f %.3f\n", lat, lon, year + 1900, jday, hour, minute, second, level, dist);
                                }
                              else
                                {
                                  printf ("%.9f %.9f %d %03d %02d:%02d:%05.2f %.3f\n", lat, lon, year + 1900, jday, hour, minute, second, level);
                                }

                              sum = 0.0;
                              sumlat = 0.0;
                              sumlon = 0.0;
                              wl_count = 0;
                            }

                          wl_count++;
                          sum += hof.kgps_water_level;
                          sumlat += hof.latitude;
                          sumlon += hof.longitude;
                          last_time = hof.timestamp;
                        }
                    }
                  else
                    {
                      if (hof.data_type != 1)
                        {
                          fprintf (stderr, "\nCannot get water level from non-KGPS HOF files - Doh!\n\n");
                          exit (-1);
                        }
                    }
                }
              else
                {
                  if (list_null || hof.correct_depth != -998.0)
                    {
                      if (shot_data)
                        {
                          wave_read_record (wfp, i + 1, &wave_data);

                          dump_shot_data (&wave_data);
                        }

                      if (yxz)
                        {
                          printf ("%.11f,%.11f,%.2f\n", hof.latitude, hof.longitude, hof.correct_depth);
                        }
                      else
                        {
                          hof_dump_record (&hof);
                        }
                    }
                }
            }
        }
    }


    fclose (fp);


    if (tide_check)
      {
        i = ((float) zero_tide / (float) total) * 100.0;

        if (i > 1)
          {
            fprintf (stderr, "\nFile : %s not tide corrected\n", file);
            fprintf (stderr, "Total = %d, no tide correction = %d\n", total, zero_tide);
          }
      }


    return (0);
}
