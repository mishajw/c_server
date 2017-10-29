# Diary

Creating a server in C was a fun project, and allowed me to refresh my C skills, and more generally my appreciation of low level constructs.

Having raw handles on data coming in and out of the server allowed for some interesting optimisations, for example examining the effect of changing the `BUFFER_SIZE` constant found in `./src/server.c`. I found that up to a point, increasing this value helped speed up processing of large request, but made no difference after this point.

Setting up the byte range options for the server was also interesting. Unfortunately, I did not get a chance to implement all possibilities of this - I was only able to support explicit byte ranges (e.g. `256-512`). I would have liked to have time to implement implicit ranges such as `256-` to get to the end of the file, and raising the appropriate messages when the byte range was out of bounds.

A big issue I had with this implementation was dealing with string manipulation comprehensively and securely. Using the appropriate functions such as `strncmp` helped deal with security, but parsing strings was still very verbose and tedious. For example, to parse the byte ranges, you must:

1) Split by line to get request fields
2) Split by `: ` to get the field name and value
3) Split by `=` to get the `bytes` string and the actual ranges
4) Split by `-` to get the lower and upper ranges

Of course this is all doable in vanilla C, but it does take a while and makes it quite difficult to debug as there is a lot of code for it to go wrong. If I was to do this project again, I would have used some RegEx library to deal with string parsing. This would have made the code a lot shorter, and therefore a lot more comprehensive. 

Another feature I would have liked to accomplish is absolutely no memory leaked on exit. If you modify the code to only run for one request, there are no memory leaks. However, due to multithreading, once you Ctrl-C out of the server, some memory is left allocated. I tried to solve this by using C's `signal.h` library to handle SIGINT, but I could not find a way to interrupt the socket `accept` function and handle deallocation of the `connection` struct.

I would have also liked to support command line arguments to set, for example, the port number, server path, and whether to run multithreaded.
