IMAGE_NAME="texttospeech"
VERSION="latest"

# docker login (fhg docker registry)
echo cWfqv7yPo9boTtAaGLga| docker login -u development --password-stdin container-registry.gitlab.cc-asp.fraunhofer.de

# build Docker
docker build . -t container-registry.gitlab.cc-asp.fraunhofer.de/ist-201/images/$IMAGE_NAME:$VERSION

# push Docker image
docker push container-registry.gitlab.cc-asp.fraunhofer.de/ist-201/images/$IMAGE_NAME:$VERSION
