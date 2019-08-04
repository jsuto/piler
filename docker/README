How to build

  Pick the latest deb package from Bitbucket download page (https://bitbucket.org/jsuto/piler/downloads/)
  and use it as the PACKAGE build argument, eg.

  docker build --build-arg PACKAGE=piler_1.3.6~bionic-65cc7eb_amd64.deb -t sutoj/piler .

How to run the image

  Set the PILER_HOST env variable to match your hostname, eg.

  docker run -d --name piler1 -p 25:25 -p 80:80 -e PILER_HOST=archive.yourdomain.com sutoj/piler
