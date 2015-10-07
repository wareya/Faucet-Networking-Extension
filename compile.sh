#!/bin/bash

source=(
 "faucet/Asio.cpp"
 "faucet/GmStringBuffer.cpp"
 "faucet/IpLookup.cpp"
 "faucet/ReadWritable.cpp"
 "faucet/macAddress.cpp"
 "faucet/socketApi.cpp"
 "faucet/tcp/CombinedTcpAcceptor.cpp"
 "faucet/tcp/TcpAcceptor.cpp"
 "faucet/tcp/TcpSocket.cpp"
 "faucet/tcp/connectionStates/ConnectionState.cpp"
 "faucet/tcp/connectionStates/TcpClosed.cpp"
 "faucet/tcp/connectionStates/TcpConnected.cpp"
 "faucet/tcp/connectionStates/TcpConnecting.cpp"
 "faucet/udp/UdpSocket.cpp"
 "faucet/udp/broadcastAddrs.cpp"
)
 
mkdir -p obj
mkdir -p obj/tcp
mkdir -p obj/tcp/connectionStates
mkdir -p obj/udp
 
cmd="g++ --std=gnu++11 -DBUILD_DLL -I.\
 -O2 -s -m32 -march=i686\
 -Wall -Wextra\
 -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-strict-aliasing"
linker="
 -static"
 
objects=""

for i in "${source[@]}"
do
    obj="`echo $i | sed 's-faucet/-obj/-g' | sed 's-.cpp-.o-g'`"
    deps=($(gcc -std=c++11 -MM $i | sed -e 's/^\w*.o://' | tr '\n' ' ' | sed -e 's/\\//g' | sed 's/ \+//' | sed 's/ \+/\n/g'))
    for j in "${deps[@]}"
    do
        if test $j -nt $obj; then
            echo "$cmd -c $i -o $obj"
            $cmd -c $i -o $obj
			break
        fi
    done
    objects="$objects $obj"
done
echo "g++ -o $executable $objects $linker"
g++ -m32 -o $executable $objects $linker

echo "done"
 