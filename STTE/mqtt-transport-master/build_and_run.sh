IMAGE_NAME="mqtttransport"
VERSION="latest"

# build Docker
docker build . -t container-registry.gitlab.cc-asp.fraunhofer.de/ist-201/images/$IMAGE_NAME:$VERSION

# run Docker image
docker run --rm --network host container-registry.gitlab.cc-asp.fraunhofer.de/ist-201/images/$IMAGE_NAME:$VERSION
