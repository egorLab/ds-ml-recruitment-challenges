# Task

Create an application for checking GRIB-files (special-way formatted file with information about meteorological conditions)
with HIRLAM forecasts.

Conditions to be checked:

1. Passed set of files consists of **all and only** those forecasts, whose start dates and forecast lead time hold these     conditions:
    * date of earliest forecast;
    * date of latest forecast;
    * interval between two consequent start dates;
    * **min** forecast lead time;
    * **max** forecast lead time;
    * interval between two consequent forecast lead times.
    
1. Date of the beginning of GRIB-records and their forecast lead time in each file match same two parameters in the name of this file;
1. Each file includes *only* three GRIB-records : mean sea level pressure, two wind components (10m mark).

The result of the application should be a list of detected violation of conditions.

Moreover, this application will be evaluated on speed issues (input files ~300000) and code style.
Test set download link (~200Mb) : https://dl.dropboxusercontent.com/u/1859138/hirlam_sample_input.zip