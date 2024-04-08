# build image

Currently image is 6.5 GB due to many unused dependecies and shall be dramatically reduced.

### example build command

```docker build -t server_image .```

# configure and build app

### enter container

```docker run -it -v ${PWD}:/src --name clone_server server_image:latest```

### cmake configure

```cd src/ ```

```mkdir build && cd build ```

```cmake .. -G Ninja -DCMAKE_PREFIX_PATH="/usr/local/Qt/6.5.3/gcc_64/"```

``` cmake --build . ```

# Manual testing

Open 4 terminals

1. For socat simulation
2. For device
3. For server
4. For curl

```docker exec -it clone_server bash```

In first terminal call:

```socat -d -d pty,raw,echo=0,link=/dev/ttyUSB0 pty,raw,echo=0,link=/dev/ttyUSB1```

Example output shall appear

```
2024/04/08 15:08:31 socat[239] N PTY is /dev/pts/2
2024/04/08 15:08:31 socat[239] N PTY is /dev/pts/3
2024/04/08 15:08:31 socat[239] N starting data transfer loop with FDs [5,5] and [7,7]
```

In second terminal

```cd src/build && ./server```

In third terminal

```cd src/build/device && ./Device```

In fourth terminal

simulate http request from following examples

```curl -i -X GET http://localhost:7100/start```

```curl -i -X GET http://localhost:7100/stop```

```curl -i -X PUT -d "frequency=1&debug=false" http://localhost:7100/configure```

```curl -i -X GET http://localhost:7100/device```

```curl -i -X GET http://localhost:7100/messages?limit=5```

