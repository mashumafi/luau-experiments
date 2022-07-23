FROM alpine

RUN apk add build-base cmake git ninja linux-headers gdb

WORKDIR /code
