```
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
docker buildx rm builder
docker buildx create --name builder --driver docker-container --use
docker buildx inspect --bootstrap
```

```
docker buildx build --push --platform linux/amd64,linux/arm64 -t sutoj/builder:noble .
```
