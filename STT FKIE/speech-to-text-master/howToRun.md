From directory speech-to-text-master, build Docker with:

```
docker build -t speechtotext .
```

Run the docker container in terminal:

```
docker run --rm --network host --name speechtotext speechtotext
```

OR

Run the docker container in detached mode:

```
docker run --rm -d --network host --name speechtotext speechtotext
```

Open localhost on:
```
http://localhost:8080/speechtotext/ui
```