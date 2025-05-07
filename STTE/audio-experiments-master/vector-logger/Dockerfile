# Use the official Vector image from Timber
FROM timberio/vector:0.10.0-debian

# Copy the Vector configuration file (Make sure you have vector.yaml in the same directory)
COPY vector.toml /etc/vector/vector.toml

# Expose the port for HTTP log ingestion
EXPOSE 8686

# Start Vector with the provided configuration file
ENTRYPOINT ["vector"]
