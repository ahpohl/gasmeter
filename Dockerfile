FROM alpine:latest

RUN apk add git build-base linux-headers mosquitto-dev

WORKDIR /app

ADD https://api.github.com/repos/ahpohl/gasmeter/git/refs/heads/master gasmeter-version.json
RUN git clone https://github.com/ahpohl/gasmeter.git
RUN cd /app/gasmeter && make PREFIX=/usr install

ENTRYPOINT gasmeter --config /etc/gasmeter.conf
