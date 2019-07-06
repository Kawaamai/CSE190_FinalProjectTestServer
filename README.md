# Space Pong

For CSE 190

by Alan Mai

[Blog Posts](https://github.com/Kawaamai/CSE190_FinalProjectBlog)

[Project Video](https://www.youtube.com/watch?v=DzfWp2AYeEE&list=PLYZSzgYHJxiyt9ymwjVNnx9wq4d-WeQ9R&index=11)

## Source Code

```
git clone https://github.com/Kawaamai/CSE190_FinalProjectTestServer.git
```

populate `yojimbo` and `PhysX`

```
git submodule update --init --recursive
```

build `PhysX`:

- build all of `PhysXSDK` filter + `SnippetUtils` project on Release and dynamic linked.

- `Properties -> C/C++ -> Runtime Library-> Multi-threaded Dll (/MD)`
