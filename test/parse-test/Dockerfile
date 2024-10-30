FROM ubuntu:latest

RUN apt-get update
RUN apt-get install build-essential -y
RUN apt-get install cmake -y
RUN apt-get install postgresql -y
RUN apt-get install libpq-dev -y
RUN apt-get install git -y
RUN apt-get install libboost-all-dev -y
WORKDIR /app/deps
RUN git clone https://github.com/jtv/libpqxx.git

WORKDIR /app

COPY . .

RUN cmake . . && cmake --build .

CMD ["./main"]