# Delta   Evento   Canal   Param1   Param2
0         12       1       1        1        # Canal 1 activa el efecto 1 (Delay)
0         9        1       60       90       # Toca la nota Do4 (Note On)
200       8        1       60       0        # Apaga la nota Do4 tras 200 ticks (~0.8s)
600       9        1       64       90       # Toca la nota Mi4 tras 600 ticks (~2.5s)
200       8        1       64       0        # Apaga la nota Mi4 tras 200 ticks (~0.8s)
600       0        1       0        0        # Margen final de 600 ticks (~2.5s) para escuchar los ecos finales
