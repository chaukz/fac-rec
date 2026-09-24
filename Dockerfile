# --- build stage ---
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libopencv-dev \
    libdlib-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -S . && cmake --build build -j"$(nproc)"

# --- runtime stage ---
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libopencv-highgui4.5d \
    libopencv-imgproc4.5d \
    libopencv-videoio4.5d \
    libdlib19 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/fac_rec .
COPY --from=builder /app/models ./models

# Mount your faces.db here at runtime: -v /path/to/data:/app/data
CMD ["./fac_rec"]
