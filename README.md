# stopwatch-GTK

This small GTK 3 program works well for keeping track of time for submitting timesheets or taking tests, which is what it is made for.  It will follow the clock for that and be accurate.  This program may be off about +/- 1 seconds total because it just uses the UNIX epoch time.  Therefore, don't use this program for anything medical or anything requiring sub-second precision. All it does it check the UNIX epoch time 10 times a second and updates the stopwatch when the UNIX epoch time changes.  It doesn't use GTimer, as I wanted more control for further development of the stopwatch.

There is an upper limit on how long the program will run, which is 3,215 years.

# To compile
This short program is only for modern Linux distributions.
## Debian-based GNU/Linux
```
apt install libgtk-3-dev make clang git pkg-config
git clone https://github.com/orchardstreet/stopwatch-GTK
cd stopwatch-GTK
make
```
## Alpine Linux
```
apk add gtk+3.0-dev clang make git pkgconfig
git clone https://github.com/orchardstreet/stopwatch-GTK
cd stopwatch-GTK
make
```
