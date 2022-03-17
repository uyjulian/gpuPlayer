======================================================================
                         GPU Player v2.0         (October, 16th 2003.)

                         by Matesic Darko
======================================================================
                                              e-mail: darkman@eunet.yu

1) About
---------
	This is a program for playing REC files created by gpuRecorder
while using it with psx emulator.

2) Usage
---------
	Copy gpuPlayer.exe into a directory with some plugins. When
starting it for the first time enter options to configure it (either
by pressing O or through menu View->Options). Now you can play recorded
files by clicking Ctrl+O (or through menu File->Open) and selecting
a *.REC file.
	Seeking is possible only with new version of REC. 

3) Options
----------
	- Video plugin: choose your gpu for playback
	- Sound plugin: choose your spu for playback
	- enable sound playback: uncheck this if you don't want
		sound plyback in files that have sound data
	- show time/frames: choose type of info in the bottom
		of the player window
	- replay when done: start playback from begining when
		reached the end

4) Rebuild
----------
	- source file: your recorded file (when you select
		proper REC file some info will be displayed
		in lower left part of dialog)
	- destination file: your new (will be) rebuilded file
	- destination file settings:
		- compression: you never heard of ZIP?
		- block size: minimum size of data to be 
			compressed
		- rebuild sound data: uncheck this if you
			don't want any sound data in new file

5) Additional Info
------------------
	- I tried compression with some files and never saw
		difference beetwen zlib default and best compression
	- bzip too slow (compression and decompression wich slows
		down playback)
	- don't choose bit block size, playback may be choppy because
		more data must be decompressed in one read
	- seeking is not perfect and it will never be perfect, can cause
		garbage and alse sound errors
	- enabling / disabling sounds is not perfect, can cause sound
		errors

6) Version history
-------------------
October, 16th 2003. Version 2.0
-------------------------------
	- new gui
	- sound playback (if it was recorded)
	- old and new files can be rebuilded
	- seeking is possible
	- snapshot is possible
	
August, 4th 2002. Version 1.1
----------------------------------
	- added more checks when calling gpu functions
	- fixed "divide by zero" error that happend when .REC file
		is not properly closed; this happens when for
		example emulator does not terminate properly and
		does not call GPUclose; now playback is still possible


July, 30th 2002. Version 1.0
----------------------------------
	- first public release

====================================================================
   URL: http://mrdario.tripod.com
e-mail: darkman@eunet.yu
