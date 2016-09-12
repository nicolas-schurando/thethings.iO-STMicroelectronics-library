# thethings.iO-DISCO_F746NG-http

Follow this simple steps to connect your STM32F746 Discovery board to thethings.iO

1. Create a thing with our platform.
2. Go to the online mbed compiler and add this board (DISCO_F46) as the platform you're working with.
3. Search and import from the mbed library, the sample program (mqtt or http version).
4. Change the thingToken with an actual thingToken.
5. Compile, download and drag & drop the resulting binary file to your board (via USB filesystem).

## Library endpoints:

#### ```string thingReadAll(int limit);```

Read the latest values (specified by limit) of all the resources of the actual thingToken.

#### ```string thingRead(string resource, int limit);```

Read the latest values (specified by limit) of a resource.

#### ```int thingWrite(string resource, string value);```

Write a pair resource - value to thethings.io.

#### ```string thingActivate(string activationCode);```

Get a thingToken from an activationCode.

All the previous calls return the response body from the request to thethings.iO. Please filter the response at your will.
