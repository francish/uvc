#!/usr/bin/python2.7
import subprocess
import _pg

#conn = _pg.connect("imdb", "w.cs.uvic.ca", 5432, None, None, "csc370", "ecs108")
conn = _pg.connect("postgres", "localhost", 5432, None, None, "postgres", "something")

print "Running question 2. J)...."
x = conn.query("select * from discoverychannel_j()")
subprocess.call("clear")
print x

conn.close()
