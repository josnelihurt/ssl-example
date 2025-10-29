#!/bin/bash

set -e

SCRIPT_DIR="$(dirname "$0")"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "SSL/TLS and mTLS Demo Project"
echo "=========================================="

# Change to project root
cd "$PROJECT_ROOT"

# Clean previous builds and containers
echo "Cleaning previous builds and containers..."
docker-compose down --remove-orphans 2>/dev/null || true
docker system prune -f 2>/dev/null || true

# Generate certificates using local script
echo "Generating SSL certificates..."
./scripts/generate_certs.sh

# Build Nginx image
echo "Building Nginx image..."
docker build -f nginx/Dockerfile -t ssl-example-nginx .

# Get current user ID and group ID
USER_ID=$(id -u)
GROUP_ID=$(id -g)

# Build client Docker images in parallel (each one compiles its own binary)
echo "Building client Docker images in parallel..."
docker build -f clients/Dockerfile --build-arg BINARY_NAME=test_https_no_ca --build-arg USER_ID=$USER_ID --build-arg GROUP_ID=$GROUP_ID -t ssl-example-test_https_no_ca . &
docker build -f clients/Dockerfile --build-arg BINARY_NAME=test_https_insecure --build-arg USER_ID=$USER_ID --build-arg GROUP_ID=$GROUP_ID -t ssl-example-test_https_insecure . &
docker build -f clients/Dockerfile --build-arg BINARY_NAME=test_https_with_ca --build-arg USER_ID=$USER_ID --build-arg GROUP_ID=$GROUP_ID -t ssl-example-test_https_with_ca . &
docker build -f clients/Dockerfile --build-arg BINARY_NAME=test_mtls_no_client_cert --build-arg USER_ID=$USER_ID --build-arg GROUP_ID=$GROUP_ID -t ssl-example-test_mtls_no_client_cert . &
docker build -f clients/Dockerfile --build-arg BINARY_NAME=test_mtls_invalid_cert --build-arg USER_ID=$USER_ID --build-arg GROUP_ID=$GROUP_ID -t ssl-example-test_mtls_invalid_cert . &
docker build -f clients/Dockerfile --build-arg BINARY_NAME=test_mtls_valid_cert --build-arg USER_ID=$USER_ID --build-arg GROUP_ID=$GROUP_ID -t ssl-example-test_mtls_valid_cert . &

# Wait for all builds to complete
echo "Waiting for all client builds to complete..."
wait

# Start all services
echo "Starting all services with docker-compose..."
docker-compose up -d

# Wait for services to start
echo "Waiting for services to start..."
sleep 10

# Show logs
echo "=========================================="
echo "Showing logs from all containers..."
echo "Press Ctrl+C to stop all services"
echo "=========================================="

# Function to handle cleanup on exit
cleanup() {
    echo -e "\n\nStopping all services..."
    docker-compose down
    echo "Cleanup completed."
    exit 0
}

# Set trap to cleanup on script exit
trap cleanup SIGINT SIGTERM

# Show logs from all containers
docker-compose logs -f
