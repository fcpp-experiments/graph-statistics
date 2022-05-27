#!/bin/bash

git submodule init
git submodule update
mkdir -p plot
cp fcpp/src/extras/plotter/plot.asy plot/

if [ "$1" == download ]; then
    wget http://data.law.di.unimi.it/webdata/$2/$2.graph
    wget http://data.law.di.unimi.it/webdata/$2/$2.properties
    wget http://data.law.di.unimi.it/webdata/$2/$2.urls.gz
    mv $2.{graph,properties,urls.gz} input/
fi

for i in input/*.graph; do
    base=`basename $i .graph`
    f=input/$base
    if [ ! -f $f.properties ]; then
        echo "Converting $i...ERROR: missing $f.properties." >&2
        echo "Try this command to download it:" >&2
        echo "wget http://data.law.di.unimi.it/webdata/cnr-2000/$base.properties; mv $base.properties input/" >&2
        continue
    fi
    if [ ! -f $f.graph-txt -o $f.graph -nt $f.graph-txt -o ! -f $f.out -o $f.graph -nt $f.out ]; then
        mkdir java/data
        cp $f.* java/data
        cd java
        mvn compile
        if [ ! -f $f.graph-txt -o $f.graph -nt $f.graph-txt ]; then
            mvn exec:java -Dexec.mainClass="HBApp" -Dexec.args="conv data/$base"
            mv data/$base.graph-txt ../input/
        fi
        if [ ! -f $f.out -o $f.graph -nt $f.out  ]; then
            mvn exec:java -Dexec.mainClass="HBApp" -Dexec.args="stats data/$base"
            mv data/$base.out ../input/
        fi
        cd ..
        rm -rf java/data
    fi
    if [ ! -f $f.urls.gz -a ! -f $f.urls ]; then
        echo "Converting $i...ERROR: missing $f.urls.gz" >&2
        echo "Try this command to download it:" >&2
        echo "wget http://data.law.di.unimi.it/webdata/cnr-2000/$base.urls.gz; mv $base.urls.gz input/" >&2
        continue
    fi
    if [ ! -f $f.urls -o $f.urls.gz -nt $f.urls ]; then
        cd input
        gzip -dk $base.urls.gz
        cd ..
    fi
done

fcpp/src/make.sh "$@"
if [ "$1" == "" ]; then
    echo
    echo "Custom command: ./make.sh download <test>"
fi
