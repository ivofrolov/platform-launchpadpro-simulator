simulator := src/*.[ch]
launchpad := include/*.h
raylib := lib/raylib/src/*.[ch]
sources = $(simulator) $(launchpad) $(raylib)

tags: $(sources)
	etags -l c --declarations -o $@ $(sources)
