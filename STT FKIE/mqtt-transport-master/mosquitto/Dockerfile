# Use the official Eclipse Mosquitto image
FROM eclipse-mosquitto:2.0.15

# Expose the default MQTT and WebSocket ports (already handled by the base image)
EXPOSE 1883 9001

# Copy custom configuration if needed
# Uncomment the next line to use your own mosquitto.conf
COPY mosquitto.conf /mosquitto/config/mosquitto.conf

# Optional: Create directories for persistence and logging
# (The official image already supports volume mounting for /mosquitto/data and /mosquitto/log)
VOLUME ["/mosquitto/data", "/mosquitto/log"]

# Start the broker with the default or custom configuration
CMD ["mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
