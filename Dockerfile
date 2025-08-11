FROM gcc:15 AS base
EXPOSE 9000
WORKDIR /app
RUN apt-get update && apt install -y cmake ninja-build
COPY . .
RUN cmake --preset "x64-release"
RUN cmake --build ./out/build/x64-release
CMD [ "./out/build/x64-release/event-streaming/event-streaming" ]