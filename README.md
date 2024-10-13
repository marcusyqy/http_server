# HTTP server from scratch
A simple http server that serves static text and images.
This is meant for educational purposes and should not be used in production unless you know what you are doing.

## How to run
if you are on linux/osx: run `./build.sh run`
if you are on windows:   run `build.bat run`

## Motivation
I wanted to learn what goes into a http server and how to parse the headers.
This is a simple server that serves static files.

## What's next
This list is something that I probably want to implement but may never do. I don't want to
be a http expert, I just want to know stuff.

- [ ] Upgrade to websockets.
- [ ] Parse command line args so that we don't hardcode port.
- [ ] Parse other header lines of http request.
- [ ] Parse other functions (POST) in http request.
- [ ] Use better memory management pools.
