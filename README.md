# CSE190_FinalProjectTestServer

## Source Code

```
git clone https://github.com/Kawaamai/CSE190_FinalProjectTestServer.git
```

populate `yojimbo`

```
git submodule update --init --recursive
```

build with `premake5 solution`, then build `yojimbo` as 

```
Code Generation -> Runtime Library -> Multi-threaded Dll (/MD)  
```

same for `server` sample (I think)

build `PhysX`: build all of `PhysXSDK` as:

- `release -> Runtime Library-> Multi-threaded Dll (/MD)`