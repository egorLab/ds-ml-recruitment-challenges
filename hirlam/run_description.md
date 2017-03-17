**Ubuntu Linux** : 

1. Download wgrib (ftp://ftp.cpc.ncep.noaa.gov/wd51we/wgrib/wgrib.c)

1. Run `gcc wgrib.c -o wgrib`

1. Run `gcc main.c -o grib_checks`

1. Run `./grib_checks <full_path_to_dir_with_GRIB_files>`


**Windows 7** :

1. Install wgrib ftp.cpc.ncep.noaa.gov/wd51we/wgrib/machines/Windows/wgrib.exe; 

1. Check if gcc >= 4.2 else https://gcc.gnu.org/releases.html;

1. Open cmd and run from dir with main.c:
    * `<path-to-gcc>/gcc.exe main.c -o grib_checks.exe`
    * `grib_checks.exe <full_path_to_dir_with_GRIB_files>`