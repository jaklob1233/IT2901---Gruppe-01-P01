IMAGE_NAME="speechtotext"
VERSION="latest"

# build Docker
docker build . -t $IMAGE_NAME:$VERSION

# run Docker image
docker run --rm -d --network host $IMAGE_NAME:$VERSION
