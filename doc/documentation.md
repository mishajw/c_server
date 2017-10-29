# `c_server`

A simple server that serves files in a directory. Supports:

1) `GET`/`POST`/`HEAD` requests
2) Byte ranges
3) Redirects to `index.html` on an empty request path
4) Send appropriate response for `400 Bad Request`, `404 Not Found`, `505 HTTP Version Not Supported`

## How to build

`gcc` is needed for compilation.

Run `make build` from the `./src` directory, and a binary will be created at `./bin/c_server`.

## How to run

Either run `make run` from the `./src` directory, or simply run the executable at `./bin/c_server`.

## How to use

1) Change directory into the directory you would like to serve on the server
2) Run the binary from this directory
3) Make HTTP requests to `localhost:12612`

### Supported requests

```bash
# Return the contents of "some-page.html"
curl localhost:12612/some-page.html

# Return the binary of the image "img.png"
curl localhost:12612/img.png > img.png

# Both will return the contents of index.html
curl localhost:12612
curl localhost:12612/index.html

# Send a POST request to the server, just returns "some-page.html"
curl -X POST localhost:12612/some-page.html -d "..."

# Send a HEAD request to the server, doesn't return contents
curl --head localhost:12612/some-page.html

# Returns "400 Bad Request" error
curl -X NOTVALID localhost:12612/some-page.html

# Returns "404 Not Found" error
curl localhost:12612/doesnt-exist.html

# Returns "505 HTTP Version Not Supported" error
curl --http1.0 localhost:12612/some-page.html

# Returns first 512 bytes of "img.png"
curl localhost:12612/img.png -H "Range: bytes=0-512"

# Returns bytes 1024-2048
curl localhost:12612/img.png -H "Range: bytes=1024-2048"
```

