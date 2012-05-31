Tyk : a console stopwatch
=========================

`tyk` runs a command, counts how much time its completion takes, and kills it
after a certain timeout.

Normal mode
-----------

`tyk [-t time] command args` will run `command args` and display a progress bar.
When the progress bar is full (it takes `time`, or 8 seconds by default), the
process is killed.

Tea mode
--------

`tyk -t 180 cat` will wait 3 minutes displaying a nice progress bar. Shortly after,
this kills the `cat`. Enjoy your tea.

Bugs
----

Probably.
