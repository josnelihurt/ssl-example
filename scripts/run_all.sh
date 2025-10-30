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
docker compose down --remove-orphans 2>/dev/null || true
docker system prune -f 2>/dev/null || true

# Generate certificates using local script
echo "Generating SSL certificates..."
./scripts/generate_certs.sh

# Start all services (docker compose will build images automatically)
echo "Starting all services with docker compose..."
docker compose up --build -d

# Wait for services to start
echo "Waiting for services to start..."
sleep 10

# Show logs
echo "=========================================="
echo "Showing logs from all containers..."
echo "Press Ctrl+C to stop all services"
echo "=========================================="
echo ""
echo "NOTE: If you need to make changes and recompile:"
echo "  1. Press Ctrl+C to stop the services"
echo "  2. Modify the code as needed"
echo "  3. Run this script again"
echo ""
echo "The health endpoint is automatically called by Docker"
echo "=========================================="

# Function to handle cleanup on exit
cleanup() {
    echo -e "\n\nStopping all services..."
    docker compose down
    echo "Cleanup completed."
    exit 0
}

# Set trap to cleanup on script exit
trap cleanup SIGINT SIGTERM

# Show logs from all containers
docker compose logs -f
