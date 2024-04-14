## How to run piler

Edit the variables in docker-compose.yaml, then run

```
docker-compose up -d
```

## How to build the image for yourself

Pick the latest deb package from Github releases (https://github.com/jsuto/piler/releases)
and use it as the PACKAGE build argument, eg.

```
docker buildx build --load --platform linux/amd64 -t sutoj/piler:1.4.5 --build-arg PACKAGE=piler_1.4.5-jammy-ba34363 .
```
