FROM nginx:alpine

# Copy nginx configuration
COPY nginx/nginx.conf /etc/nginx/nginx.conf

# Create directory for certificates
RUN mkdir -p /etc/nginx/certs

# Copy certificates (will be mounted at runtime)
# The certificates will be mounted from the host via docker-compose

# Expose ports
EXPOSE 8443 8444

# Start nginx
CMD ["nginx", "-g", "daemon off;"]
