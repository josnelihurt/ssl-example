#!/bin/bash

set -e

CERT_DIR="$(dirname "$0")/../certs"
SCRIPT_DIR="$(dirname "$0")"

echo "Generating SSL certificates..."

# Clean previous certificates
rm -rf "$CERT_DIR"/*
mkdir -p "$CERT_DIR"

cd "$CERT_DIR"

# Generate Root CA private key
echo "Generating Root CA private key..."
openssl genrsa -out root-ca.key 4096

# Generate Root CA certificate
echo "Generating Root CA certificate..."
openssl req -new -x509 -days 3650 -key root-ca.key -out root-ca.crt -subj "/C=US/ST=CA/L=San Francisco/O=SSL Example/OU=IT Department/CN=Root CA"

# Generate server private key
echo "Generating server private key..."
openssl genrsa -out server.key 2048

# Generate server certificate signing request
echo "Generating server certificate signing request..."
openssl req -new -key server.key -out server.csr -subj "/C=US/ST=CA/L=San Francisco/O=SSL Example/OU=IT Department/CN=nginx" -addext "subjectAltName = DNS:nginx,DNS:localhost,DNS:server.local"

# Generate server certificate signed by Root CA
echo "Generating server certificate..."
openssl x509 -req -in server.csr -CA root-ca.crt -CAkey root-ca.key -CAcreateserial -out server.crt -days 365 -extfile <(printf "subjectAltName=DNS:nginx,DNS:localhost,DNS:server.local")

# Generate valid client private key
echo "Generating valid client private key..."
openssl genrsa -out client-valid.key 2048

# Generate valid client certificate signing request
echo "Generating valid client certificate signing request..."
openssl req -new -key client-valid.key -out client-valid.csr -subj "/C=US/ST=CA/L=San Francisco/O=SSL Example/OU=IT Department/CN=client-valid"

# Generate valid client certificate signed by Root CA
echo "Generating valid client certificate..."
openssl x509 -req -in client-valid.csr -CA root-ca.crt -CAkey root-ca.key -CAcreateserial -out client-valid.crt -days 365

# Generate invalid client private key (self-signed)
echo "Generating invalid client private key..."
openssl genrsa -out client-invalid.key 2048

# Generate invalid client certificate (self-signed, not from Root CA)
echo "Generating invalid client certificate..."
openssl req -new -x509 -key client-invalid.key -out client-invalid.crt -days 365 -subj "/C=US/ST=CA/L=San Francisco/O=SSL Example/OU=IT Department/CN=client-invalid"

# Create combined certificate files for easier use
echo "Creating combined certificate files..."
cat server.crt server.key > server.pem
cat client-valid.crt client-valid.key > client-valid.pem
cat client-invalid.crt client-invalid.key > client-invalid.pem

# Set proper permissions
chmod 600 *.key
chmod 644 *.crt *.pem

# Clean up CSR files
rm -f *.csr

echo "Certificate generation completed!"
echo "Generated files:"
echo "  - root-ca.crt (Root CA certificate)"
echo "  - root-ca.key (Root CA private key)"
echo "  - server.crt (Server certificate)"
echo "  - server.key (Server private key)"
echo "  - server.pem (Server certificate + key combined)"
echo "  - client-valid.crt (Valid client certificate)"
echo "  - client-valid.key (Valid client private key)"
echo "  - client-valid.pem (Valid client certificate + key combined)"
echo "  - client-invalid.crt (Invalid client certificate)"
echo "  - client-invalid.key (Invalid client private key)"
echo "  - client-invalid.pem (Invalid client certificate + key combined)"
