#!/bin/bash

rm -rf master.tar.gz ~/rpmbuild; mkdir -p mkdir ~/rpmbuild/{SOURCES,BUILDROOT}
wget https://bitbucket.org/jsuto/piler/get/master.tar.gz
tar zxf master.tar.gz
rm -rf piler-0.1.25
mv jsuto-piler-* piler-0.1.25
tar cfz ~/rpmbuild/SOURCES/master.tar.gz piler-0.1.25
(cd piler-0.1.25 ; rpmbuild -bb piler.spec)

