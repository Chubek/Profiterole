import ExtProfiterole




profiler = ExtProfiterole(name="myprofiler", outpath="myprofiler.bin")


profiler(breakid="INITBRK1")






profiler(breakid=profiler.sentinel)