FROM ubuntu:20.04

RUN apt-get update \
    && apt-get install -y cmake g++ \
    && apt-get clean

COPY . /app
WORKDIR /app

CMD ["./build.sh"]
