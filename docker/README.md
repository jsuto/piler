How to run piler

Edit the variables in docker-compose.yaml, then run

```
docker-compose up -d
```

How to build the image for yourself

  Pick the latest deb package from Bitbucket download page (https://bitbucket.org/jsuto/piler/downloads/)
  and use it as the PACKAGE build argument, eg.

  docker build --build-arg PACKAGE=piler_1.3.11-focal-5c2ceb1_amd64.deb -t piler:1.3.11 .
