# Multi-stage build for C++ clients
FROM ubuntu:22.04 as builder

# Install build dependencies with optimizations
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    curl \
    libcurl4-openssl-dev \
    libssl-dev \
    pkg-config \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Copy only necessary source files first (for better cache)
COPY clients/ssl_utils.h clients/ssl_utils.cc ./

# Build the specified binary
ARG BINARY_NAME
COPY clients/$BINARY_NAME.cc ./
RUN g++ -std=c++17 -I. -o $BINARY_NAME ssl_utils.cc $BINARY_NAME.cc -lcurl -lssl -lcrypto

# Runtime stage - use ubuntu for compatibility
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    libcurl4 \
    libssl3 \
    ca-certificates \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Get user ID from build arg
ARG USER_ID=1000
ARG GROUP_ID=1000

# Create user with the same UID/GID as host user
RUN groupadd -g $GROUP_ID appuser && \
    useradd -u $USER_ID -g appuser -s /bin/bash -m appuser

# Copy the built binary and set proper permissions
ARG BINARY_NAME
COPY --from=builder /workspace/$BINARY_NAME /usr/local/bin/$BINARY_NAME
RUN chmod +x /usr/local/bin/$BINARY_NAME && \
    chown appuser:appuser /usr/local/bin/$BINARY_NAME

# Create certificates directory with proper ownership
RUN mkdir -p /certs && chown appuser:appuser /certs

# Create a startup script before switching to non-root user
RUN echo '#!/bin/bash' > /usr/local/bin/start.sh && \
    echo 'exec /usr/local/bin/'"$BINARY_NAME" >> /usr/local/bin/start.sh && \
    chmod +x /usr/local/bin/start.sh && \
    chown appuser:appuser /usr/local/bin/start.sh

# Switch to non-root user
USER appuser

# Set working directory
WORKDIR /certs

# Run the startup script
CMD ["/usr/local/bin/start.sh"]
