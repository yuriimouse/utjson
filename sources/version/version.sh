#!/bin/bash
self=$_
version=${self%.*}

if [ -e "$version" ]; then
    current=$( cat $version | grep -m 1 '[^[:blank:]]' )
    echo $current
else 
    V_MAJOR=$($version.major)
    V_MINOR=$($version.minor)
    V_COMMIT=`git log --pretty=format:'%h' -n 1`
    V_BUILD=$($version.build)
    VERSION=$V_MAJOR.$V_MINOR.$V_BUILD

    echo "$VERSION"
    record="$VERSION\n$V_COMMIT"
    echo -e "$record" >$version
fi 

